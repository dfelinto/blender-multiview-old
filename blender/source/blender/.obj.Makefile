#
# $Id$
#

SDIR = $(HOME)/develop/source/blender

all debug clean:
	@echo "****> Object Makefile, chdir to $(SDIR) ..."
	@$(MAKE) -C $(SDIR) $@ || exit 1;
