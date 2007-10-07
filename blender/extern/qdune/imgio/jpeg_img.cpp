//-------------------------------------------------------
// JPEG image support (input only)
//-------------------------------------------------------

#include <cstdio>
#include <cstdlib>
#include <csetjmp>
extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}

#include "jpeg_img.h"

__BEGIN_QDRENDER

// string for error messages
static char errString[256];

METHODDEF(void) _jpeg_errmsg(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message)(cinfo, buffer);
	sprintf(errString, buffer);
}

struct my_jpeg_error_mgr {
	struct jpeg_error_mgr pub;	// public fields
	jmp_buf setjmp_buffer;	// for return to caller
};

// JPEG error handler
typedef struct my_jpeg_error_mgr *error_ptr;

void my_jpeg_error_exit(j_common_ptr cinfo)
{
	char buffer[JMSG_LENGTH_MAX];
	error_ptr myerr = (error_ptr)cinfo->err;
	(*cinfo->err->format_message)(cinfo, buffer);
	sprintf(errString, buffer);
	longjmp(myerr->setjmp_buffer, 1);
}

imgbuf_t* loadJPG(const char filename[])
{
	struct jpeg_decompress_struct cinfo;
	struct my_jpeg_error_mgr jerr;
	FILE * infile;		// source file
	JSAMPARRAY buffer;		// Output row buffer
	int row_stride;		// physical row width in output buffer
	int x, yx, errNo;

	if ((infile = fopen(filename, "rb")) == NULL) {
		sprintf(errString, "Could not load %s", filename);
		return NULL;
	}

	// We set up the normal JPEG error routines, then override error_exit.
	cinfo.err = jpeg_std_error(&jerr.pub);
	cinfo.err->output_message = _jpeg_errmsg;     // catch error messages
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_jpeg_error_exit;

	// Establish the setjmp return context for my_error_exit to use.
	errNo = setjmp(jerr.setjmp_buffer);
	if (errNo) {
		// If we get here, the JPEG code has signaled an error.
		// We need to clean up the JPEG object, close the input file, and return.
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		//INVALID_TYPE = (cinfo.err->msg_code==JERR_NO_SOI);
		if (errNo==255) sprintf(errString, "Unsupported colorspace");
		return NULL;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);

	//if (!img.data) longjmp(jerr.setjmp_buffer, 1);
	jpeg_start_decompress(&cinfo);

	// only accept greyscale or rgb colorspace
	bool grey = (cinfo.out_color_space == JCS_GRAYSCALE);
	if ((cinfo.out_color_space!=JCS_RGB) && (!grey))  longjmp(jerr.setjmp_buffer, 255); //unsup. colorspace

	imgbuf_t* img = new imgbuf_t(cinfo.image_width, cinfo.image_height, cinfo.num_components, IMG_BYTE);

	row_stride = cinfo.image_width * cinfo.num_components;
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
	yx = 0;
	while (cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		unsigned char* imgdata = reinterpret_cast<unsigned char*>(img->getData());
		if (grey) {
			for (x=0; x<int(cinfo.image_width); x++)
				imgdata[yx + x] = (*buffer)[x];
		}
		else {
			for (x=0; x<int(cinfo.image_width*3); x+=3) {
				imgdata[yx + x    ] = (*buffer)[x    ];
				imgdata[yx + x + 1] = (*buffer)[x + 1];
				imgdata[yx + x + 2] = (*buffer)[x + 2];
			}
		}
		yx += img->Width() * cinfo.num_components;
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return img;
}

__END_QDRENDER
