//-------------------------------------------------------
// JPEG image support
//-------------------------------------------------------

#ifndef _JPEG_IO_H
#define _JPEG_IO_H

#include "img_io.h"

#include "QDRender.h"
__BEGIN_QDRENDER

imgbuf_t* loadJPG(const char filename[]);

__END_QDRENDER

#endif	// _JPEG_IO_H
