///###################################################################################
/// Simple RIB parser
///
/// Very first simple parser experiment. Doesn't know anything about rib syntax.
/// It just tries to deduce from what is read how it all should be interpreted.
/// So if a bracket character is encountered, it assumes an array,
/// if a quote character is read, it's a string, etc..
/// If a token starts with an uppercase char, it is assumed to be a keyword,
/// from there a parameter list is built until the next keyword is encountered,
/// at which point the parameter list is passed to the ribInterface and a
/// new parameter list is prepared.
/// And so on until EOF...
///
/// The complete file is read one char at a time, because of this,
/// number arrays are a bit problematic, rib supports both integer and float arrays,
/// but here both are interpreted as float arrays for now, and the parameter process
/// code will handle conversion to int if needed. This will cause trouble if the
/// integers are outside the range +/- 2^24, for IEEE format at least.
/// There is support in the code for integer arrays, but currently not used.
/// Another option would be to store the arrays as strings and interpret the values
/// as needed by the parse code, but this probably would use more memory.
/// Test some time.
/// Something similar happens for single float and integer values.
/// If the string does not contain a decimal point, the string is interpreted as
/// an integer number.
/// (Of course, an actual (simple RD) parser that knows what to expect would help too)
///
///###################################################################################

#include "RIB_parser.h"

#include <iostream>
//#include <cassert>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

using namespace std;

__BEGIN_QDRENDER

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

// float/int to string conversion, only used for debug output
// could do it with stringstreams, but it's a lot slower than snprintf()

// float to string
inline string FTOA(float fv)
{
	char str[32];
	snprintf(str, 32, "%g", fv);
	return string(str);
}

// int to string
inline string ITOA(int iv)
{
	char str[32];
	snprintf(str, 32, "%d", iv);
	return string(str);
}

// test if character might be part of float or integer string
inline bool isNumber(char s)
{
	return (isdigit(s) || (s=='-') || (s=='.'));
}

// test if string contains a float number
inline bool isStringFloat(char* s)
{
	// doesn't need full test, since conversion error can be detected
	return (isNumber(s[0]) && (strchr(s, '.')!=NULL));
}

// test if string contains an integer
// (it can of course be interpreted as float/double without decimal point as well)
inline bool isStringInteger(char* s)
{
	//return (isNumber(s[0]) && (strchr(s, '.')==NULL));
	// more complete test
	if (!(isNumber(s[0]) && (strchr(s, '.')==NULL))) return false;
	if (s[0]=='-') s++;
	size_t sl = strlen(s);
	while (sl--) {
		if (!isdigit(s[sl])) return false;
	}
	return true;
}

//------------------------------------------------------------------------------
// RIBparse_t

// read one char from file
char RIBparse_t::getNextChar()
{
	// read file in blocks of 16384 bytes, not really much faster than reading single chars though...
	// (and disk io probably already works like that anyway...)
	if ((_FPTR == 0) || (_FPTR == 16384)) {
		NUMREAD = (unsigned int)fread(linebuf, 1, 16384, fp);
		if (NUMREAD < 16384) lastblock = true;
		_FPTR = 0;
	}
	if (lastblock && (_FPTR == NUMREAD)) return EOF;
	char c = linebuf[_FPTR++];

	if (c == '\n') curline++;
	return c;
}

// read chars until non-whitespace found
char RIBparse_t::skipWhitespace()
{
	char c = getNextChar();
	while ((c!=EOF) && isspace(c)) c = getNextChar();
	return c;
}

// read a quoted string from the file into the tokenBuffer
char RIBparse_t::readString()
{
	unsigned int p = 0;
	char c = getNextChar();
	// start quote char was already read
	if (c == '"') return c; // empty string?
	tokenBuffer[p++] = c;
	while ((c != EOF) && (p < TKBUFSIZE)) {
		c = getNextChar();
		if ((c == '"') || (c == '\n')) break;
		tokenBuffer[p++] = c;
	}
	if (p >= TKBUFSIZE) throw RiException_t("tokenBuffer Overflow!");
	if (c == '\n') throw RiException_t("String not terminated!");
	tokenBuffer[p] = '\0';
	return c;
}

