//---------------------------------------------------------------------------------------
// Handles image based textures, including environment & shadow maps
//---------------------------------------------------------------------------------------

#ifndef _IMAGETEXTURES_H
#define _IMAGETEXTURES_H

#include "ri.h"

#include "QDRender.h"
__BEGIN_QDRENDER

void getTextureColor(RtColor col, const char* texname, float st[8], float A);
void getTextureFloat(RtFloat& fval, const char* texname, float st[8], float A);
float getShadowFactor(const char* texname, float st[8], const RtPoint& wldP, float* samples, float* blur, float* bias);

__END_QDRENDER

#endif // _IMAGETEXTURES_H
