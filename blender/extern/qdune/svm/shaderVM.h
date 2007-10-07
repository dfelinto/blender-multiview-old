#ifndef _SHADERVM_H
#define _SHADERVM_H

#include "slshader.h"

#include "QDRender.h"
__BEGIN_QDRENDER

typedef RtVoid (*OPCODEFUNC)(SlRunContext&);
struct _opcode
{
	const char* name;
	const char* types;
	OPCODEFUNC shadeop;
};

extern _opcode opcodeTable[];
__END_QDRENDER

#endif // _SHADERVM_H
