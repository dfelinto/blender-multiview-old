if BlenderGAME
gameblenderdir = gameengine
gamelib =  gameengine/libblender_gameengine.la
playerlib = libblender_player.la
else
playerlib =
gameblenderdir =
gamelib =
endif

if WIN32
icondir = icons
else 
icondir =
endif

SUBDIRS = blender kernel creator $(gameblenderdir) $(icondir)

lib_LTLIBRARIES = libblender_source.la $(playerlib)

libblender_source_la_LDFLAGS = \
	-L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
	 $(X_LIBS)

libblender_source_la_SOURCES = 

libblender_source_la_LIBADD = \
	-lpython@PYTHON_VERSION@ \
	$(gamelib) \
	creator/libblender_creator.la \
	blender/libblender_blender.la \
	kernel/libblender_kernel.la

libblender_player_la_LDFLAGS = \
	-L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
	 $(X_LIBS)

libblender_player_la_SOURCES =

libblender_player_la_LIBADD = \
	gameengine/libgameblender_player.la
