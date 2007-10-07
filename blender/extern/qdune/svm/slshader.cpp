/// -------------------------------------------------------------------------------------
///  Shader assembler source parser, initialization, binding and code execution handling
/// -------------------------------------------------------------------------------------
///
/// Special symbols used: '#' -> comment
///                       '$' -> temporary 'register', $ + possibly some characters, followed by number
///                       '@' -> (+number) label
///
/// The structure of a sqd file is:
/// one statement per line, if line begins with '#', it is a comment,
/// and if it begins with '@' (only used in code segment), it's a label.
/// Labels must be separate from code, there can be no code on the same line as a label
///
/// general layout:
/// --- shader type (surface/light/etc) followed by shader name
/// --- parameter and variable definitions, either 'param', 'temp' or 'const'.
///     constants are nothing but uniform temporaries, and they are not quite 'constant' either, it is actually possible to assign to them...
///     parameters are writable as well, but in shading language that is really only legal if it is declared as 'output'.
/// --- global variables used (the standard primitive variables used by the shader)
/// --- 'codesegment' symbol followed by optional label,
///     if it has a label, code up to that label is run once to initialize parameter defaults when the shader is loaded,
///     the label designates the start of the main code segment.
/// --- VM assembler code
/// --- 'return' symbol
///
/// Comments can be on any line.
/// Temporary 'register' variables can really be any name, but compiler generated code will use '$' followed by number (possibly some other chars inbetween)
/// Any operand that does not start with an alphabetic character will be interpreted as an immediate value.
/// These are just handled as constants in disguise, see parseVMsource().
/// This feature is only useful for handcoded shaders, compiler will always use constants.
///
/// Lots of checks/assertions used here, VM code errors are not acceptable !!!
///

// For compiler generated code could just as well use direct bytecodes,
// would speedup & simplify loading a lot.
// But for now it's nice to have a somewhat readable format.

#include "slshader.h"
#include "shaderVM.h"
#include "MicroPolygonGrid.h"
#include "State.h"
#include "ImageTextures.h"
#include <cstdio>
#include <cassert>

__BEGIN_QDRENDER

// used to test that operands are of the expected type for current operator in parseVMsource().
// directly corresponds to SlType enum, see slshader.h
static char SlTypeSymbol[] = {'f', 'p', 'c', 's', 'v', 'n', 'm', 'b'};

// Data accessibility table of the standard primitive variables
// for the surface, light, displacement, volume & imager shaders respectively.
// n -> none, not available
// r -> read only
// w -> both write and read
// NOTE: the accessiblity flags are actually not used at all here,
// this is more a task for the compiler, so that for instance when an attempt
// is made to modify a readonly variable in the sourcecode, the compiler will either reject the code,
// or for some cases create a temporary copy so that the variable itself is not touched, etc. 
// ...not sure what to do exactly yet though, since assembler code could still be abused, which defeats all that...
_SlAccess _sl_access[] =
{
	{"Cs",      'r', 'n', 'n', 'n', 'n', SL_COLOR, RI_TRUE},
	{"Os",      'r', 'n', 'n', 'n', 'n', SL_COLOR, RI_TRUE},
	{"P",       'r', 'r', 'w', 'r', 'r', SL_POINT, RI_TRUE},
	{"dPdu",    'r', 'r', 'r', 'n', 'n', SL_VECTOR, RI_TRUE},
	{"dPdv",    'r', 'r', 'r', 'n', 'n', SL_VECTOR, RI_TRUE},
	{"N",       'r', 'r', 'w', 'n', 'n', SL_NORMAL, RI_TRUE},
	{"Ng",      'r', 'r', 'r', 'n', 'n', SL_NORMAL, RI_TRUE},
	{"u",       'r', 'r', 'r', 'n', 'n', SL_FLOAT, RI_TRUE},
	{"v",       'r', 'r', 'r', 'n', 'n', SL_FLOAT, RI_TRUE},
	{"du",      'r', 'r', 'r', 'n', 'n', SL_FLOAT, RI_TRUE},
	{"dv",      'r', 'r', 'r', 'n', 'n', SL_FLOAT, RI_TRUE},
	{"s",       'r', 'r', 'r', 'n', 'n', SL_FLOAT, RI_TRUE},
	{"t",       'r', 'r', 'r', 'n', 'n', SL_FLOAT, RI_TRUE},
	{"L",       'r', 'r', 'n', 'n', 'n', SL_VECTOR, RI_TRUE},
	{"Ps",      'n', 'r', 'n', 'n', 'n', SL_POINT, RI_TRUE},
	{"Cl",      'r', 'w', 'n', 'n', 'n', SL_COLOR, RI_TRUE},
	{"Ol",      'r', 'w', 'n', 'n', 'n', SL_COLOR, RI_TRUE},
	{"E",       'r', 'r', 'r', 'r', 'n', SL_POINT, RI_FALSE},
	{"I",       'r', 'n', 'r', 'r', 'n', SL_VECTOR, RI_TRUE},
	{"ncomps",  'r', 'r', 'r', 'r', 'r', SL_FLOAT, RI_FALSE},
	{"time",    'r', 'r', 'r', 'r', 'r', SL_FLOAT, RI_FALSE},
	{"dtime",   'r', 'r', 'r', 'r', 'r', SL_FLOAT, RI_FALSE},
	{"dPdtime", 'r', 'n', 'r', 'n', 'n', SL_FLOAT, RI_TRUE},
	{"alpha",   'n', 'n', 'n', 'n', 'r', SL_FLOAT, RI_FALSE},
	{"Ci",      'w', 'n', 'n', 'w', 'w', SL_COLOR, RI_TRUE},
	{"Oi",      'w', 'n', 'n', 'w', 'w', SL_COLOR, RI_TRUE},
	{NULL, 0, 0, 0, 0, 0, SL_FLOAT, 0}	// SL_FLOAT does not mean anything here, just there to make compiler happy
};


// helper functions, returns new copy of a c string
inline char* newCopy_CString(const char* cstr)
{
	assert(cstr != NULL);
	size_t L = strlen(cstr);
	char* ncstr = new char[L + 1];
	strcpy(ncstr, cstr);
	return ncstr;
}

