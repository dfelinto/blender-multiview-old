if BlenderGAME
gameblenderdir = gameengine
gamelib =  gameengine/libblender_gameengine.la
else
gameblenderdir =
gamelib =
endif

if WIN
icondir = icons
else 
icondir =
endif

SUBDIRS = blender kernel creator $(gameblenderdir) $(icondir)
## Took out ode since not sure what to do with it yet.

## Assuming no reason to build creator anymore lets just call publisher blender
if CARBON
bin_PROGRAMS = blenderpublisher.app blenderplayer.app 
else
bin_PROGRAMS = blender blenderplayer plugin
endif

blender_LDADD = \
	creator/libblender_creator.la \
	$(gamelib) \
	blender/libblender_blender.la \
	kernel/libblender_kernel.a \
	../intern/libblender_intern.la
## Need to add python stuff here
