dnl This file should be processed with GNU Autoconf to produce a "configure"
dnl script, with which a user can:
dnl - check for build requirements/options (headers, libraries, functions,
dnl   data types);
dnl - handle user definable build options (compiler flags, extra features);
dnl - parse the ".in" files into standard files (Makefile's, manpages, etc.).

dnl --------------------
dnl AC/AM Initialization
dnl --------------------

  AC_INIT(blender-creator, 2.26, )
  AC_CANONICAL_SYSTEM
  AM_INIT_AUTOMAKE([blender-creator], [2.26])

dnl check for the platform make
AC_PATH_PROGS(U_MAKE, gmake gnumake, make)
AC_SUBST(U_MAKE)

dnl -------------------
dnl Checks for programs
dnl -------------------

  AC_PROG_CXX
  AC_PATH_XTRA
  AC_PROG_CC
  AC_PROG_LN_S
  AC_PROG_LIBTOOL
  AC_PROG_RANLIB
  AC_CHECK_TOOL([STRIP],[strip])
  AC_PROG_AWK

dnl -----------------------------
dnl Debug builds & compiler flags
dnl -----------------------------

if test "x$GCC" = xyes; then
  CFLAGS="$CFLAGS -funsigned-char"
  CXXFLAGS="$CXXFLAGS -funsigned-char"
fi

case "$target" in
  *sparc* )
        AC_DEFINE(SUN_OGL_NO_VERTEX_MACROS,1,[Fix for Sun's GL])
  ;;  
  *-*-powerpc )
  ;;
  *-*-irix* )
     AC_CHECK_LIB([cl],[clSetParams])
     AC_CHECK_LIB([dmedia],[dmACCreate])
     AC_CHECK_LIB([C],[free])
     AC_CHECK_LIB([movieGL], [mvOpenFile])
     IRIX_SSL="/usr/freeware"
     LDFLAGS="$LDFLAGS -L/usr/freeware/lib32"
     if test "x$GCC" = xyes; then
        CXXFLAGS="$CXXFLAGS -DNO_FAST_ATOMS"
     fi

  ;;
  *-*-darwin* )
dnl  	AC_CHECK_HEADER([/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/gl.h]) 
dnl  	AC_CHECK_HEADER([/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/glu.h]) 
     SYSTEM_LIBS="$SYSTEM_LIBS -framework OpenGL -framework AGL"
     CPPFLAGS="$CPPFLAGS -D_THREAD_SAFE"
  ;;
  *-*-openbsd* )
     CPPFLAGS="$CPPFLAGS -DFreeBSD"
     LDFLAGS="$LDFLAGS -pthread"
  ;;
  *-*-freebsd* )
     CPPFLAGS="$CPPFLAGS -DFreeBSD -D_THREAD_SAFE"
  ;;
  *-*-beos* )
     CPPFLAGS="$CPPFLAGS -D__BeOS"
  ;;
esac

dnl -----------------------
dnl User definable features
dnl -----------------------

  AC_ARG_ENABLE(blenderstatic,
     AC_HELP_STRING([--enable-blenderstatic],[Build static binary (default=disable)]),
     ,dnl  default assign.
     [ enable_blenderstatic=no ]  dnl default setting.
  )

  AC_ARG_ENABLE(gameblender,
   AC_HELP_STRING([--enable-gameblender],[Build game engine (default=disable)]),
   ,dnl default assign.
   [ enable_gameblender=no ]  dnl default setting.
  )

  AC_ARG_ENABLE(blenderplugin,
   AC_HELP_STRING([--enable-blenderplugin],[Build browser plugin(default=disable)]),
   ,dnl default assign.
   [ enable_blenderplugin=no ]  dnl default setting.
  )

  AC_ARG_ENABLE(exppython,
   AC_HELP_STRING([--enable-exppython],[Build experimental Python(default=disable)]),
   ,dnl default assign.
   [ enable_exppython=no ]  dnl default setting.
  )