// returns new partial copy of a c string
inline char* newCopyN_CString(const char* cstr, size_t len)
{
	assert(cstr != NULL);
	char* ncstr = new char[len + 1];
	strncpy(ncstr, cstr, len);
	ncstr[len] = 0;
	return ncstr;
}

// 'global' is handled separately, since it just is a single line of all used glob. var. names
// sets the combined bits of globals used in 'globits'
void parseGlobal_SL(char* declaration, std::vector<SlSymbol*>& symtab, unsigned int& globits)
{
	if ((declaration==NULL) || (*declaration==0)) return; // just in case...
	char globname[256] = {0};
	char* sp = declaration + 6 + ((declaration[0]=='d') ? 1 : 0);
	while (*sp && isspace(*sp)) sp++; // skip ws
	if (*sp==0) {
		printf("[ERROR]: parseGlobal_SL() -> no variable names found in string '%s'\n", declaration);
		exit(1);
	}

	globits = 0;

	for (;;) {
		int scanf_ok = sscanf(sp, "%s", globname);
		if (scanf_ok==0) {
			printf("[ERROR]: parseGlobal_SL() -> unexpected error while scanning for global varname\n");
			exit(1);
		}

		// add new symbol
		SlSymbol* sm = new SlSymbol();
		symtab.push_back(sm);
		sm->token = newCopy_CString(globname);
		sm->storage = SL_GLOBAL;
		// type/arraysize undefined here
		// lookup if varying and also set type
		// and the corresponding slGlobals bit (which corresponds directly to varname position in _SlAccess table)
		unsigned int sa_idx = 0;
		while (_sl_access[sa_idx].name) {
			if (!strcmp(_sl_access[sa_idx].name, sm->token)) {
				sm->isvarying = _sl_access[sa_idx].isvarying;
				sm->type = _sl_access[sa_idx].type;
				globits |= (1 << sa_idx);
				break;
			}
			++sa_idx;
		}
		sm->default_value = NULL;

		// next name
		sp += strlen(globname);
		while (*sp && isspace(*sp)) sp++; // skip ws
		if (*sp==0) break;
	}

}

