#
# $Id$
#

SDIR = $(HOME)/develop/source

all debug link debuglink clean linkclean debuglinkclean:
	@echo "****> Object Makefile, chdir to $(SDIR) ..."
	@$(MAKE) -C $(SDIR) $@ || exit 1;
