noinst_LTLIBRARIES = libblender_kernel.la

LIBS =

INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/intern/string \
	-I$(top_srcdir)/intern/moto/include


libblender_kernel_la_SOURCES = \
	gen_messaging/GEN_messaging.h \
	gen_messaging/intern/messaging.c \
	gen_system/GEN_DataCache.h \
	gen_system/GEN_HashedPtr.cpp \
	gen_system/GEN_HashedPtr.h \
	gen_system/GEN_Map.h \
	gen_system/GEN_Matrix4x4.cpp \
	gen_system/GEN_Matrix4x4.h \
	gen_system/GEN_SmartPtr.h \
	gen_system/SYS_SingletonSystem.cpp \
	gen_system/SYS_SingletonSystem.h \
	gen_system/SYS_System.cpp \
	gen_system/SYS_System.h

EXTRA_DIST = \
	Makefile \
	gen_messaging/Makefile \
	gen_messaging/intern/Makefile \
	gen_system/Makefile
