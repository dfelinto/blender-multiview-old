if BlenderGAME
gameplayerdir = GamePlayer
gameplayerlib = libgameblender_player.la
else
gameplayerdir =
gameplayerlib =
endif

SUBDIRS = BlenderRoutines Converter Expressions GameLogic Ketsji \
	Rasterizer SceneGraph SoundSystem Network Physics $(gameplayerdir)

noinst_LTLIBRARIES = libblender_gameengine.la $(gameplayerlib)

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
	Physics/libgameblender_physics.la

## I don't think we need these but hey maybe we do... Kent
##	Physics/Sumo/Fuzzics/libfuzzics.la \
##	Physics/Sumo/libgameblender_physics_sumo.la 

libgameblender_player_la_SOURCES = 
libgameblender_player_la_LIBADD = \
	GamePlayer/libgameblender_player.la