// scan known tokens one by one in expected order
// not sensitive to any spacing, so 'floatkd1.23' is actually parsed correctly,
// probably shouldn't allow that though...
bool parseDeclaration_SL(char* declaration, std::vector<SlSymbol*>& symtab)
{
	if ((declaration==NULL) || (*declaration==0)) return false; // just in case...

	SlSymbol* sm = new SlSymbol();
	symtab.push_back(sm);

	// init ptrs to NULL
	sm->token = NULL;
	sm->default_value = NULL;

	char* s = declaration;
	// skip whitespace, if any
	while (*s && isspace(*s)) s++;
	if (*s==0) {
		printf("[ERROR]: parseDeclaration_SL() -> unexpected end of string '%s'\n", declaration);
		return false;
	}

	// storage type
	if (!strncmp(s, "temp", 4))
		{ sm->storage = SL_TEMPORARY;  s += 4; }
	else if (!strncmp(s, "param", 5))
		{ sm->storage = SL_PARAMETER;  s += 5; }
	else if (!strncmp(s, "const", 5))
		{ sm->storage = SL_CONSTANT;  s += 5; }
	else // 'param' assumed
		sm->storage = SL_PARAMETER;

	// skip whitespace
	while (*s && isspace(*s)) s++;
	if (*s==0) {
		printf("[ERROR]: parseDeclaration_SL() -> unexpected end of string '%s'\n", declaration);
		return false;
	}

	// detail
	bool varset = false; // used later to warn if varying was set with string type, see below
	if (!strncmp(s, "varying", 7)) {
		if (sm->storage == SL_CONSTANT) { // 'const' always uniform
			printf("[ERROR]: parseDeclaration_SL() -> 'const' cannot be varying\n");
			return false;
		}
		sm->isvarying = RI_TRUE;  s += 7;
		varset = true;
	}
	else if (!strncmp(s, "uniform", 7))
		{ sm->isvarying = RI_FALSE;  s += 7; }
	else {
		// by default, if nothing specified, everything is varying,
		// exception is 'const' which is always uniform, and 'param' which is assumed to be uniform by default
		sm->isvarying = ((sm->storage == SL_CONSTANT) || (sm->storage == SL_PARAMETER)) ? RI_FALSE : RI_TRUE;
	}

	// skip whitespace
	while (*s && isspace(*s)) s++;
	if (*s==0) {
		printf("[ERROR]: parseDeclaration_SL() -> unexpected end of string '%s'\n", declaration);
		return false;
	}

	// parse data type
	if (!strncmp(s, "float", 5))
		{ sm->type = SL_FLOAT;  s += 5; }
	else if (!strncmp(s, "bool", 4))	// extra type for conditional ops
		{ sm->type = SL_BOOL;  s += 4; }
	else if (!strncmp(s, "point", 5))
		{ sm->type = SL_POINT;  s += 5; }
	else if (!strncmp(s, "color", 5))
		{ sm->type = SL_COLOR;  s += 5; }
	else if (!strncmp(s, "string", 6))
	{
		sm->type = SL_STRING;  s += 6;
		// if string declared as 'varying', ignore, impossible, warn
		if (sm->isvarying) {
			// only warn if it was explicitely specified, otherwise silently change to uniform
			if (varset) printf("[WARNING]: parseDeclaration_SL() -> 'string' type cannot be varying\n");
			sm->isvarying = RI_FALSE;
		}
	}
	else if (!strncmp(s, "vector", 6))
		{ sm->type = SL_VECTOR;  s += 6; }
	else if (!strncmp(s, "normal", 6))
		{ sm->type = SL_NORMAL;  s += 6; }
	else if (!strncmp(s, "matrix", 6))
		{ sm->type = SL_MATRIX;  s += 6; }
	else {
		// nothing recognized, return error
		printf("[ERROR]: parseDeclaration_SL() -> unknown variable type '%s'\n", s);
		return false;
	}

	// skip more whitespace
	while (*s && isspace(*s)) s++;

	// preset arraysize to 1 here, also indicates single values
	sm->arraysize = 1;

	// if string ends here, return false,
	// since name and possible default value is required
	if (*s==0) {
		printf("[ERROR]: parseDeclaration_SL() -> expected variable name and/or default value\n");
		return false;
	}

	// parameter name
	// For shader params, first character must be alpha or underscore,
	// following chars may also be digits.
	// Compiler generated temporary var names as well as constants always start with '$' followed by alphanum chars,
	// (unless generated from sourcecode variables itself)
	const char* s2 = s;
	if (*s=='$') {
		if (sm->storage != SL_CONSTANT) {
			if (sm->storage != SL_TEMPORARY) {
				printf("[ERROR]: parseDeclaration_SL() -> only 'const' and 'temp' types can use '$'\n");
				return false;
			}
		}
		// name alphanum chars
		s++;	// skip '$'
		while (*s && isalnum(*s)) s++;
	}
	else {
		if (!(isalpha(*s) || (*s=='_'))) {
			printf("[ERROR]: parseDeclaration_SL() -> var.name '%s' must start with a letter, digit or underscore\n", s2);
			return false;
		}
		// find end of name
		while (isalnum(*s) || (*s=='_')) s++;
	}
	const int len = (int)(s - s2);
	// 256 chars max
	if (len > 256) {  // no more than 256 chars
		printf("[ERROR]: parseDeclaration_SL() -> variable name too long (256 chars max)\n");
		return false;
	}
	if (len < 1) { // empty string? (or no alpha chars anyway)
		printf("[ERROR]: parseDeclaration_SL() -> expected variable name, nothing found\n");
		return false;
	}
	sm->token = newCopyN_CString(s2, len);

	// parse array length, if any
	if (*s=='[') {
		s++; // skip start bracket
		const char* s2 = s; // start of number, conversion below
		// syntax check 1, check contents are digits only
		while (*s && isdigit(*s)) s++;
		// syntax check 2, find end bracket, only whitespace allowed until found
		while (*s && (*s!=']')) {
			if (!isspace(*s)) {
				printf("[ERROR]: parseDeclaration_SL() -> syntax error in array declaration\n");
				return false;
			}
			s++;
		}
		if (*s!=']') { // no end bracket, return error
			printf("[ERROR]: parseDeclaration_SL() -> missing end bracket in array declaration\n");
			return false;
		}
		// set array length from string
		// here s2 is complete string starting from first digit, but conversion 'should' work anyway...
		sm->arraysize = atoi(s2);
		s++; // skip end bracket
	}

	// parse the value(s)
	while (*s && isspace(*s)) s++;
	if (*s==0) {
		// if 'temp' and string ends, is valid
		if (sm->storage == SL_TEMPORARY) {
			return true;
		}
		// otherwise, error, need default value for 'param'
		printf("[ERROR]: parseDeclaration_SL() -> missing default value in '%s'\n", declaration);
		return false;
	}
	else if (sm->storage == SL_TEMPORARY) {
		// nothing allowed after name if 'temp', initialize must be done in code if needed
		printf("[ERROR]: parseDeclaration_SL() -> 'temp' cannot have more arguments after variable name\n");
		return false;
	}

	if (*s=='\"') {
		// string
		char lastc = *s;
		s++;
		char* s2 = s;
		// find end of string (taking possible quote as escape char (\") in string into account)
		while (*s) {
			if ((*s=='\"') && (lastc!='\\')) break;
			lastc = *s;
			s++;
		}
		if (*s==0) { // no end of string
			printf("[ERROR]: parseDeclaration_SL() -> missing end quote in string declaration\n");
			return false;
		}
		const int len = (int)(s - s2);
		// must be allocated as array
		RtString* nstr = new RtString[1];
		nstr[0] = newCopyN_CString(s2, len);
		sm->default_value = nstr;
		return true; // string can only have one value (actually not true, can be array, TODO)
	}

	// assume number(s) from this point

	// Parameters can also be varying, but since the grid size is not known yet, can only initialize as uniform,
	// and then only at the binding stage re-initialize to the full varying default (if var. not found on grid),
	// not particularly efficient, but will have to do for now...
	int ofs = 1; // parameter offset, 1 for float (def.), 3 for color/point/vec/norm, 16 for mtx
	if ((sm->type == SL_POINT) || (sm->type == SL_COLOR) ||
			(sm->type == SL_VECTOR) || (sm->type == SL_NORMAL))
		ofs = 3;
	else if (sm->type == SL_MATRIX)
		ofs = 16;
	int numf = sm->arraysize * ofs;
	float* fa = new float[numf];
	sm->default_value = (void*)fa;
	for (int i=0; i<numf; i++) {
		// strtod instead of atof, so errors can be detected
		// find str.len. of current token in string (s is complete string)
		char* strend = s;
		while (*strend && !isspace(*strend)) strend++;
		// convert
		char* erp;
		fa[i] = (float)strtod(s, &erp);
		bool convert_error = ((erp - s) < (s2 - s));
		assert(!convert_error); // no errors accepted
		while (*s && (!isspace(*s))) s++; // to next number
		while (*s && isspace(*s)) s++; // skip ws
		if (i != (numf-1)) {
			// error if string ends early
			if (*s==0) {
				printf("[ERROR]: parseDeclaration_SL() -> expected %d default arguments in '%s'\n", numf, declaration);
				return false;
			}
		}
	}

	// warn if still more non-whitespace string left
	bool tc = false;
	while (*s) {
		if (!isspace(*s)) tc = true;
		s++;
	}
	if (tc) printf("[WARNING] trailing characters ignored in '%s'\n", declaration);

	return true;
}

// lookup instruction and fill in the offset and operandcount from the opcode table
// returns false if not found
// TODO use hashtable here
bool getOpcodeOffset(const char* name, SlCPUWord& cpuw)
{
	int ofs = 0;
	while (opcodeTable[ofs].name!=NULL) {
		if (!strcmp(opcodeTable[ofs].name, name)) {
			cpuw.opcode.tableOffset = ofs;
			cpuw.opcode.operandCount = (opcodeTable[ofs].types==NULL) ? 0 : (RtInt)strlen(opcodeTable[ofs].types);
			return true;
		}
		ofs++;
	}
	return false;
}


