#
# $Id: Makefile,v 1.3 2002/01/16 14:34:09 frank Exp $
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

include nan_definitions.mk

DIR = $(OCGDIR)/$(DEBUG_DIR)

all:: 
	@# set up directory structure for the OSX aplication bundle
	@echo "---> creating directory structure for $(APPLICATION)"
	@rm -rf $(DIR)/$(APPLICATION).app
	@cp -R $(APPLICATION).app $(DIR)
	@#cat $(APPLICATION).app/Contents/Info.plist | sed s/VERSION/`cat ../../VERSION`/ | sed s/DATE/`date +'%Y-%b-%d'`/ > $(DIR)/$(APPLICATION).app/Contents/Info.plist
	@echo "---> copying binary"
	@cp $(DIR)/$(APPLICATION) $(DIR)/$(APPLICATION).app/Contents/MacOS/
	@echo "---> removing CVS directories and Mac hidden files from distribution"
	@find $(DIR)/$(APPLICATION).app -name CVS -prune -exec rm -rf {} \;
	@find $(DIR)/$(APPLICATION).app -name .DS_Store -exec rm -f {} \;
