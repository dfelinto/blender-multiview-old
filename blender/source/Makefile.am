if BlenderGAME
gameblenderdir = gameengine
gamelib =  gameengine/libblender_gameengine.la
else
gameblenderdir =
gamelib =
endif

if WIN32
icondir = icons
else 
icondir =
endif

SUBDIRS = blender kernel creator $(gameblenderdir) $(icondir)
## Took out ode since not sure what to do with it yet.

## Assuming no reason to build creator anymore lets just call publisher blender
if CARBON
## bin_PROGRAMS = blender.app blenderplayer.app plugin.app
bin_PROGRAMS = blender.app blenderplayer.app

blender_app_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config

blender_app_SOURCES = 

blenderplayer_app_SOURCES =

blender_app_LDADD = \
	-lpython@PYTHON_VERSION@ \
	$(gamelib) \
	creator/libblender_creator.la \
	blender/libblender_blender.la \
	kernel/libblender_kernel.la \
	../intern/libblender_intern.la \
	../intern/python/frozen/libfrozen.a

blenderplayer_app_LDADD =

else
## bin_PROGRAMS = blender blenderplayer plugin
bin_PROGRAMS = blender.bin blenderplayer.bin

blender_bin_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config

blender_bin_SOURCES = 

blenderplayer_bin_SOURCES =

blender_bin_LDADD = \
	-lpython@PYTHON_VERSION@ \
	$(gamelib) \
	creator/libblender_creator.la \
	blender/libblender_blender.la \
	kernel/libblender_kernel.la \
	../intern/libblender_intern.la \
	../intern/python/frozen/libfrozen.a

blenderplayer_bin_LDADD =

endif