// helper function, check expected symbol type is correct, if not, exit
inline void isTypeCorrect(const char* line, char expected_tsym, char given_tsym)
{
	// exception, the 'v', 'c', 'p' or 'n' can be used interchangebly, since all are float[3] arrays.
	// (see shaderVM opcode table explanation)
	// Of course, if the result makes any sense depends on the operator (it may be specialized
	// for the expected type), but that is something the programmer/compiler has to take care of...
	if ((expected_tsym == 'v') || (expected_tsym == 'c') || (expected_tsym == 'p') || (expected_tsym == 'n'))
	{
		if ((given_tsym != 'v') && (given_tsym != 'c') && (given_tsym != 'p') && (given_tsym != 'n'))
		{
			printf("[ERROR]: parseVMsource() '%s' -> expected operator type of '%c', but got '%c' instead\n",
			        line, expected_tsym, given_tsym);
			exit(1);
		}
	}
	else if (given_tsym != expected_tsym) {
		printf("[ERROR]: parseVMsource() '%s' -> expected operator type of '%c', but got '%c' instead\n",
		       line, expected_tsym, given_tsym);
		exit(1);
	}
}

// creates the virtual machine bytecode from the given ascii sourcecode string
SlShader* parseVMsource(const char* scode, SlShaderType expected_type)
{
	const size_t slen = strlen(scode);
	if (slen==0) return NULL;

	SlShader* SH = new SlShader();

	// keep track of label locations
	std::map<int, int> labels;
	// also keep track of labels used for forward jumps
	std::multimap<int, int> futurelabels;

	bool has_initcode = false, initcodeseg = false;
	int initcode_LNUM = -1; // initcode label number for maincode segment

	char sbuf[1024];
	unsigned int lcnt = 0, cnt = 0;
	enum {STYPE, DATA, CODE} state = STYPE;
	int data_symbolsize = 0;
	for (;;) {
		// read line
		int strq = 0;  // count single \" so that esc.seq in string decl. are not mistaken for line breaks etc.
		char lastc = 0;
		bool initws = true; // skip initial whitespace
		while (cnt < slen) {
			char c = scode[cnt++];
			if (initws && isspace(c)) continue;
			initws = false;
			if ((c=='\"') && (lastc!='\\')) strq++;
			if (c=='\n' && ((strq & 1)!=1)) break;
			lastc = c;
			sbuf[lcnt++] = c;
			assert(lcnt < 1024);
		}
		sbuf[lcnt] = 0;
		lcnt = 0;

		// convert line to code
		if (sbuf[0]=='#') continue; // skip comments

		if (state == STYPE) {
			char* sp = sbuf;
			const char* tpname[5] = {"surface", "light", "displacement", "volume", "imager"};
			if (!strncmp(sp, tpname[0], 7))
				{ SH->type = SL_SURFACE;  sp += 7; }
			else if (!strncmp(sp, tpname[1], 5))
				{ SH->type = SL_LIGHT;  sp += 5; }
			else if (!strncmp(sp, tpname[2], 12))
				{ SH->type = SL_DISPLACEMENT;  sp += 12; }
			else if (!strncmp(sp, tpname[3], 6))
				{ SH->type = SL_VOLUME;  sp += 6; }
			else if (!strncmp(sp, tpname[4], 6))
				{ SH->type = SL_IMAGER;  sp += 6; }
			else {
				// shader type not recognized??? should never happen
				printf("[ERROR]: parseVMsource() -> unknown shader type '%s'\n", sbuf);
				delete SH;
				return NULL;
			}
			// is it the correct expected shader type?
			if (SH->type != expected_type) {
				assert(expected_type != SL_NONE);	// just in case...
				printf("[ERROR]: parseVMsource() -> incorrect shader type: '%s', expected '%s'\n", sbuf, tpname[int(expected_type)-1]);
				delete SH;
				return NULL;
			}
			while (*sp && isspace(*sp)) sp++; // skip ws
			if (*sp==0) {
				printf("[ERROR]: parseVMsource() -> unexpected end of string\n");
				delete SH;
				return NULL;
			}
			// get name of shader
			SH->name = newCopy_CString(sp);
			state = DATA;
			continue;
		}

		if (state == DATA)
		{
			// variable declarations
			if (!strncmp(sbuf, "codesegment", 11)) {
				// code segment is next
				state = CODE;
				// additional 'immediate' mode vars have no name, and since name is used for operand lookup,
				// save the current symbol table size so lookup is only done for 'real' symbols.
				data_symbolsize = (int)SH->symbols.size();
				// get possible codesegment label to start of main code
				char* ins = sbuf+11;
				while (*ins && isspace(*ins)) ins++; // skip ws
				// if no argument, there is no initialization code
				if (*ins != 0) {
					if (ins[0] != '@') {
						printf("[ERROR]: parseVMsource() -> 'codesegment' argument is not a label ->'%s'\n", ins);
						delete SH;
						return NULL;
					}
					ins++;
					const int Lnum = atoi(ins);
					initcode_LNUM = Lnum;
					// by definition this is a 'future' label, resolve after code scan completion
					futurelabels.insert(std::make_pair(Lnum, 0)); // since there is no code yet, offset is always 0 at this point
					has_initcode = initcodeseg = true;
				}
				continue;
			}
			// try to parse the data declaration string
			if (!strncmp(sbuf, "global", 5))
				parseGlobal_SL(sbuf, SH->symbols, SH->globals_used);
			else if (!parseDeclaration_SL(sbuf, SH->symbols)) {
				printf("data declaration error in '%s'\n", sbuf);
				// not acceptable, so exit
				delete SH;
				return NULL;
			}
		}

		if (state == CODE)
		{
			// code segment, parse instructions
			char* cp = sbuf;

			if (cp[0]=='@') {
				// current line is label, mark for later
				int Lnum = atoi(cp + 1);
				// if maincode label reached, storage type access checking is not necessary anymore
				if (has_initcode && initcode_LNUM==Lnum) initcodeseg = false;
				labels[Lnum] = (unsigned int)SH->code.size();
				continue;
			}

			if (!strncmp(cp, "return", 6)) {
				// end of code, no opcode for return (yet, later might be needed for user functions, but probably not)
				// so can just break here
				break;
			}

			// find first whitespace, set string terminator and lookup instruction,
			// remainder of string later used to parse the operands that follow
			char* opernd = cp;
			while (*opernd && !isspace(*opernd)) opernd++;
			// can already be end of string if instruction has no operands
			if (*opernd) *opernd = 0;
			SlCPUWord cpuw = {0, 0};
			// lookup instruction
			bool opfound = getOpcodeOffset(cp, cpuw);
			if (!opfound) {
				printf("[ERROR]: parseVMsource() -> unknown instruction '%s'\n", cp);
				delete SH;
				return NULL;
			}
			
			// opcode expected argument typestring
			const char* opcode_types = opcodeTable[cpuw.opcode.tableOffset].types;
			const size_t optypes_len = opcode_types ? strlen(opcode_types) : 0;	// typestring can be NULL if opcode does not require args
			
			// for light shaders, as soon as a solar() or illuminate() instruction is found,
			// the ambient flag is set to false
			if ((!strncmp(cp, "solar", 5)) || (!strncmp(cp, "illuminate", 10)))
				SH->isAmbient = RI_FALSE;

			// add new cpuword to code table
			SH->code.push_back(cpuw);

			// continue if this instruction has no operands
			if (cpuw.opcode.operandCount == 0) continue;

			// parse operands, calculating offsets for symbol table
			opernd++; // skip string terminator set above
			while (*opernd && isspace(*opernd)) opernd++; // skip ws
			if (*opernd==0) {
				printf("[ERROR]: parseVMsource() -> no operands found in string '%s'\n", sbuf);
				delete SH;
				return NULL;
			}

			// current operand name
			char opname[256] = {0};

			// variadic functions have unknown type & number of args
			// so are handled separately here
			if (opcode_types[optypes_len - 1]=='u') {
				// get all args
				int numargs = 0;
				// weird errors happened here, took a very long time to find a solution...
				// for some reason when using g++ (it's probably perfectly reasonable, but I don't have a clue)
				// *directly* trying to increment the operand count with the code:
				//		SH->code[last_cpuw_idx].opcode.operandCount++;
				// causes segmentation faults...
				// No problems however if first counting the args and then reset it afterwards...
				// It is not an optimization issue, since it also happens without optimization enabled...
				const size_t last_cpuw_idx = SH->code.size() - 1;	// to set the opcode operand count
				for (;;) {
					int scanf_ok = sscanf(opernd, "%s", opname);
					if (scanf_ok==0) {
						// no errors accepted
						printf("[ERROR]: parseVMsource() -> could not find operand name in '%s'\n", opernd);
						delete SH;
						return NULL;
					}
					// lookup symbol name
					bool found = false;
					for (int sto=0; sto<data_symbolsize; ++sto) {
						if (!strcmp(SH->symbols[sto]->token, opname)) {
							found = true;
							// for the initialization code segment, since no grid defined yet, global vars cannot be accessed
							if (initcodeseg) {
								if (SH->symbols[sto]->storage == SL_GLOBAL) {
									printf("[ERROR]: parseVMsource() -> initcode can not access 'global' variables\n");
									delete SH;
									return NULL;
								}
							}
							cpuw.symbol.tableOffset = sto;
							cpuw.symbol.detailFlag = SH->symbols[sto]->isvarying;
							SH->code.push_back(cpuw);
							if (numargs < ((int)optypes_len - 1)) {
								// not yet a variadic argurment, make sure argtype is in fact of the expected type
								isTypeCorrect(sbuf, opcode_types[numargs], SlTypeSymbol[SH->symbols[sto]->type]);
								// if this is a displacement shader, and if this is the first operand, type is 'p' or 'v',
								// and the name of the operand is 'P'. then set the update_Ng flag to true
								if ((SH->type == SL_DISPLACEMENT) and (numargs == 0)
								   and ((opcode_types[0] == 'p') or (opcode_types[0] == 'v'))
								   and (!strcmp(opname, "P")))
									SH->update_Ng = true;
							}
							break;
						}
					}
					if (!found) {
						// symbol must be known
						printf("[ERROR]: parseVMsource() -> unknown symbol '%s'\n", opname);
						delete SH;
						return NULL;
					}
					numargs++;
					// to next operand in string, if any
					opernd += strlen(opname);
					while (*opernd && isspace(*opernd)) opernd++; // skip ws
					if (*opernd==0) break;
				}
				// reset operand count to correct size
				SH->code[last_cpuw_idx].opcode.operandCount = numargs;
				// continue to next line
				continue;
			}

			// known argument count
			int maxop = cpuw.opcode.operandCount;

			// cpuw reset, doesn't matter which members, is union
			cpuw.symbol.tableOffset = 0;
			cpuw.symbol.detailFlag = 0;
			for (int oprnum=0; oprnum<maxop; ++oprnum) {
				int scanf_ok = sscanf(opernd, "%s", opname);
				if (scanf_ok==0) {
					// no errors accepted
					printf("[ERROR]: parseVMsource() -> could not find operand name in '%s'\n", opernd);
					delete SH;
					return NULL;
				}

					// if this is a displacement shader, and if this is the first operand, type is 'p' or 'v',
					// and the name of the operand is 'P'. then set the update_Ng flag to true
					if ((SH->type == SL_DISPLACEMENT) and (oprnum == 0)
					   and ((opcode_types[0] == 'p') or (opcode_types[0] == 'v'))
					   and (!strcmp(opname, "P")))
						SH->update_Ng = true;

				if (opname[0]=='@') {
					// label argument (branching instructions)
					int Lnum = atoi(opname+1);
					std::map<int, int>::const_iterator L = labels.find(Lnum);
					if (L != labels.end()) {
						// label is known (backward jump), so can be directly set
						cpuw.address = L->second;
					}
					else {
						// label not defined yet, save it, resolve after code scan completion
						futurelabels.insert(std::make_pair(Lnum, (int)SH->code.size()));
						// insert empty SlCPUWord here, actual jump offset filled in later
						cpuw.address = 0;
					}
					SH->code.push_back(cpuw);
					// since there can be only one label argument and also a final operand, exit loop now
					break;
				}

				// lookup and calculate symbol table offset
				bool found = false;

				// Exception if operand name does not start with an alpha character,
				// These are assumed to be an immediate value and handled as constants in disquise.
				// A nameless symbol is added to the table in this case.
				// Currently only works for floats or strings.
				if (!((isalpha(opname[0])) || (opname[0]=='_') || (opname[0]=='$')))
				{
					SlSymbol* imsym = new SlSymbol();
					imsym->token = NULL; // nameless
					imsym->storage = SL_CONSTANT;
					imsym->isvarying = RI_FALSE;
					imsym->arraysize = 1;
					if (isdigit(opname[0]) || opname[0]=='.' || opname[0]=='-') {
						imsym->type = SL_FLOAT;
						char* erp;
						RtFloat* fa = new RtFloat[1];
						imsym->default_value = fa;
						fa[0] = (float)strtod(opname, &erp);
						bool convert_error = ((erp - opname) < (int)strlen(opname));
						if (convert_error) {
							printf("[ERROR]: parseVMsource() -> Conversion error in '%s' ('%s' %d)\n", opname, erp, (int)strlen(opname));
							exit(1); // no errors accepted
						}
						isTypeCorrect(sbuf, opcode_types[oprnum], 'f');
					}
					else if (opname[0] == '"') {
						imsym->type = SL_STRING;
						RtString* ca = new RtString[1];
						char* endq = opname+1;
						while (*endq && (*endq != '"')) endq++;
						if (*endq == 0) {
							printf("[ERROR]: parseVMsource() -> no string endquote found in '%s'\n", opname);
							exit(1);
						}
						const size_t len = endq - (opname+1);
						ca[0] = newCopyN_CString(opname+1, len);
						imsym->default_value = ca;
						isTypeCorrect(sbuf, opcode_types[oprnum], 's');
					}
					else {
						printf("[ERROR]: parseVMsource() -> '%s'??? (only string or float allowed as immediate value)\n", opname);
						exit(1);
					}
					// no need to check for initialization code exception, since this is a constant type
					SH->symbols.push_back(imsym);
					int symofs = (int)SH->symbols.size()-1;
					cpuw.symbol.tableOffset = symofs;
					cpuw.symbol.detailFlag = SH->symbols[symofs]->isvarying;
					SH->code.push_back(cpuw);
					found = true;
				}
				else {
					// regular addressing mode lookup
					for (int sto=0; sto<data_symbolsize; ++sto) {
						if (!strcmp(SH->symbols[sto]->token, opname)) {
							found = true;
							// make sure argtype is in fact of the expected type
							isTypeCorrect(sbuf, opcode_types[oprnum], SlTypeSymbol[SH->symbols[sto]->type]);
							cpuw.symbol.tableOffset = sto;
							cpuw.symbol.detailFlag = SH->symbols[sto]->isvarying;
							SH->code.push_back(cpuw);
							// for the initialization code segment, since no grid defined yet, global vars cannot be accessed
							if (initcodeseg) {
								if (SH->symbols[sto]->storage == SL_GLOBAL) {
									printf("[ERROR]: parseVMsource() -> initcode can not access 'global' variables\n");
									delete SH;
									return NULL;
								}
							}
							break;
						}
					}
				}
				if (!found) {
					// symbol must be known
					printf("[ERROR]: parseVMsource() -> unknown symbol '%s'\n", opname);
					delete SH;
					return NULL;
				}

				opernd += strlen(opname);
				while (*opernd && isspace(*opernd)) opernd++; // skip ws
				if (oprnum != (maxop-1))
					if (*opernd==0) {
						// missing operands
						printf("[ERROR]: parseVMsource() -> expected %d operands, found %d in '%s'\n", maxop, oprnum+1, sbuf);
						delete SH;
						return NULL;
					}
				if (oprnum == (maxop-1)) {
					// test possibly opposite case, too many operands
					while (*opernd && isspace(*opernd)) opernd++; // skip ws
					// still more left? than error, too many operands
					if (strlen(opernd) > 0) {
						printf("[ERROR]: parseVMsource() -> too many operands in '%s', required %d\n", sbuf, maxop);
						delete SH;
						return NULL;
					}
				}
			}

		}

		if (cnt >= slen) break;
	}

	// correct forward jumps, if any
	for (std::multimap<int, int>::const_iterator mi=futurelabels.begin(); mi!=futurelabels.end(); ++mi)
	{
		std::map<int, int>::const_iterator L = labels.find(mi->first);
		assert(L != labels.end()); // all labels must be known
		if (has_initcode && (mi->second == 0)) // if initcode & initial code offset 0 -> maincode address
			SH->maincode_offset = L->second;
		else // otherwise it's a jmp address
			SH->code[mi->second].address = L->second;
	}

	return SH;
}