dnl ----------------------
dnl ssl
dnl ----------------------
  AC_ARG_WITH(ssl, 
    AC_HELP_STRING([--with-ssl[=PFX]],[Path to ssl files]),
[       if test $withval != "yes"; then
		if test -f $withval/include/openssl/ssl.h; then
           		LDFLAGS="$LDFLAGS -L$withval/lib"
           		CPPFLAGS="$CPPFLAGS -I$withval/include"
		fi
	fi
],
[
	if test "x$IRIX_SSL" != "x"; then
		if test -f $IRIX_SSL/include/openssl/ssl.h; then
			LDFLAGS="$LDFLAGS -L$IRIX_SSL/lib"
			CPPFLAGS="$CPPFLAGS -I$IRIX_SSL/include"
		fi
	fi
	
])

  AC_ARG_WITH(ode,
    AC_HELP_STRING([--with-ode[=PFX]],[Path to ode files]),
[       if test $withval != "yes"; then
        	if test -f $withval/include/ode/ode.h; then
           		LDFLAGS="$LDFLAGS -L$withval/lib"
           		CPPFLAGS="$CPPFLAGS -I$withval/include"
		fi
        fi
])

  AC_ARG_WITH(libjpeg,
    AC_HELP_STRING([--with-libjpeg[=PFX]],[Path to libjpeg files]),
[       if test $withval != "yes"; then
		if test -f  $withval/include/jpeglib.h; then
           		LDFLAGS="$LDFLAGS -L$withval/lib"
           		CPPFLAGS="$CPPFLAGS -I$withval/include"
		fi
        fi
])

  AC_ARG_WITH(libpng,
    AC_HELP_STRING([--with-libpng[=PFX]],[Path to libpng files]),
[       if test $withval != "yes"; then
        	if test -f  $withval/include/png.h; then
           		LDFLAGS="$LDFLAGS -L$withval/lib"
           		CPPFLAGS="$CPPFLAGS -I$withval/include"
		fi
        fi
])

  AC_ARG_WITH(libz,
    AC_HELP_STRING([--with-libz[=PFX]],[Path to libz files]),
[       if test $withval != "yes"; then
                if test -f  $withval/include/zlib.h; then
                        LDFLAGS="$LDFLAGS -L$withval/lib"
                        CPPFLAGS="$CPPFLAGS -I$withval/include"
                fi
	fi
])

  AC_ARG_WITH(mozilla,
    AC_HELP_STRING([--with-mozilla[=PFX]],[Path to mozilla files]),
[       if test $withval != "yes"; then
                with_mozilla =$withval
        fi
        if test x$with_mozilla != "yes"; then
           LDFLAGS="$LDFLAGS -L$withval/lib/mozilla-$MOZVER -lnspr"
           CPPFLAGS="$CPPFLAGS -I$withval/include/mozilla-$MOZVER/nspr",
        fi
])

#Refactor sound lib options.
  AC_ARG_ENABLE(openal,
   AC_HELP_STRING([--enable-openal],[Enable OpenAL in game engine (default=disable)]),
   ,dnl default assign.
   [ enable_openal=no ]  dnl default setting.
  )

  AC_ARG_ENABLE(fmod,
   AC_HELP_STRING([--enable-fmod],[Enable FMOD audio in game engine (default=disable)]),
   ,dnl default assign.
   [ enable_fmod=no ]  dnl default setting.
  )

#Python!
  AM_PATH_PYTHON(2.0)
#Find out what path to use for python.
python_sysprefix=`echo "import sys; print sys.prefix" | python -`

if test x"$enable_openal" = x"yes"; then
  XIPH_PATH_OGG
  XIPH_PATH_VORBIS
#  AM_PATH_SDL(1.0)
#  AM_PATH_SMPEG
fi


dnl -----------------------
dnl Checks for header files
dnl -----------------------

  AC_CONFIG_SRCDIR([source/creator/creator.c])
  AM_CONFIG_HEADER([config.h])

  AC_PATH_XTRA
  AC_HEADER_DIRENT
  AC_HEADER_STDC
  AC_HEADER_STAT
  AC_HEADER_SYS_WAIT
  AC_CHECK_HEADERS([fcntl.h float.h inttypes.h limits.h malloc.h memory.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/statfs.h sys/time.h sys/vfs.h unistd.h termios.h wchar.h wctype.h sys/socket.h])
  AC_CHECK_HEADER([openssl/crypto.h])
  AC_CHECK_HEADER([jpeglib.h])
  AC_CHECK_HEADER([png.h])
  AC_CHECK_HEADER([GL/gl.h])
  AC_CHECK_HEADER([GL/glu.h])
  AC_CHECK_HEADERS([alloca.h]) 
  AC_CHECK_HEADERS([arpa/inet.h]) 
  AC_CHECK_HEADERS([errno.h]) 
  AC_CHECK_HEADERS([netdb.h]) 

