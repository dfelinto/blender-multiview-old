SUBDIRS = intern source

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
	intern/libblender_intern.la

endif

## Assuming no reason to build creator anymore lets just call publisher blender
if CARBON

bin_PROGRAMS = blender.app $(MACPLAYER)

blender_app_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config

blender_app_SOURCES =

blenderplayer_app_SOURCES =

blender_app_LDADD = \
        source/libblender_source.la \
        intern/libblender_intern.la \
        $(blenpythonlib) \
        -lpython@PYTHON_VERSION@

blenderplayer_app_LDADD = \
	source/libblender_player.la \
        intern/libblender_intern.la \
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
        intern/libblender_intern.la \
        $(blenpythonlib) \
        -lpython@PYTHON_VERSION@

blenderplayer_LDADD = \
	source/libblender_player.la \
        intern/libblender_intern.la \
        $(blenpythonlib) \
        -lpython@PYTHON_VERSION@
endif
