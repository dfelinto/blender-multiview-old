noinst_LTLIBRARIES = libblender_belgium.la

LIBS =

INCLUDES = -I. -I$(top_srcdir)/intern/keymaker/python

libblender_belgium_la_SOURCES = \
	blenkey.h \
	key_internal.h \
	mt19937int.h \
	python/key_pyc.h \
	key.c \
	mt19937int.c

## Keyloader is a seperate test program
bin_PROGRAMS = keyloader
keyloader_SOURCES = keyloader.c
keyloader_LDADD = -lcrypto libblender_belgium.la
