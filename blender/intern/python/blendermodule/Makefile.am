noinst_LTLIBRARIES = libblender_module.la

libblender_module_la_LDFLAGS = -static \
	-L@PYTHONSYSPREFIX@/lib/python@PYTHON_VERSION@/config \
	-lpython@PYTHON_VERSION@ @X_EXTRA_LIBS@ -lm

INCLUDES = -I@PYTHONSYSPREFIX@/include/python@PYTHON_VERSION@

libblender_module_la_SOURCES = main.c
