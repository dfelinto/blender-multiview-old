if BlenderGAME
gameblenderdir = gameengine
gamelib =  gameengine/libblender_gameengine.la
playerlib = libblender_player.la
else
gameblenderdir =
gamelib =
playerlib =
endif

if BlenderPLUGIN
pluginlib= libblender_plugin.la
else
pluginlib=
endif

if WIN32
icondir = icons
else 
icondir =
endif

if EXPPYTHON
blenpythonlib = blender/python/libblender_python.la
else
blenpythonlib = blender/bpython/libblender_bpython.la
endif

SUBDIRS = blender kernel creator $(gameblenderdir) $(icondir)

noinst_LTLIBRARIES = libblender_source.la $(playerlib) $(pluginlib)

libblender_source_la_LDFLAGS = \
	-L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
	 $(X_LIBS)

libblender_source_la_SOURCES = 

libblender_source_la_LIBADD = \
	-lpython@PYTHON_VERSION@ \
	$(gamelib) \
	creator/libblender_creator.la \
	blender/src/libblender_blendersrc.la \
	blender/renderconverter/libblender_renderconverter.la \
	blender/blenloader/libblender_blenloader.la \
	blender/deflate/libblender_deflate.la \
	blender/inflate/libblender_inflate.la \
	blender/encrypt/libblender_encrypt.la \
	blender/decrypt/libblender_decrypt.la \
	blender/sign/libblender_sign.la \
	blender/verify/libblender_verify.la \
	blender/writestreamglue/libblender_writestreamglue.la \
	blender/readstreamglue/libblender_readstreamglue.la \
	blender/writeblenfile/libblender_writeblenfile.la \
	blender/readblenfile/libblender_readblenfile.la \
	blender/avi/libblender_avi.la \
	blender/img/libblender_img.la \
	blender/render/libblender_render.la \
	blender/radiosity/libblender_radiosity.la \
	blender/blenkernel/libblender_blenkernel.la \
	blender/blenpluginapi/libblender_blenpluginapi.la \
	blender/imbuf/libblender_imbuf.la \
	blender/blenlib/libblender_blenlib.la \
	$(blenpythonlib) \
	blender/makesdna/libblender_makesdna.la \
	kernel/libblender_kernel.la

libblender_player_la_LDFLAGS = \
	-L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
	 $(X_LIBS)

libblender_player_la_SOURCES =

libblender_player_la_LIBADD = \
	gameengine/libgameblender_player.la

libblender_plugin_la_SOURCES =

libblender_plugin_la_LIBADD = \
	gameengine/libgameblender_plugin.la
