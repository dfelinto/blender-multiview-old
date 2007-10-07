#ifndef RIBINTERFACE_H
#define RIBINTERFACE_H

#include "ri.h"
#include <string>
#include <vector>

#include "decparam.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// parameter lists
class parameter_t
{
		friend class parameterList_t;
	public:
		// default, unknown class & type, zero array length
		parameter_t():ct_flags(0), arlen(0) {}
		parameter_t(float fv):ct_flags(DT_FLOAT) { float_array.push_back(fv); }
		parameter_t(int iv):ct_flags(DT_INT) { int_array.push_back(iv); }
		parameter_t(char* sv):ct_flags(DT_STRING) {  string_array.push_back(sv); }
		parameter_t(const std::vector<std::string>& strvec);
		parameter_t(const std::vector<float>& fltvec);
		parameter_t(const std::vector<int>& intvec);
		~parameter_t() {}

		// returns value as string
		std::string valueAsString() const;

		// try to get specific type from data, throws error if not expected type
		// but only if type==string, does float -> int conversion, and vice versa
		float getFloat() const;
		int getInt() const;
		std::string getString() const;

		// storage class/data type bit flags & array length, public for simpler access
		// arlen only used for declared params
		int ct_flags, arlen;

		// all types as separate array variables,
		// single value params are just single value arrays
		std::vector<RtFloat> float_array;
		std::vector<std::string> string_array;
		std::vector<int> int_array;
};


//------------------------------------------------------------------------------

class parameterList_t
{
	public:
		parameterList_t():listname("NONE"), tokens(NULL), parms(NULL), num_riparams(0), array_sizes(NULL) {}
		parameterList_t(const std::string &nom):listname(nom), tokens(NULL),
				parms(NULL), array_sizes(NULL) {}
		~parameterList_t() { freeArrays(); }
		void freeArrays()
		{
			// free allocated c strings
			for (std::vector<std::pair<int, RtString*> >::iterator si=cstrings.begin(); si!=cstrings.end(); ++si) {
				for (int i=0; i<si->first; ++i)
					delete[] si->second[i];
				delete[] si->second;
			}
			cstrings.clear();
			// free token array
			if (tokens) {
				for (unsigned int i=0; i<num_riparams; ++i)
					delete[] tokens[i];
				delete[] tokens;
				tokens = NULL;
			}
			// parameter and array size array
			if (parms) { delete[] parms;  parms = NULL; }
			if (array_sizes) { delete[] array_sizes;  array_sizes = NULL; };
		}
		void add(const parameter_t &p) { params.push_back(p); }
		const parameter_t* getParameter(const std::string &name) const;
		void clear() { params.clear(); }
		std::string name() const { return listname; }
		void name(const std::string &_name) { listname=_name; }

		void convertTypes(int n);
		// creates Ri parameter lists
		int makeRiParamList(int n);
		// checks expected length of primitive variable list (geom only),
		// type 'constant' is always size 1
		// update: added facevarying, all guesswork
		bool checkPrimvarList(int numparam, int uniformMax, int varyingMax, int vertexMax, int faceVaryingMax);

		// process the parameterList and call the corresponding Ri_ functions
		// state_hint is used to give hints to allow RiBegin/End to be called
		// -1: begin, 0: process, 1: end
		// basepath is only set at beginning of fileparsing
		std::string process(int state_hint, const char* basepath=NULL);

		typedef std::vector<parameter_t>::const_iterator ParamIterator;
		ParamIterator begin() const { return params.begin(); }
		ParamIterator end() const { return params.end(); }
		size_t size() const { return params.size(); }

		// data
		// the equivalent Ri parameterlist, created by makeRiParamList()
		std::string listname;
		RtToken* tokens;
		RtPointer* parms;
		unsigned int num_riparams;
		unsigned int* array_sizes;
		std::vector<parameter_t> params;
		// this only necessary for C string assignment in makeRiParamList()
		std::vector<std::pair<int, RtString*> > cstrings;
};

// rib function table for rib parser
typedef void (*RIBFUNC)(unsigned int, parameterList_t&);
struct _ribFunc
{
	RtToken name;
	RIBFUNC func;
};
extern _ribFunc ribFuncTable[];

//-------------------------------------------------------------------------------

__END_QDRENDER

#endif // RIBINTERFACE_H
