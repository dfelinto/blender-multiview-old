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
	intern/iksolver/libblender_IK.la \
	intern/moto/libblender_MT.la

if EXPPYTHON
  blenpythonlib =
else
  blenpythonlib = intern/python/frozen/libfrozen.a
endif

if BlenderGAME
PLAYER=blenderplayer
else
PLAYER=
endif

bin_PROGRAMS = blender $(PLAYER)

if BlenderPLUGIN

lib_LTLIBRARIES = libbrowserplugin.la

libbrowserplugin_la_SOURCES =

libbrowserplugin_la_LDFLAGS =  \
	$(blenpythonlib) \
	-lpython@PYTHON_VERSION@ \
	@LIBM@

libbrowserplugin_la_LIBADD = \
	source/libblender_plugin.la \
	$(internlibs) \
	@LIBM@

endif

## Assuming no reason to build creator anymore lets just call publisher blender
if CARBON

if BlenderQUICKTIME
quicktimeldflags = -framework QuickTime
else
quicktimeldflags = 
endif

blender_LDFLAGS = -framework OpenGL -framework AGL -framework Carbon \
				  $(quicktimeldflags)

blender_SOURCES =

blenderplayer_SOURCES =

blender_LDADD = \
	source/libblender_source.la \
	$(internlibs) \
	$(blenpythonlib) \
	@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config/libpython@PYTHON_VERSION@.a \
	@LIBM@

blenderplayer_LDADD = \
	source/libblender_player.la \
	$(internlibs) \
	$(blenpythonlib) \
	@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config/libpython@PYTHON_VERSION@.a \
	@LIBM@

all-local: 
	@for bundle in $(bin_PROGRAMS); do APPLICATION=$$bundle $(MAKE) bundle; done

bundle:
	@# set up directory structure for the OSX aplication bundle
	@echo "---> creating directory structure for $(APPLICATION)"
	@rm -rf $(APPLICATION).app
	@cp -R $(top_srcdir)/source/darwin/$(APPLICATION).app .
	@cat $(top_srcdir)/source/darwin/$(APPLICATION).app/Contents/Info.plist | \
		sed s/VERSION/`cat $(top_srcdir)/release/VERSION`/ | \
		sed s/DATE/`date +'%Y-%b-%d'`/ > $(APPLICATION).app/Contents/Info.plist
	@echo "---> copying binary"
	@cp $(APPLICATION) $(APPLICATION).app/Contents/MacOS/
	@echo "---> adding excutable attributes"
	@chmod +x $(APPLICATION).app/Contents/MacOS/$(APPLICATION)
	@echo "---> removing CVS directories and Mac hidden files from distribution"
	@find $(APPLICATION).app -name CVS -prune -exec rm -rf {} \;
	@find $(APPLICATION).app -name .DS_Store -exec rm -f {} \;

else

blender_LDFLAGS = -L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config @PYTHON_LDFLAGS@ 

blender_SOURCES =

blenderplayer_SOURCES =

blender_LDADD = \
        source/libblender_source.la \
		$(internlibs) \
		$(blenpythonlib) \
		-lpython@PYTHON_VERSION@ \
		@LIBM@
 

blenderplayer_LDADD = \
	source/libblender_player.la \
	$(internlibs) \
	$(blenpythonlib) \
	-lpython@PYTHON_VERSION@ \
		@LIBM@
	


endif
