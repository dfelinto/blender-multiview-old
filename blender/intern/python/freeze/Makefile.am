noinst_LIBRARIES = libfrozen.a

SRCDIR = $(top_srcdir)/intern/python/modules

PYFILES = \
	bkfile.py \
	checkextensions.py \
	checkextensions_win32.py \
	freeze.py \
	hello.py \
	makeconfig.py \
	makefreeze.py \
	makemakefile.py \
	modulefinder.py \
	parsesetup.py \
	winmakemakefile.py

EXTRA_DIST = $(top_srcdir)/intern/python/frozen/*.c README.NaN $(PYFILES)

libfrozen.a: $(PYFILES)
	@PYTHON@@PYTHON_VERSION@ -S -O \
		$(top_srcdir)/intern/python/frozen/freeze.py -d -x os -x \
		pprint -I $(SRCDIR) $(SRCDIR)/VRMLmain.py
#Or leave out @PYTHON@@PYTHON_VERSION@?
	cd ../frozen && make -f Makefile.freeze && cd ../freeze

clean:;
	rm -f *.pyo
	cd ../frozen && rm *.o && cd ../freeze

distclean:;
	rm -f *.pyc
	cd ../frozen && rm -f *.* && cd ../freeze