dnl ----------------
dnl determine what type of windowing we are using
dnl ----------------
  AC_CHECK_HEADER(windows.h)
  AM_CONDITIONAL(WIN32,[test "x$ac_cv_header_windows_h" = "xyes"])

  AC_CHECK_HEADER(Carbon/Carbon.h)
  AM_CONDITIONAL(CARBON,[test "x$ac_cv_header_Carbon_Carbon_h" = "xyes"])

  AC_CHECK_HEADER(X11/Xlib.h)
  AM_CONDITIONAL(X11,[test "x$ac_cv_header_X11_Xlib_h" = "xyes"])

  dnl AC_SUBST(blabla_libs)
  AC_SUBST([PYTHONSYSPREFIX],[$python_sysprefix])
#  AC_SUBST([BlenderSTATIC],[$enable_blenderstatic])
  AM_CONDITIONAL([BlenderSTATIC],[test x$enable_blenderstatic = x"yes"])
  AM_CONDITIONAL([BlenderGAME],[test x$enable_gameblender = x"yes"])
  AM_CONDITIONAL([BlenderPLUGIN],[test x$enable_blenderplugin = x"yes"])
  AM_CONDITIONAL([BlenderOPENAL],[test x$enable_openal = x"yes"])
  AM_CONDITIONAL([BlenderFMOD],[test x$enable_fmod = x"yes"])


dnl --------------------------
dnl Checks for POSIX functions
dnl --------------------------

  AC_FUNC_ERROR_AT_LINE
  AC_FUNC_LSTAT
  AC_FUNC_OBSTACK
dnl  complaints about not on some systems.  Not sure if its needed or not
dnl  mein@cs.umn.edu
dnl  AC_FUNC_REALLOC
  AC_FUNC_ALLOCA
  AC_FUNC_FNMATCH
  AC_FUNC_FORK
  AC_FUNC_MALLOC
  AC_FUNC_MEMCMP
  AC_FUNC_MMAP
  AC_TYPE_SIGNAL
  AC_FUNC_STAT
  AC_FUNC_STRFTIME
  AC_FUNC_VPRINTF
  AC_CHECK_FUNCS([endpwent floor getcwd gettimeofday memmove memset mkdir munmap pow select sqrt strcasecmp strchr strdup strncasecmp strpbrk strrchr strstr strtoul atexit fdatasync gethostname])

dnl --------------------------
dnl Checks for extra libraries
dnl --------------------------
  AC_CHECK_LIBM  dnl Use libtool.m4's provided macro to get the math library 
  AC_SUBST(LIBM)
  AC_CHECK_LIB([z], [gzopen])
  AC_LANG_PUSH([C++])
  AC_CHECK_LIB([stdc++], [printf]) dnl  Zarquon's knees, this is an evil hack.
  AC_LANG_POP([C++])
  AC_CHECK_LIB([dl], [dlopen])
  AC_CHECK_LIB([pthread], [pthread_exit])
  AC_CHECK_LIB([ssl], [SSL_new])
  AC_CHECK_LIB([crypto], [CRYPTO_lock])
  AC_CHECK_LIB([png], [png_get_header_version])
  AC_CHECK_LIB([jpeg], [jpeg_destroy])
  AC_CHECK_LIB([util], [openpty])
  AC_CHECK_LIB([SDL], [SDL_Init])
if test x"$enable_gameblender" = x"yes"; then
  AC_CHECK_LIB([ode], [dJointGetAMotorAngle],,
    AC_MSG_ERROR(First build ode or do not use --enable-gameblender))
fi
if test x"$enable_openal" = x"yes"; then
  AC_CHECK_LIB([smpeg], [SMPEG_getinfo])
  AC_CHECK_LIB([esd], [esd_close])
  AC_CHECK_LIB([ogg], [ogg_stream_destroy])
  AC_CHECK_LIB([vorbis], [vorbis_info_init])
  AC_CHECK_LIB([vorbisfile], [ov_open])
  AC_CHECK_LIB([openal], [alutInit] ,
   , dnl found -- nop
   [enable_openal=no] )  dnl not found -- disable openal