//------------------------------------------------------------------------------
// SlShaderInstance

SlShaderInstance::~SlShaderInstance()
{
	// don't delete shader! borrowed reference !!
	for (unsigned i=0; i<callparams.size(); ++i) {
		delete callparams[i];
		callparams[i] = NULL;
	}
}


SlValue* SlShaderInstance::bind(MicroPolygonGrid* grid, bool initcode)
{
	std::vector<SlSymbol*>& symbols = shader->symbols;
	const size_t symsz = symbols.size();
	SlValue* data;

	data = new SlValue[symsz];
	for (unsigned int i=0; i<symsz; ++i)
	{
		SlStorage stp = symbols[i]->storage;
		// for initcode, skip SL_GLOBAL type (there is no grid yet)
		// same is true for any varying var, but these are initialized as uniforms and expanded to varying later, see below
		if (initcode and (stp == SL_GLOBAL)) continue;
		switch (stp)
		{
			case SL_CONSTANT:
			{
				data[i].voidVal = symbols[i]->default_value;
				break;
			}
			case SL_PARAMETER:
			{
				// check if grid variable overrides default
				data[i].voidVal = grid->findVariable(symbols[i]->token);
				// if not, try shader declaration params, unless it is a varying, in which it will have to reinitialized
				if ((not symbols[i]->isvarying) and (data[i].voidVal == NULL)) {
					std::vector<SlSymbol*>::iterator si = callparams.begin();
					for (; si != callparams.end(); ++si)
						if (!strcmp((*si)->token, symbols[i]->token)) break;
					if (si != callparams.end()) data[i].voidVal = (*si)->default_value;
				}
				// if still nothing, and running initcode, try to add callparam using default value
				// (this allows shaderinstances to modify and preserve param values in the initcode seg.),
				// otherwise reference the default value directly
				if (data[i].voidVal == NULL) {
					if (initcode) { // initcode, add callparam
						// (TODO possibly optimize to only initialize the params used in initcode,
						//       currently it is possible that it needlessly allocates (and frees again) much more memory than really needed,
						//       not that urgent though, since it only executes once)
						bool varok = setParameter(symbols[i]->token, symbols[i]->type, symbols[i]->isvarying,
						                          symbols[i]->arraysize, symbols[i]->default_value);
						if (varok)
							data[i].voidVal = callparams.back()->default_value;
						else {
							// should never happen since this parameter *is* a shader parameter
							printf("[WARNING]: bind() -> Could not assign '%s' from shader callparams!\n", symbols[i]->token);
							data[i].voidVal = symbols[i]->default_value;
						}
					}
					else {	// not running initcode, assign default
						// however, parameter can be varying, in which case it will have to be re-initialized here,
						// since the grid size is only now known.
						if (symbols[i]->isvarying) {
							int ofs = 1; // parameter offset, 1 for float (def.), 3 for color/point/vec/norm, 16 for mtx
							if ((symbols[i]->type == SL_POINT) || (symbols[i]->type == SL_COLOR) ||
									(symbols[i]->type == SL_VECTOR) || (symbols[i]->type == SL_NORMAL))
								ofs = 3;
							else if (symbols[i]->type == SL_MATRIX)
								ofs = 16;
							else
								assert(symbols[i]->type == SL_FLOAT);
							const int uniform_numf = symbols[i]->arraysize * ofs;
							const int vary_numf = uniform_numf * grid->get_nverts();
							float* nfa = new float[vary_numf];
							float* ofa = reinterpret_cast<float*>(symbols[i]->default_value);
							for (int j=0; j<vary_numf; ++j)
								nfa[j] = ofa[j % uniform_numf];
							// can now delete old float array and assign new one
							delete[] ofa;
							data[i].voidVal = symbols[i]->default_value = (void*)nfa;
						}
						else	// uniform, can assign default directly
							data[i].voidVal = symbols[i]->default_value;
					}
				}
				break;
			}
			case SL_TEMPORARY:
			{
				const int arlen = symbols[i]->arraysize;
				const int numval = arlen*(symbols[i]->isvarying ? grid->get_nverts() : 1);	// if uniform var, single value, no grid
				switch (symbols[i]->type)
				{
					case SL_FLOAT:
						data[i].floatVal = new RtFloat[numval];
						memset(data[i].floatVal, 0, sizeof(RtFloat)*numval);
						break;
					// extra boolean type, only internal shader vartype, not visible to RSL or rib
					case SL_BOOL:
						assert(arlen == 1);	// never array
						data[i].voidVal = new boolarray_t(numval);
						break;
					case SL_POINT:
					case SL_VECTOR:
					case SL_NORMAL:
					case SL_COLOR:
						data[i].vectorVal = new RtVector[numval];
						memset(data[i].vectorVal, 0, sizeof(RtVector)*numval);
						break;
					case SL_MATRIX:
						data[i].matrixVal = new RtMatrix[numval];
						memset(data[i].matrixVal, 0, sizeof(RtMatrix)*numval);
						break;
					case SL_STRING: {
						// default empty string
						data[i].stringVal = new RtString[1];
						data[i].stringVal[0] = new char[1];
						data[i].stringVal[0][0] = 0;
						break;
					}
					default:
						// should never happen
						printf("[ERROR]: bind() -> Unexpected 'temp' variable type!\n");
						exit(1);
				}
				break;
			}
			case SL_GLOBAL:
				// get value from grid
				data[i].voidVal = grid->findVariable(symbols[i]->token);
				break;
		}

		// value must be assigned at this point, unless running initcode (some variables defined later when main code is run)
		if (initcode) {
			if ((data[i].voidVal == NULL) && (stp != SL_GLOBAL))
			{
				printf("[ERROR]: bind() -> failed for variable '%s' (initcode)\n", symbols[i]->token);
				exit(1);
			}
		}
		else if (data[i].voidVal == NULL) {
			if (symbols[i]->token)
				printf("[ERROR]: bind() -> '%s' not found\n", symbols[i]->token);
			else
				printf("[ERROR]: bind() -> could not bind unnamed variable\n");
			exit(1);
		}
	}

	return data;
}

