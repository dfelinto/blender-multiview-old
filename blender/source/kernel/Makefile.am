noinst_LTLIBRARIES = libblender_kernel.la

INCLUDES = -I$(top_srcdir)/source/kernel/gen_messaging \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/intern/string \
	-I$(top_srcdir)/intern/moto/include


libblender_kernel_la_SOURCES = gen_system/SYS_SingletonSystem.cpp \
	gen_system/GEN_HashedPtr.cpp \
	gen_system/GEN_Matrix4x4.cpp \
	gen_system/SYS_System.cpp \
	gen_messaging/intern/messaging.c
