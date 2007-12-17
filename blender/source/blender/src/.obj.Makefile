#
# $Id$
#

SDIR = $(HOME)/develop/source/blender/src

all debug clean:
	@echo "****> Object Makefile, chdir to $(SDIR) ..."
	@$(MAKE) -C $(SDIR) $@ || exit 1;