// free data which was alloc'ed in bind() (only 'temp' case)
RtVoid SlShaderInstance::unbind(SlValue* data, bool initcode)
{
	std::vector<SlSymbol*>& symbols = shader->symbols;
	const unsigned int symsz = (unsigned int)symbols.size();
	for (unsigned int i=0; i<symsz; ++i) {
		if (symbols[i]->storage == SL_TEMPORARY) {
			// with exception of SL_BOOL type, all are array ptr in union, so just delete voidVal
			if (symbols[i]->type == SL_BOOL)
				delete reinterpret_cast<boolarray_t*>(data[i].voidVal);
			else {
				delete[] reinterpret_cast<unsigned long*>(data[i].voidVal);
				data[i].voidVal = NULL;
			}
		}
	}
}

// run initialization code, if any
RtVoid SlShaderInstance::run_initcode()
{
	if (shader->maincode_offset == 0) return;
	SlRunContext ct;
	// reset program counter
	ct.PC = 0;
	// 'fake' empty grid
	MicroPolygonGrid grid;
	// bind variables
	ct.data = bind(&grid, true);
	// needed for lightshaders, set the parent microgrid
	// also needs to be set here since SIMD_init() below also needs it
	ct.grid = &grid;
	// initialize SIMD array
	ct.SIMD_init();
	// set the shader
	ct.shdInstance = this;
	// execute code
	while (ct.PC < shader->maincode_offset)
		opcodeTable[ct.getOperand(0)->opcode.tableOffset].shadeop(ct);
	// free alloc'd data
	unbind(ct.data, true);
	// free data array itself
	delete[] ct.data;
	ct.data = NULL;
}

