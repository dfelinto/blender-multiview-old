noinst_LIBRARIES = libfrozen.a

SRCDIR = $(top_srcdir)/intern/python/modules

MODULE_FILES = $(shell cat ../frozen/src.list)

PYFILES = \
	bkfile.py \
	checkextensions.py \
	checkextensions_win32.py \
	freeze.py \
	freeze_ac.py \
	hello.py \
	makeconfig.py \
	makefreeze.py \
	makemakefile.py \
	makemakefile_ac.py \
	modulefinder.py \
	parsesetup.py \
	winmakemakefile.py

libfrozen_a_SOURCES =

EXTRA_DIST = README.NaN $(PYFILES)

libfrozen.a:	../frozen/src.list $(PYFILES) $(MODULE_FILES)

../frozen/src.list:	$(PYFILES) $(MODULE_FILES)
	@PYTHON@ -S -O \
		$(top_srcdir)/intern/python/freeze/freeze_ac.py -d -x os -x \
		pprint -I $(SRCDIR) -o ../frozen $(SRCDIR)/VRMLmain.py
	cd ../frozen && $(MAKE) && cd ../freeze

clean:;
	rm -f *.pyo
	cd ../frozen && rm -f *.o *.list && cd ../freeze

distclean:;
	rm -f *.pyc
	cd ../frozen && rm -f *.* && cd ../freeze
