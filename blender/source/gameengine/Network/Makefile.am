noinst_LTLIBRARIES = libgameblender_network.la

libgameblender_network_la_SOURCES = \
	NG_NetworkDeviceInterface.h \
	NG_NetworkMessage.cpp \
	NG_NetworkMessage.h \
	NG_NetworkObject.cpp \
	NG_NetworkObject.h \
	NG_NetworkScene.cpp \
	NG_NetworkScene.h \
	LoopBackNetwork/NG_LoopBackNetworkDeviceInterface.cpp \
	LoopBackNetwork/NG_LoopBackNetworkDeviceInterface.h

INCLUDES = -I$(top_srcdir)/intern/string \
	-I$(top_srcdir)/intern/moto/include

EXTRA_DIST = \
	Makefile \
	LoopBackNetwork/Makefile \
	TerraplayNetwork/Makefile \
	TerraplayNetwork/NG_TerraplayNetworkDeviceInterface.cpp \
	TerraplayNetwork/NG_TerraplayNetworkDeviceInterface.h
