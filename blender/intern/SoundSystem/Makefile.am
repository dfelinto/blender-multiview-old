noinst_LTLIBRARIES = libgameblender_soundsystem.la

if BlenderFMOD
FMOD_DEVICE = \
	fmod/SND_FmodDevice.cpp \
	fmod/SND_FmodDevice.h
FMOD_DIST =
else
FMOD_DEVICE =
FMOD_DIST = \
	fmod/SND_FmodDevice.cpp \
	fmod/SND_FmodDevice.h
endif

if BlenderOPENAL
OPENAL_DEVICE = \
	openal/SND_OpenALDevice.cpp \
	openal/SND_OpenALDevice.h \
	openal/pthread_cancel.cpp
OPENAL_DIST =
else
OPENAL_DEVICE =
OPENAL_DIST = \
	openal/SND_OpenALDevice.cpp \
	openal/SND_OpenALDevice.h \
	openal/pthread_cancel.cpp
endif

#there is a test file for those who want to build it... as long as you edit this file

#note fmod might be a windows only file as it includes from the windows library

libgameblender_soundsystem_la_SOURCES = \
	SND_C-api.h \
	SND_CDObject.h \
	SND_DependKludge.h \
	SND_DeviceManager.h \
	SND_IAudioDevice.h \
	SND_Object.h \
	SND_Scene.h \
	SND_SoundListener.h \
	SND_SoundObject.h \
	SND_Utils.h \
	SND_WaveCache.h \
	SND_WaveSlot.h \
	SoundDefines.h \
	dummy/SND_DummyDevice.cpp \
	dummy/SND_DummyDevice.h \
	$(FMOD_DEVICE) \
	intern/SND_AudioDevice.cpp \
	intern/SND_AudioDevice.h \
	intern/SND_C-api.cpp \
	intern/SND_CDObject.cpp \
	intern/SND_DeviceManager.cpp \
	intern/SND_IdObject.cpp \
	intern/SND_IdObject.h \
	intern/SND_Scene.cpp \
	intern/SND_SoundListener.cpp \
	intern/SND_SoundObject.cpp \
	intern/SND_Utils.cpp \
	intern/SND_WaveCache.cpp \
	intern/SND_WaveSlot.cpp \
	$(OPENAL_DEVICE)

INCLUDES = -I$(top_srcdir)/lib/windows/fmod/include \
	-I$(top_srcdir)/source/gameengine/SoundSystem/intern \
	-I$(top_srcdir)/intern/moto/include \
	-I$(top_srcdir)/intern/string \
	-I$(top_srcdir)/source/kernel/gen_system \
	-I$(top_srcdir)/source/gameengine/SoundSystem/dummy \
	-I$(top_srcdir)/source/gameengine/SoundSystem/openal

EXTRA_DIST = \
	Makefile \
	dummy/Makefile \
	fmod/Makefile \
	intern/Makefile \
	openal/Makefile \
	$(FMOD_DIST) \
	$(OPENAL_DIST)
