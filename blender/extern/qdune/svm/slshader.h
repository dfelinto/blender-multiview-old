#ifndef _SLSHADER_H
#define _SLSHADER_H

#include "ri.h"
#include "qdtl.h"
#include "decparam.h"
#include "boolarray.h"
#include <vector>

#include "QDRender.h"
__BEGIN_QDRENDER

// added extra bool type, is only internal shading engine type, not visible to RSL or rib
enum SlType {SL_FLOAT, SL_POINT, SL_COLOR, SL_STRING, SL_VECTOR, SL_NORMAL, SL_MATRIX, SL_BOOL};
enum SlStorage {SL_CONSTANT, SL_PARAMETER, SL_TEMPORARY, SL_GLOBAL};

struct SlSymbol
{
	SlSymbol():token(NULL), default_value(NULL) {}
	~SlSymbol()
	{
		if (token) { delete[] token;  token = NULL; }
		if (default_value)
		{
			// string is allocated separately in array !!
			if (type == SL_STRING)
				delete[] reinterpret_cast<RtString*>(default_value)[0];
			delete[] reinterpret_cast<unsigned long*>(default_value);
			default_value = NULL;
		}
	}
	RtToken       token;          // name of variable
	SlType        type;           // Shader language type
	SlStorage     storage;        // Parameter, constant, etc.
	RtBoolean     isvarying;
	RtInt         arraysize;
	RtVoid       *default_value;
};

struct SlOpcode
{
	RtInt tableOffset;  // opcode table offset
	RtInt operandCount; // number of operands
};

struct SlSymbolWord
{
	RtInt tableOffset; // symbol table offset
	RtInt detailFlag;  // uniform(0)/varying(1)
};

typedef RtInt SlAddress;
union SlCPUWord
{
	SlOpcode      opcode;
	SlSymbolWord  symbol;
	SlAddress     address;
};

struct SlValue
{
	union {
		RtFloat  *floatVal;
		RtVector *vectorVal;	// also used for color/point/normal
		RtMatrix *matrixVal;
		RtString *stringVal;	// note: unlike in p.r.book, here *is* an array, varying strings are not possible, but array of strings are!
		RtVoid   *voidVal;		// this also used for bool type
	};
};

// this is used to initialize the required global grid variables before the shader is executed
// order corresponds to entries of the _sl_access table, see slshader.cpp
// (now includes all variables, not only varying)
enum slGlobalBits {GB_Cs=0x1, GB_Os=0x2, GB_P=0x4, GB_dPdu=0x8, GB_dPdv=0x10,
                   GB_N=0x20, GB_Ng=0x40, GB_u=0x80, GB_v=0x100,
                   GB_du=0x200, GB_dv=0x400, GB_s=0x800, GB_t=0x1000,
                   GB_L=0x2000, GB_Ps=0x4000, GB_Cl=0x8000, GB_Ol=0x10000,
                   GB_E=0x20000, GB_I=0x40000, GB_ncomps=0x80000,
                   GB_time=0x100000, GB_dtime=0x200000, GB_dPdtime=0x400000,
                   GB_alpha=0x800000, GB_Ci=0x1000000, GB_Oi=0x2000000};
typedef unsigned int slGlobals;

// data accessibility table of the standard primitive variables for the various shader types
struct _SlAccess
{
	RtToken name;
	// these are one of 'rwn', read / write / none respectively
	// if it has write access, obviously it is also readable
	char surface, light, displacement, volume, imager;
	SlType type;
	RtBoolean isvarying;
};
extern _SlAccess _sl_access[];

// shader type, SL_NONE only here for init.
enum SlShaderType {SL_NONE, SL_SURFACE, SL_LIGHT, SL_DISPLACEMENT, SL_VOLUME, SL_IMAGER};

class SlShader
{
public:
	// ctor
	SlShader():name(NULL), type(SL_NONE), isAmbient(RI_TRUE), maincode_offset(0), update_Ng(false) {}
	// dtor
	~SlShader()
	{
		if (name) { delete[] name;  name = NULL; }
		for (unsigned i=0; i<symbols.size(); ++i) {
			delete symbols[i];
			symbols[i] = NULL;
		}
	}
	// data
	RtToken name; // shader name
	SlShaderType type;
	std::vector<SlSymbol*> symbols;
	std::vector<SlCPUWord> code; // actual VM code
	// flag for lights, set when no illuminate/solar statements in shader
	RtBoolean isAmbient;
	// if initialization code used, offset to main code
	RtInt maincode_offset;
	// global variables used in the shader
	slGlobals globals_used;
	// this used for displacement shaders which need to update Ng when P is modified
	bool update_Ng;
	// other user-defined *varying* variables that might need dicing (iff also specified as primitive variable)
	NamedLinkedList_t<decParam_t> user_vars;
};

