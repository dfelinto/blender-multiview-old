SUBDIRS = intern source

if BlenderGAME
MACPLAYER=blenderplayer.app
PLAYER=blenderplayer
else
MACPLAYER=
PLAYER=
endif

if BlenderPLUGIN

lib_LTLIBRARIES = browserplugin.la

browserplugin_la_SOURCES =

browserplugin_la_LDFLAGS =  \
	intern/python/frozen/libfrozen.a \
	-lpython@PYTHON_VERSION@

browserplugin_la_LIBADD = \
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
        intern/python/frozen/libfrozen.a \
        -lpython@PYTHON_VERSION@

blenderplayer_app_LDADD = \
	source/libblender_player.la \
        intern/libblender_intern.la \
        intern/python/frozen/libfrozen.a \
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
        intern/python/frozen/libfrozen.a \
        -lpython@PYTHON_VERSION@

blenderplayer_LDADD = \
	source/libblender_player.la \
        intern/libblender_intern.la \
        intern/python/frozen/libfrozen.a \
        -lpython@PYTHON_VERSION@
endif