// read a number into the tokenBuffer from the file
char RIBparse_t::readNumber(char lastChar)
{
	unsigned int p = 0;
	char c = lastChar;
	while ((c != EOF) && (p < TKBUFSIZE)) {
		if (!isNumber(c)) break;
		tokenBuffer[p++] = c;
		c = getNextChar();
	}
	if (p >= TKBUFSIZE) throw RiException_t("tokenBuffer Overflow!");
	tokenBuffer[p] = '\0';
	return c;
}


// as above, but specifically for fpnums
char RIBparse_t::readFloat(char lastChar)
{
	unsigned int p = 0;
	char c = lastChar;
	while ((c != EOF) && (p < TKBUFSIZE)) {
		// here all possible float chars are just individually tested in ascii order.
		if (c < '0') {
			if ((c != '+') && (c != '-') && (c != '.')) break;
		}
		else if (c > '9') {
			if ((c != 'E') && (c != 'e')) break;
		}
		tokenBuffer[p++] = c;
		c = getNextChar();
	}
	if (p>=TKBUFSIZE) throw RiException_t("tokenBuffer Overflow!");
	tokenBuffer[p] = '\0';
	return c;
}


// read a token into the tokenBuffer from the file
char RIBparse_t::readToken(char lastChar)
{
	unsigned int p = 0;
	char c = lastChar;
	while ((c != EOF) && (p < TKBUFSIZE)) {
		if (isspace(c)) break;
		tokenBuffer[p++] = c;
		c = getNextChar();
	}
	if (p >= TKBUFSIZE) throw RiException_t("tokenBuffer Overflow!");
	tokenBuffer[p] = '\0';
	return c;
}

// read chars until a newline found
char RIBparse_t::readUntilNewline()
{
	char c = getNextChar();
	while ((c != EOF) && (c != '\n')) c = getNextChar();
	return c;
}

