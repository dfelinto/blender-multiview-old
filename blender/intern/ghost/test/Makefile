#
# $Id$
#
# ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
#
# The contents of this file may be used under the terms of either the GNU
# General Public License Version 2 or later (the "GPL", see
# http://www.gnu.org/licenses/gpl.html ), or the Blender License 1.0 or
# later (the "BL", see http://www.blender.org/BL/ ) which has to be
# bought from the Blender Foundation to become active, in which case the
# above mentioned GPL option does not apply.
#
# The Original Code is Copyright (C) 2002 by NaN Holding BV.
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): none yet.
#
# ***** END GPL/BL DUAL LICENSE BLOCK *****
# GHOST test applications makefile.
# This bounces to test application directories.
#

LIBNAME = ghost
SOURCEDIR = intern/$(LIBNAME)/test
DIR = $(OCGDIR)/$(SOURCEDIR)
DIRS = gears

include nan_subdirs.mk

include nan_compile.mk
include nan_link.mk

OCGGHOST = $(OCGDIR)/intern/$(LIBNAME)
GEARDIR = $(OCGGHOST)/test/$(DEBUG_DIR)gears.app

LIBS = $(OCGGHOST)/$(DEBUG_DIR)libghost.a
SLIBS += $(LCGDIR)/string/lib/libstring.a

all debug:: $(LIBS)
	@echo "****> linking $@ in $(SOURCEDIR)"
ifeq ($(OS),darwin)
	$(CCC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)gears $(DIR)/$(DEBUG_DIR)GHOST_Test.o $(LIBS) $(SLIBS) $(LLIBS) $(DADD) $(LOPTS)
	@# set up directory structure for the OSX application bundle
	@[ -d $(OCGGHOST)/test/ ] || mkdir $(OCGGHOST)/test/
	@[ -d $(OCGGHOST)/test/debug ] || mkdir $(OCGGHOST)/test/debug
	@[ -d $(GEARDIR) ] || mkdir $(GEARDIR)
	@[ -d $(GEARDIR)/Contents ] || mkdir $(GEARDIR)/Contents
	@[ -d $(GEARDIR)/Contents/MacOS ] || mkdir $(GEARDIR)/Contents/MacOS
	@[ -d $(GEARDIR)/Contents/Resources ] || mkdir $(GEARDIR)/Contents/Resources
	@[ -d $(GEARDIR)/Contents/Resources/English.lproj ] || mkdir $(GEARDIR)/Contents/Resources/English.lproj
	@[ -d $(GEARDIR)/Contents/Resources/English.lproj/MainMenu.nib ] || mkdir $(GEARDIR)/Contents/Resources/English.lproj/MainMenu.nib
	@# copy the files into the bundle directory tree
	cp -f $(DIR)/$(DEBUG_DIR)gears $(GEARDIR)/Contents/MacOS
	cp -f gears/resources/osx/PkgInfo $(GEARDIR)/Contents/
	cp -f gears/resources/osx/Info.plist $(GEARDIR)/Contents/
	cp -f gears/resources/osx/English.lproj/InfoPlist.strings $(GEARDIR)/Contents/Resources/English.lproj
	cp -f gears/resources/osx/English.lproj/MainMenu.nib/classes.nib $(GEARDIR)/Contents/Resources/English.lproj
	cp -f gears/resources/osx/English.lproj/MainMenu.nib/info.nib $(GEARDIR)/Contents/Resources/English.lproj
	cp -f gears/resources/osx/English.lproj/MainMenu.nib/objects.nib $(GEARDIR)/Contents/Resources/English.lproj
else
	$(CCC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)gears_cpp $(DIR)/$(DEBUG_DIR)GHOST_Test.o $(LIBS) $(SLIBS) $(LLIBS) $(DADD) $(LOPTS)
	$(CCC) $(LDFLAGS) -o $(DIR)/$(DEBUG_DIR)gears_c $(DIR)/$(DEBUG_DIR)GHOST_C-Test.o $(LIBS) $(SLIBS) $(LLIBS) $(DADD) $(LOPTS)
endif

clean::
	@# mac stuff. well ok, only the binary
	@rm -f $(DIR)/gears     $(DIR)/debug/gears
	@# others
	@rm -f $(DIR)/gears_c   $(DIR)/debug/gears_c
	@rm -f $(DIR)/gears_cpp $(DIR)/debug/gears_cpp

test:: all
	$(DIR)/gears_cpp
	$(DIR)/gears_c
