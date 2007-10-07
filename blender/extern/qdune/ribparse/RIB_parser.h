#ifndef RIBPARSER_H
#define RIBPARSER_H 1

#include <string>
#include <cstdio>
#include "ri.h"

#include "ribInterface.h"

#include "QDRender.h"
__BEGIN_QDRENDER

// for debug text output
typedef void (*debug_print_out)(const std::string&);

// 4k buffer for parameters, big enough?
// (-1 for catching read overflow, see code)
const unsigned int TKBUFSIZE = 4095;

class RIBparse_t
{
	public:
		// ctor
		RIBparse_t(debug_print_out dbo=NULL)
		{
			dbg_out = dbo;
			fp = NULL;
			curline = keyline = 0;
			_FPTR = NUMREAD = 0;
			lastblock = false;
		}
		// dtor
		~RIBparse_t() {}
		// mtds
		RtInt parse(const char* filename, bool isbasefile=true);
		unsigned int getLastLine() const { return curline; }
	private:
		// data
		// buffer for tokens read from the file
		char tokenBuffer[TKBUFSIZE + 1];
		// data used for file reading per block
		char linebuf[16384];
		unsigned _FPTR, NUMREAD;
		bool lastblock;
		// optional function to be called to write debug output
		debug_print_out dbg_out;
		// current file pointer
		FILE* fp;
		// current line in file being read, and last line read where a keyword started
		unsigned int curline, keyline;
		// the list of parsed parameters, starts with a ri command
		parameterList_t params;
		// flag to indicate parsing errors,
		// if any occured, current line will be skipped
		bool parse_errors;

		// mtds
		char getNextChar();
		char readString();
		char readNumber(char lastChar);
		char readFloat(char lastChar);
		char readToken(char lastChar);
		char skipWhitespace();
		char readUntilNewline();
};

__END_QDRENDER

#endif // RIBPARSER_H