//------------------------------------------------------------------------------
// parse the given file
RtInt RIBparse_t::parse(const char* filename, bool isbasefile)
{
	// open file in binary mode, want raw input, even for ascii, might add binary file support later
	fp = fopen(filename, "rb");

	// no file?
	if (fp == NULL) {
		cout << "Cannot open file " << filename << endl;
		return RIE_NOFILE;
	}

	parse_errors = false;

	// if basefile, signal start state and set basepath of current file
	if (isbasefile) {
		string bpath = filename;
		size_t ld = bpath.rfind('\\');
		if (ld == string::npos) ld = bpath.rfind('/');
		if (ld != string::npos) bpath = bpath.substr(0, ld+1);
		params.process(-1, bpath.c_str());
	}

	// current char read from file
	char cc;

	// parse the file one char at a time
	for (;;) {
		cc = getNextChar();

		if (cc == EOF) {
			// final process of last line
			string procerr = params.process(0);
			if (dbg_out && (!procerr.empty())) dbg_out(procerr);
			if (dbg_out) dbg_out("FILE END");
			break;
		}

		// TODO handle binary code, for now skip and ignore
		if (!isascii(cc)) continue;

		// keep reading until next token, skipping whitespace
		if (isspace(cc)) continue;

		// is comment?
		if (cc == '#') {
			// structural hint can be skipped, assume all '#' == comments
			readUntilNewline();
			continue;
		}

		// string?
		if (cc == '"') {
			readString();
			if (dbg_out) dbg_out(string("STRING: \"") + string(tokenBuffer) + "\"");
			params.add(parameter_t(tokenBuffer));
			continue;
		}

		// array?
		if (cc == '[') {
			// yes, string or int/float array?
			cc = skipWhitespace();
			if (isNumber(cc)) {
				// int/float/double array, all handled as float, see comments in header
				if (dbg_out) dbg_out("FLOAT ARRAY");
				vector<float> fp_array;
				for (;;) {
					cc = readFloat(cc); //readNumber(cc);
					char* cve;
					const float fpn = (float)strtod(tokenBuffer, &cve); // better than atof, error detection possible
					bool convert_error = ((unsigned int)(cve - tokenBuffer) < strlen(tokenBuffer));
					if (convert_error) parse_errors = true;
					if (dbg_out) {
						if (convert_error) dbg_out(string("LINE ") + ITOA(curline) + string(": float array conversion error: ") + string(tokenBuffer) + string(" at char ") + ITOA(int(cve - tokenBuffer)));
					}
					// only add parameter if no conversion errors
					if ((!convert_error) && (strlen(tokenBuffer)>0)) fp_array.push_back(fpn);
					// if end bracket, array ends
					if (cc == ']') break;
					// next number
					cc = skipWhitespace();
				}
				// error if no end bracket
				if (cc != ']') {
					parse_errors = true;
					if (dbg_out) dbg_out(string("LINE ") + ITOA(curline) + string(": ARRAY MISSING ']'"));
				}
				// if not empty, add array to list
				if (!fp_array.empty()) params.add(parameter_t(fp_array));
			}
			else if (cc == '"') {
				// string array
				if (dbg_out) dbg_out("STRING ARRAY");
				vector<string> st_array;
				while (cc != ']') {
					readString();
					st_array.push_back(tokenBuffer);
					cc = skipWhitespace();
				}
				if (!st_array.empty()) params.add(parameter_t(st_array));
				// error if no end bracket found
				if (cc != ']') {
					parse_errors = true;
					if (dbg_out) dbg_out(string("LINE ") + ITOA(curline) + string(": ARRAY MISSING ']'"));
				}
			}
			else if (cc != ']') { // <- possibly empty array is ok
				// unknown array contents? could be syntax error, missing quote maybe, emit warning
				parse_errors = true;
				if (dbg_out) dbg_out(string("LINE ") + ITOA(curline) + string(": warning, unknown array contents"));
				// continue until end of array
				while (cc != ']') {
					cc = getNextChar();
					if (cc==EOF) {
						// end of file?
						if (dbg_out) dbg_out("Unexpected end of file!");
						fclose(fp);
						return RIE_BADFILE;
					}
				}
			}
			continue;
		}

		// something other, keyword (starts with uppercase char), number, or unknown
		// read into tokenBuffer
		readToken(cc); // return value not needed anymore at this point
		// float or int number?
		if (isStringFloat(tokenBuffer)) {
			// float
			char* cve;
			const float fpn = (float)strtod(tokenBuffer, &cve);
			bool convert_error = ((unsigned int)(cve - tokenBuffer) < strlen(tokenBuffer));
			if (convert_error) parse_errors = true;
			// only add parameter if no conversion errors
			if (!convert_error) params.add(parameter_t(fpn));
			if (dbg_out) {
				if (convert_error)
					dbg_out(string("LINE ") + ITOA(curline) + string(": float conversion error: ") + string(tokenBuffer) + string(" at char ") + ITOA(int(cve - tokenBuffer)));
				else
					dbg_out(string("FPNUM: ") + FTOA(fpn));
			}
		}
		else if (isStringInteger(tokenBuffer)) {
			// integer
			// again, can be interpreted as float as well, see integer array comments above
			int i = atoi(tokenBuffer);
			params.add(parameter_t(i));
			if (dbg_out) dbg_out("INTNUM: " + ITOA(i));
		}
		else {
			// a string, keyword or unknown?
			if (isupper(tokenBuffer[0])) {
				// KEYWORD. main entry point to Ri...() calls here
				// if there were no parse errors, process the currently built parameterlist
				if (!parse_errors) {
					string procerr = params.process(0);
					if (!procerr.empty()) {
						if (dbg_out) {
							dbg_out(string("LINE: ") + ITOA(keyline+1) + string(" process() reports:"));
							dbg_out(procerr);
						}
					}
				}
				// now start a new list
				parse_errors = false;
				params.clear();
				params.name(string(tokenBuffer));
				keyline = curline;
				if (dbg_out) { dbg_out("");  dbg_out(string("-------------> KW: ") + string(tokenBuffer)); }
			}
			else {
				// possibly unknown token
				// there are a few (one?) reserved tokens which don't start with a capital however
				// like 'version', if so, ignore
				if (!strcmp(tokenBuffer, "version")) {
					if (dbg_out) dbg_out(string("reserved unused keyword: ") + string(tokenBuffer));
				}
				else {
					// really unknown
					parse_errors = true;
					// might be syntax error, try to create some meaningful message if possible
					if (strchr(tokenBuffer, '"')!=NULL) {
						// single quote found, an incomplete string?
						printf("LINE %d: incomplete string -> '%s'\n", curline, tokenBuffer);
					}
					else
						printf("LINE %d: unknown token -> '%s'\n", curline, tokenBuffer);
				}
			}
		}

	}

	fclose(fp);

	// if this is basefile, signal final end
	if (isbasefile) params.process(1);

	return RIE_NOERROR;

}

__END_QDRENDER

