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
#
# Bounce make to subdirectories.
# Set DIRS, SOURCEDIR. Optionally also reacts on DIR, TESTDIRS.
#

default: all

# do not add install here. install target can only be used in intern/
# top level Makefiles
all debug clean::
    ifdef DIR
	@# Make sure object toplevels are there
	@[ -d $(NAN_OBJDIR) ] || mkdir $(NAN_OBJDIR)
	@[ -d $(LCGDIR) ] || mkdir $(LCGDIR)
	@[ -d $(OCGDIR) ] || mkdir $(OCGDIR)
	@[ -d $(OCGDIR)/intern ] || mkdir $(OCGDIR)/intern
	@[ -d $(OCGDIR)/extern ] || mkdir $(OCGDIR)/extern
	@# Create object directory
	@[ -d $(DIR) ] || mkdir $(DIR)
    endif
	@for i in $(DIRS); do \
	    echo "====> $(MAKE) $@ in $(SOURCEDIR)/$$i" ;\
	    $(MAKE) -C $$i $@ || exit 1; \
	done

test::
    ifdef TESTDIRS
	@for i in $(TESTDIRS); do \
	    echo "====> $(MAKE) $@ in $(SOURCEDIR)/$$i" ;\
	    $(MAKE) -C $$i $@ || exit 1; \
	done
    endif

