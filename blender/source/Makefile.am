SUBDIRS = blender gameengine

bin_PROGRAMS = blender

blender_SOURCES = \
	creator/buildinfo.c \
	creator/creator.c

#I've assumed that everything under blender directory should be statically linked into the main file... while the gameengine and ode are separate dynamically linked to the binary

blender_LDADD = \
	blender/avi/libblender_avi.a \
	blender/blenkernel/libblender_kernel.a \
	blender/blenlib/libblender_blenlib.a \
	blender/blenloader/libblender_blenloader.a \
	blender/blenpluginapi/libblender_blenpluginapi.a \
	blender/bpython/libblender_bpython.a \
	blender/decrypt/libblender_decrypt.a \
	blender/deflate/libblender_deflate.a \
	blender/encrypt/libblender_encrypt.a \
	blender/imbuf/libblender_imbuf.a \
	blender/img/libblender_img.a \
	blender/inflate/libblender_inflate.a \
	blender/makesdna/libblender_makesdna.a \
	blender/misc/libblender_misc.a \
	blender/radiosity/libblender_radiosity.a \
	blender/readblenfile/libblender_readblenfile.a \
	blender/readstreamglue/libblender_readsreamglue.a \
	blender/render/libblender_render.a \
	blender/renderconverter/libblender_renderconverter.a \
	blender/sign/libblender_sign.a \
	blender/src/libblender_blendersrc.a \
	blender/verify/libblender_verify.a \
	blender/writeblenfile/libblender_writeblenfile.a \
	blender/writestreamglue/libblender_writestreamglue.a







