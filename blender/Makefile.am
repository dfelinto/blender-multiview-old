SUBDIRS = intern source

if BlenderGAME
MACPLAYER=blenderplayer.app
PLAYER=blenderplayer
else
MACPLAYER=
PLAYER=
endif

## Assuming no reason to build creator anymore lets just call publisher blender
if CARBON
## bin_PROGRAMS = blender.app blenderplayer.app plugin.app
bin_PROGRAMS = blender.app $(MACPLAYER)

blender_app_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config

blender_app_SOURCES =

blenderplayer_app_SOURCES =

blender_app_LDADD = \
        -lpython@PYTHON_VERSION@ \
        source/libblender_source.la \
        intern/libblender_intern.la \
        intern/python/frozen/libfrozen.a

blenderplayer_app_LDADD = \
        -lpython@PYTHON_VERSION@ \
	source/libblender_player.la \
        intern/libblender_intern.la \
        intern/python/frozen/libfrozen.a

else

## bin_PROGRAMS = blender blenderplayer plugin
bin_PROGRAMS = blender $(PLAYER)

blender_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
         $(X_LIBS)

blender_SOURCES =

blenderplayer_SOURCES =

blender_LDADD = \
        -lpython@PYTHON_VERSION@ \
        source/libblender_source.la \
        intern/libblender_intern.la \
        intern/python/frozen/libfrozen.a

blenderplayer_LDADD = \
        -lpython@PYTHON_VERSION@ \
	source/libblender_player.la \
        intern/libblender_intern.la \
        intern/python/frozen/libfrozen.a

endif
