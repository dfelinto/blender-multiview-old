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

# blender will always be built, check for the plugin option only
if BlenderPLUGIN
bin_PROGRAMS = blender.bin blenderplayer.bin
else
bin_PROGRAMS = blender.bin
endif

SUBDIRS = blender kernel creator $(gameblenderdir) $(icondir)
## Took out ode since not sure what to do with it yet.

## Assuming no reason to build creator anymore lets just call publisher blender
if CARBON

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

else # CARBON

blender_bin_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
                      $(X_LIBS)

blender_bin_SOURCES = 

blender_bin_LDADD = \
	-lpython@PYTHON_VERSION@ \
	$(gamelib) \
	creator/libblender_creator.la \
	blender/libblender_blender.la \
	kernel/libblender_kernel.la \
	../intern/libblender_intern.la \
	../intern/python/frozen/libfrozen.a

if BlenderPLUGIN

blenderplayer_bin_SOURCES =

blenderplayer_bin_LDADD =

endif # BlenderPLUGIN

endif # CARBON