class MicroPolygonGrid;
class Transform;
class SlShaderInstance
{
public:
	// ctor
	SlShaderInstance():shader(NULL), xform(NULL) {}
	// dtor
	~SlShaderInstance();
	// mtds
	bool setParameter(const char* name, SlType type, RtBoolean isvarying, RtInt arraysize, RtVoid *val);
	SlValue* bind(MicroPolygonGrid* grid, bool initcode = false);
	RtVoid unbind(SlValue* data, bool initcode = false);
	RtVoid run(MicroPolygonGrid* grid);
	RtVoid run_initcode();
	// data
	SlShader *shader;
	std::vector<SlSymbol*> callparams;
	// 'shader' space, borrowed reference, do not delete!
	const Transform* xform;
};

class SlRunContext
{
public:
	// ctors
	SlRunContext():PC(0), SIMD_count(0), SIMD_flags(NULL), data(NULL), shdInstance(NULL),
	               grid(NULL), aux_grid(NULL), light_iterator(0) {}
	// dtor
	~SlRunContext()
	{
		if (SIMD_flags) { delete[] SIMD_flags;  SIMD_flags = NULL; }
		// data[] already freed at end of run(), but just in case...
		if (data) { delete data;  data = NULL; }
	}
	// mtds
	SlCPUWord* getOperand(RtInt a) const { return &shdInstance->shader->code[PC + a]; }
	RtInt getOperandCount() const { return getOperand(0)->opcode.operandCount; }
	SlValue& getValue(RtInt a) const { return data[getOperand(a)->symbol.tableOffset]; }
	SlSymbol* getSymbol(RtInt a) const { return shdInstance->shader->symbols[getOperand(a)->symbol.tableOffset]; }
	// was boolean, but more convenient to return integer, to use directly as increment value (1 == varying, 0 == uniform)
	// (RtBoolean is int anyway, so really only the name has changed)
	RtInt isVarying(RtInt a) const { return getOperand(a)->symbol.detailFlag; }
	// data access
	RtFloat* getFloat(RtInt a) const { return getValue(a).floatVal; }
	// vector also used for color/normal/point
	RtVector* getVector(RtInt a) const { return getValue(a).vectorVal; }
	RtMatrix* getMatrix(RtInt a) const { return getValue(a).matrixVal; }
	RtString* getString(RtInt a) const { return getValue(a).stringVal; }
	boolarray_t* getBoolean(RtInt a) const { return reinterpret_cast<boolarray_t*>(getValue(a).voidVal); }
	RtVoid* getVoid(RtInt a) const { return getValue(a).voidVal; }
	bool active(RtInt i) const { return (SIMD_flags[i] == 0); }
	// see comment at start of shaderVM.cpp for explanation of this function
	bool some_active() const
	{
		for (RtInt i=0; i<SIMD_count; ++i)
			if (SIMD_flags[i] == 0) return true;
		return false;
	}
	RtVoid enable(RtInt i) { if (SIMD_flags[i]) SIMD_flags[i]--; }
	RtVoid disable(RtInt i) { SIMD_flags[i]++; }
	RtVoid nextInstruction() { PC += getOperandCount()+1; }
	RtInt getAddress(RtInt a) const { return getOperand(a)->address; }
	// simd state using activity counter
	RtVoid SIMD_init();
	RtVoid SIMD_reset();
	RtVoid SIMD_push(boolarray_t& cond);
	RtVoid SIMD_pop();
	RtVoid SIMD_else();
	
	// data, all public for direct access by VM
	RtInt PC; // programcounter
	RtInt SIMD_count;
	RtInt* SIMD_flags;
	SlValue* data;
	SlShaderInstance* shdInstance;
	// these needed for running lightshaders
	MicroPolygonGrid *grid, *aux_grid;
	unsigned int light_iterator;
};

// creates the virtual machine code from the given sourcecode string
SlShader* parseVMsource(const char* scode, SlShaderType expected_type);

__END_QDRENDER

#endif // _SLSHADER_H
