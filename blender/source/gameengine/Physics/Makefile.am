noinst_LIBRARIES = libphysics.a

#there are some test prgrams under sumo/fuzzics/sample directory... if anyome wants to build them they should modify this file

libphysics_a_SOURCES = \
		Sumo/SumoPhysicsController.cpp \
		Sumo/SumoPhysicsEnvironment.cpp \
		Sumo/Fuzzics/src/SM_FhObject.cpp \
		Sumo/Fuzzics/src/SM_Scene.cpp \
		Sumo/Fuzzics/src/SM_Object.cpp \
		common/PHY_IMotionState.cpp \
		common/PHY_IPhysicsController.cpp \
		common/PHY_IPhysicsEnvironment.cpp \
		BlOde/OdePhysicsController.cpp \
		BlOde/OdePhysicsEnvironment.cpp \
		Dummy/DummyPhysicsEnvironment.cpp

INCLUDES = -I$(top_srcdir)/source/gameengine/Physics/Sumo/Fuzzics/include -I$(top_srcdir)/source/gameengine/Physics/common -I$(top_srcdir)/source/gameengine/Physics/Sumo/include -I$(top_srcdir)/intern/moto/include -I$(top_srcdir)/source/ode/include