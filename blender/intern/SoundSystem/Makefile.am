noinst_LIBRARIES = libsoundsystem.a

#there is a test file for those who want to build it... as long as you edit this file

#note fmod might be a windows only file as it includes from the windows library

libsoundsystem_a_SOURCES = \
		dummy/SND_DummyDevice.cpp \
		fmod/SND_FmodDevice.cpp \
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
		openal/SND_OpenALDevice.cpp \
		openal/pthread_cancel.cpp

INCLUDES = -I$(top_srcdir)/lib/windows/fmod/include -I$(top_srcdir)/source/gameengine/SoundSystem/intern -I$(top_srcdir)/intern/moto/include -I$(top_srcdir)/intern/string -I$(top_srcdir)/source/kernel/gen_system -I$(top_srcdir)/source/gameengine/SoundSystem/dummy -I$(top_srcdir)/source/gameengine/SoundSystem/openal