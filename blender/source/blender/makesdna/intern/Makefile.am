noinst_PROGRAMS = makesdna

makesdna_SOURCES = makesdna.c
makesdna_LDADD = $(top_builddir)/intern/guardedalloc/libblender_guardedalloc.la
makesdna_CPPFLAGS = -DBASE_HEADER="\"$(top_srcdir)/source/blender/makesdna/\""
makesdna_CFLAGS = $(makesdna_CPPFLAGS)

INCLUDES = \
	-I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/makesdna 

# maybe there's a better way...
dna.c: makesdna
#	cp -f $(top_srcdir)/source/blender/makesdna/*.h $(top_builddir)/source/blender/makesdna
	./makesdna$(EXEEXT) dna.c

BUILT_SOURCES = dna.c
CLEANFILES = dna.c