fi
  AC_CHECK_LIB([Xext], [XextFindDisplay],,,$X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
  AC_CHECK_LIB([X11], [XOpenDisplay],,,$X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
  AC_CHECK_LIB([GL], [glIsTexture],,,$X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
  AC_CHECK_LIB([GLU], [gluGetString],,,$X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
  AC_CHECK_LIB([Xxf86dga], [XDGAQueryModes],,,$X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)

dnl ---------------------
dnl Checks for data types
dnl ---------------------

  AC_C_CONST
  AC_C_INLINE
  AC_TYPE_MODE_T
  AC_TYPE_SIZE_T
  AC_HEADER_TIME
  AC_STRUCT_TM

dnl ---  eh... i dunno... -PH  ----
if test x$with_gameblender = x"yes"; then
  use_gameblender=1
  CFLAGS="$CFLAGS -DGAMEBLENDER=1"
else
  use_gameblender=0
fi
  AC_DEFINE_UNQUOTED([USE_GAMEBLENDER],[$use_gameblender],[Compile in GameBlender support])

if test x$enable_openal = x"yes"; then
  AC_DEFINE_UNQUOTED([USE_OPENAL],[$use_openal],[Use OpenAL sound API in GameBlender])
fi

AM_CONDITIONAL([EXPPYTHON],[test x$enable_exppython = x"yes"])

dnl -----------------------------------------
dnl Parse Makefile.in's and other input files
dnl -----------------------------------------

#list content automatically generated by a shell script
AC_CONFIG_FILES([Makefile
		intern/string/Makefile
		intern/ghost/Makefile
		intern/guardedalloc/Makefile
		intern/bmfont/Makefile
		intern/moto/Makefile
		intern/container/Makefile
		intern/memutil/Makefile
		intern/decimation/Makefile
		intern/iksolver/Makefile
		intern/keymaker/Makefile
		intern/bsp/Makefile
])

if test x$enable_exppython = x"no"; then
  AC_CONFIG_FILES([intern/python/Makefile
		intern/python/blendermodule/Makefile
		intern/python/freeze/Makefile
		intern/python/modules/Makefile
  ])
fi

AC_CONFIG_FILES([intern/Makefile
		source/blender/src/Makefile
		source/blender/avi/Makefile
		source/blender/blenkernel/Makefile
		source/blender/blenlib/Makefile
		source/blender/blenloader/Makefile
		source/blender/blenpluginapi/Makefile
])

if test x$enable_exppython = x"yes"; then
  AC_CONFIG_FILES([source/blender/python/Makefile
		source/blender/python/api2_2x/Makefile
  ])
else
  AC_CONFIG_FILES([source/blender/bpython/Makefile
  ])
fi

AC_CONFIG_FILES([source/blender/decrypt/Makefile
		source/blender/deflate/Makefile
		source/blender/encrypt/Makefile
		source/blender/imbuf/Makefile
		source/blender/img/Makefile
		source/blender/inflate/Makefile
		source/blender/makesdna/Makefile
		source/blender/makesdna/intern/Makefile
		source/blender/radiosity/Makefile
		source/blender/readblenfile/Makefile
		source/blender/readstreamglue/Makefile
		source/blender/render/Makefile
		source/blender/renderconverter/Makefile
		source/blender/sign/Makefile
		source/blender/verify/Makefile
		source/blender/writeblenfile/Makefile
		source/blender/writestreamglue/Makefile
		source/blender/Makefile
		source/creator/Makefile
		source/kernel/Makefile
		source/gameengine/BlenderRoutines/Makefile
		source/gameengine/Converter/Makefile
		source/gameengine/Expressions/Makefile
		source/gameengine/GameLogic/Makefile
		source/gameengine/GamePlayer/Makefile
		source/gameengine/Ketsji/Makefile
		source/gameengine/Network/Makefile
		source/gameengine/Physics/Makefile
		source/gameengine/Rasterizer/Makefile
		source/gameengine/SceneGraph/Makefile
		source/gameengine/SoundSystem/Makefile
		source/gameengine/Makefile
		source/icons/Makefile
		source/Makefile
])
AC_OUTPUT
