#ifndef _MAKETEXTURE_H
#define _MAKETEXTURE_H

#include "ri.h"

#include "QDRender.h"
__BEGIN_QDRENDER

void makeTexture(const char* pic, const char* tex, RtToken swrap, RtToken twrap,
                 RtToken filtername, RtFloat swidth, RtFloat twidth,
                 RtInt n, RtToken tokens[], RtPointer parms[],
                 bool savehalf = true, bool forcegray = false);

__END_QDRENDER

#endif // _MAKETEXTURE_H
