SUBDIRS = intern source

internlibs = \
	intern/bsp/libblender_BSP.la \
	intern/decimation/libblender_LOD.la \
	intern/ghost/libblender_GHOST.la \
	intern/string/libblender_STR.la \
	intern/guardedalloc/libblender_guardedalloc.la \
	intern/bmfont/libblender_BMF.la \
	intern/container/libblender_CTR.la \
	intern/memutil/libblender_MEM.la \
	intern/keymaker/libblender_belgium.la \
	intern/iksolver/libblender_IK.la \
	intern/moto/libblender_MT.la

if EXPPYTHON
  blenpythonlib =
else
  blenpythonlib = intern/python/frozen/libfrozen.a
endif

if BlenderGAME
MACPLAYER=blenderplayer.app
PLAYER=blenderplayer
else
MACPLAYER=
PLAYER=
endif

if BlenderPLUGIN

lib_LTLIBRARIES = libbrowserplugin.la

libbrowserplugin_la_SOURCES =

libbrowserplugin_la_LDFLAGS =  \
	$(blenpythonlib) \
	-lpython@PYTHON_VERSION@

libbrowserplugin_la_LIBADD = \
	source/libblender_plugin.la \
	$(internlibs)

endif

## Assuming no reason to build creator anymore lets just call publisher blender
if CARBON

bin_PROGRAMS = blender.app $(MACPLAYER)

blender_app_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config -framework OpenGL -framework AGL -framework Carbon

blender_app_SOURCES =

blenderplayer_app_SOURCES =

blender_app_LDADD = \
	source/libblender_source.la \
	$(internlibs) \
	$(blenpythonlib) \
	-lpython@PYTHON_VERSION@

blenderplayer_app_LDADD = \
	source/libblender_player.la \
	$(internlibs) \
	$(blenpythonlib) \
	-lpython@PYTHON_VERSION@

else

bin_PROGRAMS = blender $(PLAYER)

blender_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
         $(X_LIBS)

blender_SOURCES =

blenderplayer_SOURCES =

blender_LDADD = \
        source/libblender_source.la \
        $(internlibs) \
        $(blenpythonlib) \
        -lpython@PYTHON_VERSION@

blenderplayer_LDADD = \
	source/libblender_player.la \
	$(internlibs) \
	$(blenpythonlib) \
	-lpython@PYTHON_VERSION@

endif