// run the actual shader code
RtVoid SlShaderInstance::run(MicroPolygonGrid* grid)
{
	SlRunContext ct;
	// reset program counter
	ct.PC = shader->maincode_offset;
	// bind variables
	ct.data = bind(grid);
	// needed for lightshaders, set the parent microgrid
	// also needs to be set here since SIMD_init() below also needs it
	ct.grid = grid;
	// initialize SIMD array
	ct.SIMD_init();
	// set the shader
	ct.shdInstance = this;
	// execute code
	const RtInt codesize = (RtInt)shader->code.size();
	while (ct.PC < codesize)
		opcodeTable[ct.getOperand(0)->opcode.tableOffset].shadeop(ct);
	// free alloc'd data
	unbind(ct.data);
	// free data array itself
	delete[] ct.data;
	ct.data = NULL;
}

bool SlShaderInstance::setParameter(const char* name, SlType type, RtBoolean isvarying, RtInt arraysize, RtVoid *val)
{
	// first test if the parameter is actually in the shader
	std::vector<SlSymbol*>& symbols = shader->symbols;
	unsigned int i;
	for (i=0; i<symbols.size(); i++) {
		if (symbols[i]->storage == SL_PARAMETER) {
			if (!strcmp(symbols[i]->token, name)) break;
		}
	}
	if (i == symbols.size()) {
		printf("'%s' is not a known parameter of shader '%s'\n", name, shader->name);
		return RI_FALSE;
	}
	// name is ok, type too?
	if (symbols[i]->type != type) {
		printf("Declared parameter '%s' not of the same type as the shader parameter\n", name);
		printf("param type: %d shader type: %d\n", type, symbols[i]->type);
		return RI_FALSE;
	}
	// and detail?
	if (symbols[i]->isvarying != isvarying) {
		printf("Declared parameter '%s' varying/uniform type differs from shader parameter\n", name);
		return RI_FALSE;
	}
	// and finally, arraysize?
	if (symbols[i]->arraysize != arraysize) {
		printf("Declared parameter '%s' incorrect arraysize %d, shader parameter has arraysize %d\n",
				name, arraysize, symbols[i]->arraysize);
		return RI_FALSE;
	}
	// all ok, add to callparameters
	// first test if not already assigned, if so, reset the value
	bool reset = false;
	std::vector<SlSymbol*>::iterator si = callparams.begin();
	for ( ; si!=callparams.end(); ++si) {
		if (!strcmp((*si)->token, name)) {
			reset = true;
			break;
		}
	}
	SlSymbol* p;
	if (reset) {
		// re-assign (possibly new) value
		p = *si;
		// delete old value
		// don't forget strings, allocated as array!
		if (type == SL_STRING)
			delete[] reinterpret_cast<RtString*>(p->default_value)[0];
		delete[] reinterpret_cast<unsigned long*>(p->default_value);
	}
	else {
		// new symbol
		p = new SlSymbol();
		p->token = newCopy_CString(name);
		p->type = type;
		p->storage = SL_PARAMETER;
		// note that the parameter in the case that it is varying is still initialized as uniform,
		// only when the grid is available can it be expanded to a varying, see bind()
		p->isvarying = isvarying;
		p->arraysize = arraysize;
	}
	switch (type) {
		// bool type not handled here, not visible to shading language or rib
		case SL_FLOAT: {
			RtFloat* fp = new RtFloat[arraysize];
			memcpy(fp, val, sizeof(RtFloat)*arraysize);
			p->default_value = fp;
			break;
		}
		case SL_POINT:
		case SL_COLOR:
		case SL_VECTOR:
		case SL_NORMAL: {
			RtVector* vp = new RtVector[arraysize];
			memcpy(vp, val, sizeof(RtVector)*arraysize);
			p->default_value = vp;
			break;
		}
		case SL_MATRIX: {
			RtMatrix* mp = new RtMatrix[arraysize];
			memcpy(mp, val, sizeof(RtMatrix)*arraysize);
			p->default_value = mp;
			break;
		}
		case SL_STRING: {
			RtString* sp = new RtString[arraysize];
			sp[0] = newCopy_CString(((RtString*)val)[0]);
			p->default_value = sp;
			break;
		}
		default:
			// unknown type ? should never happen
			delete p;
			return RI_FALSE;
	}
	if (!reset) callparams.push_back(p);
	return RI_TRUE;
}

