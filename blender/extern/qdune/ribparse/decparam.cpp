#include "decparam.h"
#include <cctype>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "QDRender.h"
__BEGIN_QDRENDER

#if _MSC_VER
#pragma warning(disable : 4996)
#endif

// helper function
// parses a declared parameter string, either inline or from RiDeclare()
// 'inline_name' if not NULL, indicates that 'declaration' is inline,
// and the extracted name will be returned in it.
bool parseDeclaration(const char* declaration, decParam_t *dp, char inline_name[256])
{
	// Currently this func will ignore spacing and correctly interprets things like "uniformfloatKd",
	// may be nice unintentional 'fuzzy' behaviour, but probably should really be flagged as error...

	if ((declaration==NULL) || (*declaration==0)) return false;

	const char* s = declaration;
	// skip whitespace, if any
	while (*s && isspace(*s)) s++;
	if (*s==0) return false;

	// storage class
	if (!strncmp(s, "constant", 8))
		{ dp->ct_flags |= SC_CONSTANT;  s += 8; }
	else if (!strncmp(s, "uniform", 7))
		{ dp->ct_flags |= SC_UNIFORM;  s += 7; }
	else if (!strncmp(s, "varying", 7))
		{ dp->ct_flags |= SC_VARYING;  s += 7; }
	else if (!strncmp(s, "facevarying", 11))
		{ dp->ct_flags |= SC_FACEVARYING;  s += 11; }
	else if (!strncmp(s, "vertex", 6))
		{ dp->ct_flags |= SC_VERTEX;  s += 6; }
	else // uniform default
		dp->ct_flags |= SC_UNIFORM;

	// skip more whitespace
	while (*s && isspace(*s)) s++;
	if (*s==0) return false;

	// data type
	if (!strncmp(s, "float", 5))
		{ dp->ct_flags |= DT_FLOAT;  s += 5; }
	else if (!strncmp(s, "point", 5))
		{ dp->ct_flags |= DT_POINT;  s += 5; }
	else if (!strncmp(s, "vector", 6))
		{ dp->ct_flags |= DT_VECTOR;  s += 6; }
	else if (!strncmp(s, "normal", 6))
		{ dp->ct_flags |= DT_NORMAL;  s += 6; }
	else if (!strncmp(s, "color", 5))
		{ dp->ct_flags |= DT_COLOR;  s += 5; }
	else if (!strncmp(s, "string", 6)) {
		// strings can only be constant or uniform, warn and change to uniform if anything else
		if (dp->ct_flags & (SC_VARYING | SC_VERTEX)) {
			dp->ct_flags = SC_UNIFORM;
			printf("[WARNING]: '%s' <- strings can only be constant or uniform\n", declaration);
		}
		dp->ct_flags |= DT_STRING;  s += 6;
	}
	else if (!strncmp(s, "matrix", 6))
		{ dp->ct_flags |= DT_MATRIX;  s += 6; }
	else if (!strncmp(s, "hpoint", 6))
		{ dp->ct_flags |= DT_HPOINT;  s += 6; }
	else if (!strncmp(s, "integer", 7))
		{ dp->ct_flags |= DT_INT;  s += 7; }
	else // nothing recognized, return error
		return false;

	// skip more whitespace
	while (*s && isspace(*s)) s++;

	// preset arlen to 1 here, also indicates single values
	dp->arlen = 1;

	// if string ends here, and not parsing an inline declaration, return true, parameter is valid,
	// otherwise return false, since a parameter name is still required at this point
	if (*s==0) {
		if (inline_name==NULL) return true;
		return false;
	}

	// array length, if any
	if (*s=='[') {
		s++; // skip start bracket
		const char* s2 = s; // start of number, conversion below
		// syntax check 1, check contents are digits only
		while (*s && isdigit(*s)) s++;
		// syntax check 2, find end bracket, only whitespace allowed until found
		while (*s && (*s!=']')) {
			if (!isspace(*s)) return false;
			s++;
		}
		if (*s!=']') return false;  // no end bracket, return error
		// set array length from string
		// here s2 is complete string starting from first digit, but conversion 'should' work anyway...
		dp->arlen = atoi(s2);
		s++; // skip end bracket
	}

	// if inline, find parameter name
	if (inline_name) {
		// skip ws
		while (*s && isspace(*s)) s++;
		if (*s==0) return false; // no name specified...
		// first character must be alpha or underscore, following chars may also be digits
		if (!(isalpha(*s) || (*s=='_'))) return false;
		const char* s2 = s;
		// find end of name
		while (isalnum(*s) || (*s=='_')) s++;
		const int len = (int)(s - s2);
		// 256 chars max
		if (len > 256) return false; // no more than 256 chars
		if (len < 1) return false; // empty string? (or no alpha chars anyway)
		strncpy(inline_name, s2, len);
	}

	// warn if anything trailing the string other than whitespace
	while (isspace(*s)) s++;
	if (*s!=0) printf("[WARNING]: trailing characters ignored -> '%s'\n", s);

	return true;
}

__END_QDRENDER
