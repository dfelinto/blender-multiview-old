SUBDIRS = dynamics

noinst_PROGRAMS = sample gldemo physics mnm 

sample_SOURCES = sample.cpp
gldemo_SOURCES = gldemo.cpp
physics_SOURCES = physics.cpp
mnm_SOURCES = mnm.cpp

GLLIBS = -lglut -lGLU -lGL -L/usr/X11R6/lib -lXmu -lXi -lX11

sample_LDADD = ../src/libsolid.la $(QHULL_LIBS)
gldemo_LDADD = ../src/libsolid.la $(QHULL_LIBS) $(GLLIBS)
physics_LDADD = dynamics/libdynamics.la ../src/libsolid.la $(QHULL_LIBS) $(GLLIBS)
mnm_LDADD = dynamics/libdynamics.la ../src/libsolid.la $(QHULL_LIBS) $(GLLIBS)

AM_CPPFLAGS = -Idynamics

EXTRA_DIST = README.txt
