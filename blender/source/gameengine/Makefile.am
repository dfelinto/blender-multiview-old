SUBDIRS = BlenderRoutines Converter Expressions GameLogic Ketsji \
	Rasterizer SceneGraph SoundSystem Network GamePlayer Physics

noinst_LTLIBRARIES = libblender_gameengine.la

libblender_gameengine_la_SOURCES =
libblender_gameengine_la_LIBADD = \
	BlenderRoutines/libgameblender_routines.la \
	Converter/libgameblender_converter.la \
	Expressions/libgameblender_expressions.la \
	GameLogic/libgameblender_logic.la \
	Ketsji/libgameblender_ketsji.la \
	Rasterizer/libgameblender_rasterizer.la \
	SceneGraph/libgameblender_scenegraph.la \
	SoundSystem/libgameblender_soundsystem.la \
	Network/libgameblender_network.la \
## Need to add GamePlayer here...
	Physics/Sumo/Fuzzics/libfuzzics.la \
	Physics/Sumo/libgameblender_physics_sumo.la \
	Physics/libgameblender_physics.la
