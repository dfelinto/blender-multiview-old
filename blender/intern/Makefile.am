SUBDIRS = string ghost guardedalloc bmfont moto container memutil \
	decimation keymaker bsp  iksolver
## python/freeze

lib_LTLIBRARIES = libblender_intern.la

libblender_intern_la_LIBADD = \
	string/libblender_STR.la \
	ghost/libblender_GHOST.la \
	ghost/libblender_GHOST_X11.la \
	guardedalloc/libblender_guardedalloc.la \
	bmfont/libblender_BMF.la \
	moto/libblender_MT.la \
	container/libblender_CTR.la \
	memutil/libblender_MEM.la \
	decimation/libblender_LOD.la \
	keymaker/libblender_belgium.la \
	bsp/libblender_BSP.la \
	iksolver/libblender_IK.la \
	python/frozen/libfrozen.a

LIBOBJS = \
	string/libblender_STR.a \
	ghost/libblender_GHOST.a \
	ghost/libblender_GHOST_X11.a \
	guardedalloc/libblender_guardedalloc.a \
	bmfont/libblender_BMF.a \
	moto/libblender_MT.a \
	container/libblender_CTR.a \
	memutil/libblender_MEM.a \
	decimation/libblender_LOD.a \
	bsp/libblender_BSP.a \
	keymaker/libblender_belgium.a \
	iksolver/libblender_IK.a \
	python/frozen/libfrozen.a