//------------------------------------------------------------------------------
// SlRunContext
// SIMD state flags handling using activity counter
// Not entirely sure if properly implemented, but it seems to work, including nested if else
// (see paper "Activity counter: new optimization for the dynamic scheduling of SIMD control flow" by Keryell/Paris)

RtVoid SlRunContext::SIMD_init()
{
	SIMD_count = grid->get_nverts();
	SIMD_flags = new RtInt[SIMD_count];
	// all processors on
	memset(SIMD_flags, 0, sizeof(RtInt)*SIMD_count);
}

RtVoid SlRunContext::SIMD_reset()
{
	// reset to all active
	memset(SIMD_flags, 0, sizeof(RtInt)*SIMD_count);
}

RtVoid SlRunContext::SIMD_push(boolarray_t& cond)
{
	// boolean array may in fact be uniform, so a single value
	if (cond.size() == 1) {
		bool bval = int(not cond[0]);
		for (int i=0; i<SIMD_count; ++i)
			if (SIMD_flags[i])
				SIMD_flags[i]++;
			else
				SIMD_flags[i] = bval;
		return;
	}
	for (int i=0; i<SIMD_count; ++i)
		if (SIMD_flags[i])
			SIMD_flags[i]++;
		else
			SIMD_flags[i] = int(not cond[i]);
}

RtVoid SlRunContext::SIMD_pop()
{
	for (int i=0; i<SIMD_count; ++i)
		if (SIMD_flags[i])
			SIMD_flags[i]--;
}

RtVoid SlRunContext::SIMD_else()
{
	for (int i=0; i<SIMD_count; ++i)
		if (SIMD_flags[i] <= 1)
			SIMD_flags[i] = int(not SIMD_flags[i]);
}

__END_QDRENDER
