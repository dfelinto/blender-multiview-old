noinst_LTLIBRARIES = libgameblender_soundsystem.la

if BlenderFMOD
FMOD_DEVICE = fmod/SND_FmodDevice.cpp
else
FMOD_DEVICE =
endif

if BlenderOPENAL
OPENAL_DEVICE = openal/SND_OpenALDevice.cpp \
	openal/pthread_cancel.cpp
else
OPENAL_DEVICE =
endif

#there is a test file for those who want to build it... as long as you edit this file

#note fmod might be a windows only file as it includes from the windows library

libgameblender_soundsystem_la_SOURCES = \
	dummy/SND_DummyDevice.cpp \
	$(FMOD_DEVICE) \
	intern/SND_AudioDevice.cpp \
	intern/SND_C-api.cpp \
	intern/SND_CDObject.cpp \
	intern/SND_DeviceManager.cpp \
	intern/SND_IdObject.cpp \
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
