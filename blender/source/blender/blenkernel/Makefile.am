if EXPPYTHON
  blenpythoninclude = -I$(top_srcdir)/source/blender/python
else
  blenpythoninclude = -I$(top_srcdir)/source/blender/bpython/include
endif

noinst_LTLIBRARIES = libblender_blenkernel.la

LIBS =

libblender_blenkernel_la_SOURCES = \
	BKE_action.h \
	BKE_anim.h \
	BKE_armature.h \
	BKE_bad_level_calls.h \
	BKE_blender.h \
	BKE_bmfont.h \
	BKE_bmfont_types.h \
	BKE_booleanops.h \
	BKE_booleanops_mesh.h \
	BKE_constraint.h \
	BKE_curve.h \
	BKE_deform.h \
	BKE_displist.h \
	BKE_effect.h \
	BKE_endian.h \
	BKE_exotic.h \
	BKE_font.h \
	BKE_global.h \
	BKE_group.h \
	BKE_ika.h \
	BKE_image.h \
	BKE_ipo.h \
	BKE_key.h \
	BKE_lattice.h \
	BKE_library.h \
	BKE_main.h \
	BKE_material.h \
	BKE_mball.h \
	BKE_mesh.h \
	BKE_nla.h \
	BKE_object.h \
	BKE_osa_types.h \
	BKE_packedFile.h \
	BKE_plugin_types.h \
	BKE_property.h \
	BKE_sca.h \
	BKE_scene.h \
	BKE_screen.h \
	BKE_sound.h \
	BKE_subsurf.h \
	BKE_text.h \
	BKE_texture.h \
	BKE_utildefines.h \
	BKE_world.h \
	BKE_writeavi.h \
	intern/constraint.c \
	intern/group.c \
	intern/material.c \
	intern/sca.c \
	intern/world.c \
	intern/curve.c \
	intern/ika.c \
	intern/mball.c \
	intern/scene.c \
	intern/writeavi.c \
	intern/action.c \
	intern/deform.c \
	intern/image.c \
	intern/mesh.c \
	intern/screen.c \
	intern/anim.c \
	intern/displist.c \
	intern/ipo.c \
	intern/nla.c \
	intern/sound.c \
	intern/armature.c \
	intern/effect.c \
	intern/key.c \
	intern/object.c \
	intern/subsurf.c \
	intern/blender.c \
	intern/exotic.c \
	intern/lattice.c \
	intern/packedFile.c \
	intern/text.c \
	intern/bmfont.c \
	intern/font.c \
	intern/library.c \
	intern/property.c \
	intern/texture.c

INCLUDES = -I$(top_srcdir)/intern/guardedalloc \
	-I$(top_srcdir)/source/blender/include \
	-I$(top_srcdir)/source/blender/blenlib \
	-I$(top_srcdir)/source/blender/makesdna \
	$(blenpythoninclude) \
	-I$(top_srcdir)/source/blender/render/extern/include \
	-I$(top_srcdir)/source/blender/imbuf \
	-I$(top_srcdir)/source/blender/avi \
	-I$(top_srcdir)/intern/iksolver/extern \
	-I$(top_srcdir)/source/blender/blenloader

EXTRA_DIST = \
	Makefile \
	intern/Makefile
