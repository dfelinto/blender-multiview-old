dnl This file should be processed with GNU Autoconf to produce a "configure"
dnl script, with which a user can:
dnl - check for build requirements/options (headers, libraries, functions,
dnl   data types);
dnl - handle user definable build options (compiler flags, extra features);
dnl - parse the ".in" files into standard files (Makefile's, manpages, etc.).

dnl --------------------
dnl AC/AM Initialization
dnl --------------------

AC_PREREQ(2.12)
AC_INIT(blender, 2.29, )
AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([$PACKAGE_NAME], [$PACKAGE_VERSION])
AC_DISABLE_SHARED

dnl check for the platform make
dnl AC_PATH_PROGS(U_MAKE, gmake gnumake, make)
dnl AC_SUBST(U_MAKE)

dnl Make sure we use GNU make. On win32, we need to have make. */
case "$target" in
  *-cygwin*|*-mingw*|*-msvc*|*-mks*)
    AC_PATH_PROGS(MAKE, $MAKE make gmake, :)
    ;;
  *)
    AC_PATH_PROGS(MAKE, $MAKE gmake make, :)
    ;;
esac

dnl --------------------------------------------------------------------------
dnl The following lines allow for the following mechanism on the command line:
dnl
dnl CFLAGS="-foo" ./configure
dnl --------------------------------------------------------------------------
CFLAGS="${CFLAGS=}"
CPPFLAGS="${CPPFLAGS=}"
CXXFLAGS="${CXXFLAGS=}"
LDFLAGS="${LDFLAGS=}"

dnl --------------------------------------------------------------------------
dnl Default to MSVC for win32
dnl --------------------------------------------------------------------------
case "$target" in
  *-cygwin*|*-mingw*|*-msvc*|*-mks*)
    if test -z "$CC";  then CC=cl;   fi
    if test -z "$CXX"; then CXX=cl;  fi
    if test -z "$CPP"; then CPP=cl;  fi
    if test -z "$LD";  then LD=link; fi
    if test -z "$AS";  then AS=ml;   fi
    ;;
esac

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
  *-cygwin*|*-mingw*|*-msvc*|*-mks* )
     if test "x$GCC" = xyes; then
        dnl Check if we are really running in a msvc environment
        CFLAGS="$CFLAGS -TC -nologo"
        CXXFLAGS="$CXXFLAGS -TP -nologo"
        AC_LANG_SAVE
        AC_LANG_C
        AC_TRY_COMPILE([#include <stdio.h>],
            [ printf("Hello World\n"); ],,
            AC_MSG_ERROR([\$(CC) test failed.  You must have MS VC++ in your path to build.]) )

        AC_LANG_CPLUSPLUS
        AC_TRY_COMPILE([#include <iostream.h>],
            [ cout << "Hello World" << endl; ],,
            AC_MSG_ERROR([\$(CXX) test failed.  You must have MS VC++ in your path to build.]) )
        AC_LANG_RESTORE

        dnl Determine compiler version
        CC_VERSION=`"${CC}" -v 2>&1 | grep Version | sed -e 's|.* Version ||' | sed -e 's|[ ]*$||'`
        _CC_MAJOR_VERSION=`echo ${CC_VERSION} | $AWK -F\. '{ print $ 1 }'`

        CXX_VERSION=`"${CXX}" -v 2>&1 | grep Version | sed -e 's|.* Version ||' | sed -e 's|[ ]*$||'`
        _CXX_MAJOR_VERSION=`echo ${CXX_VERSION} | $AWK -F\. '{ print $ 1 }'`

        if test "$_CC_MAJOR_VERSION" != "$_CXX_MAJOR_VERSION"; then
            AC_MSG_ERROR([The major versions of \$CC and \$CXX do not match.])
        fi
        if test "$_CC_MAJOR_VERSION" = "12"; then
            _CC_SUITE=6
        elif test "$_CC_MAJOR_VERSION" = "13"; then
            _CC_SUITE=7
        else
            AC_MSG_ERROR([This version of the MSVC compiler, $CC_VERSION , is unsupported.])
        fi

        dnl Check linker version
        _LD_FULL_VERSION=`"${LD}" -v 2>&1 | grep Version | sed -e 's|.* Version ||' | sed -e 's|[ ]*$||'`
        _LD_MAJOR_VERSION=`echo ${_LD_FULL_VERSION} | $AWK -F\. '{ print $ 1 }'`
        if test "$_LD_MAJOR_VERSION" != "$_CC_SUITE"; then
            AC_MSG_ERROR([The linker major version, $_LD_FULL_VERSION,  does not match the compiler suite version, $_CC_SUITE.])
        fi

        dnl Set some useful C/C++ flags
        CFLAGS="$CFLAGS /MT -W3 -nologo -D_M_IX86 -DWIN32 -D_WIN32 -D__WIN32"
        CXXFLAGS="$CXXFLAGS /MT -W3 -nologo -D_M_IX86 -DWIN32 -D_WIN32 -D__WIN32"
     fi
     dnl AC_DEFINE(WIN32)
     dnl AC_DEFINE(_WIN32)
  ;;
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
     LDFLAGS="$LDFLAGS -L/usr/freeware/lib32"
     if test "x$GCC" = xyes; then
        CXXFLAGS="$CXXFLAGS -DNO_FAST_ATOMS"
     else
         CFLAGS="$CFLAGS -n32 -mips3 -Xcpluscomm"
         CXXFLAGS="$CXXFLAGS -n32 -mips3 -Xcpluscomm -LANG:std"
     fi

  ;;
  *-*-darwin* )
dnl  	AC_CHECK_HEADER([/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/gl.h]) 
dnl  	AC_CHECK_HEADER([/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers/glu.h])
     AC_PATH_PROGS(PBBUILD, pbbuild pbxbuild)

	 SYSTEM_LIBS="$SYSTEM_LIBS -framework OpenGL -framework AGL"
     CPPFLAGS="$CPPFLAGS -D_THREAD_SAFE"
     STRIP="$STRIP -x -S"
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

  AC_ARG_ENABLE(quicktime,
   AC_HELP_STRING([--enable-quicktime],[Build with support for QuickTime (default=disable)]),
   ,dnl default assign.
   [ enable_quicktime=no ]  dnl default setting.
  )

  AC_ARG_ENABLE(international,
   AC_HELP_STRING([--enable-international],[Build with international language support (default=disable)]),
   ,dnl default assign.
   [ enable_international=no ]  dnl default setting.
  )

  AC_ARG_ENABLE(ghostdebug,
   AC_HELP_STRING([--enable-ghostdebug],[enable GHOST debugging (default=no)]),
   ,dnl default assing.
   [ enable_ghostdebug=no ]
  )

  AC_ARG_ENABLE(precompiled,
    AC_HELP_STRING([--enable-precompiled],[enables linking against the precompiled libraries (default=no)]),
    ,dnl default assign.
    [ enable_precompiled=no ]  dnl default setting.
  )

dnl --------------------------------------------------------------------------
dnl Check for optional library path defines.
dnl Are these valid when the option --enable-precompiled has been used?
dnl --------------------------------------------------------------------------

  AC_ARG_WITH(ode,
    AC_HELP_STRING([--with-ode[=PFX]],[Path to ode files]),
[       if test $withval != "yes"; then
          if test -f $withval/include/ode/ode.h; then
            LDFLAGS="$LDFLAGS -L$withval/lib"
            CPPFLAGS="$CPPFLAGS -I$withval/include"
          fi
        fi
])

  AC_ARG_WITH(sdl,
    AC_HELP_STRING([--with-sdl[=PFX]],[Path to SDL library]),
[       if test $withval != "yes"; then
          if test -f $withval/include/SDL/SDL.h; then
              LDFLAGS="$LDFLAGS -L$withval/lib"
              CPPFLAGS="$CPPFLAGS -I$withval/include/SDL"
          fi
        fi
])

  AC_ARG_WITH(freetype2,
    AC_HELP_STRING([--with-freetype2[=PFX]],[Path to freetype2 library]),
[       if test $withval != "yes"; then
          if test -f $withval/include/freetype2/freetype/freetype.h; then
            LDFLAGS="$LDFLAGS -L$withval/lib -lfreetype"
            CPPFLAGS="$CPPFLAGS -I$withval/include/freetype2 -DWITH_FREETYPE2"
            CFLAGS="$CFLAGS -I$withval/include/freetype2 -DWITH_FREETYPE2"
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
           CPPFLAGS="$CPPFLAGS -I$withval/include/mozilla-$MOZVER/nspr"
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

dnl --------------------------------------------------------------------------
dnl Ghost debugging options
dnl --------------------------------------------------------------------------
if test "x$enable_ghostdebug" = xyes; then
  if test "x$GCC" = xyes; then
    CFLAGS="$CFLAGS -O0 -DGHOST_DEBUG"
    CXXFLAGS="$CXXFLAGS -O0 -DGHOST_DEBUG"
  else
    AC_MSG_WARN([compiler unknown, please disable optimizations manually in \
your CFLAGS])
  fi
fi


dnl --------------------------------------------------------------------------
dnl Python!
dnl --------------------------------------------------------------------------
  AM_PATH_PYTHON(2.0)
  AC_CACHE_CHECK([Python link args...],
    [am_cv_python_ldflags],
    [am_cv_python_ldflags=`$PYTHON -c "import distutils.sysconfig; print distutils.sysconfig.get_config_var('LINKFORSHARED')"`])

 AC_SUBST(PYTHON_LDFLAGS, [$am_cv_python_ldflags])

#Find out what path to use for python.
python_sysprefix=`echo "import sys; print sys.prefix" | $PYTHON -`

if test x"$enable_openal" = x"yes"; then
  XIPH_PATH_OGG
  XIPH_PATH_VORBIS
fi

dnl --------------------------------------------------------------------------
dnl Check if compiler implements namespaces. This is needed for the STL
dnl macro. This macro can be found on
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_namespaces.html
dnl --------------------------------------------------------------------------
AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether the compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
fi
])

dnl --------------------------------------------------------------------------
dnl Check for a working STL implementation. This macro can be found on
dnl http://www.gnu.org/software/ac-archive/htmldoc/ac_cxx_have_stl.html
dnl --------------------------------------------------------------------------
AC_DEFUN([AC_CXX_HAVE_STL],
[AC_CACHE_CHECK(whether the compiler supports Standard Template Library,
ac_cv_cxx_have_stl,
[AC_REQUIRE([AC_CXX_NAMESPACES])
 AC_LANG_SAVE
 AC_LANG_CPLUSPLUS
 AC_TRY_COMPILE([#include <list>
#include <deque>
#ifdef HAVE_NAMESPACES
using namespace std;
#endif],[list<int> x; x.push_back(5);
list<int>::iterator iter = x.begin(); if (iter != x.end()) ++iter; return 0;],
 ac_cv_cxx_have_stl=yes, ac_cv_cxx_have_stl=no)
 AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_stl" = yes; then
  AC_DEFINE(HAVE_STL,,[define if the compiler supports Standard Template Library])
fi
])

AC_CXX_HAVE_STL

dnl -----------------------
dnl Checks for header files
dnl -----------------------

  AC_CONFIG_SRCDIR([source/creator/creator.c])
  AM_CONFIG_HEADER([config.h])

  AC_HEADER_STDC
  AC_CHECK_HEADERS([fcntl.h limits.h stdarg.h math.h])
  AC_HEADER_TIME
  AC_STRUCT_TM
  AC_CHECK_HEADER([jpeglib.h])
  AC_CHECK_HEADER([png.h])
  AC_CHECK_HEADERS([malloc.h stddef.h termios.h wchar.h wctype.h])
  AC_CHECK_HEADERS([sys/file.h sys/ioctl.h sys/mount.h sys/param.h sys/statfs.h sys/time.h sys/vfs.h sys/socket.h])
  AC_CHECK_HEADERS([netinet/in.h])
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

  AC_SUBST([PYTHONSYSPREFIX],[$python_sysprefix])
#  AC_SUBST([BlenderSTATIC],[$enable_blenderstatic])
  AM_CONDITIONAL([BlenderSTATIC],[test x$enable_blenderstatic = x"yes"])
  AM_CONDITIONAL([BlenderGAME],[test x$enable_gameblender = x"yes"])
  AM_CONDITIONAL([BlenderOPENAL],[test x$enable_openal = x"yes"])
  AM_CONDITIONAL([BlenderPLUGIN],[test x$enable_blenderplugin = x"yes"])
  AM_CONDITIONAL([BlenderFMOD],[test x$enable_fmod = x"yes"])
  AM_CONDITIONAL([BlenderQUICKTIME],[test x$enable_quicktime = x"yes"])

dnl --------------------------
dnl Checks for POSIX functions
dnl --------------------------

dnl  AC_FUNC_ERROR_AT_LINE
dnl  AC_FUNC_LSTAT
dnl  AC_FUNC_OBSTACK
dnl  complaints about not on some systems.  Not sure if its needed or not
dnl  mein@cs.umn.edu
dnl  AC_FUNC_REALLOC
dnl  AC_FUNC_ALLOCA
dnl  AC_FUNC_FNMATCH
dnl  AC_FUNC_FORK
dnl  AC_FUNC_MALLOC
dnl  AC_FUNC_MEMCMP
dnl  AC_FUNC_MMAP
dnl  AC_TYPE_SIGNAL
dnl  AC_FUNC_STAT
dnl  AC_FUNC_STRFTIME
dnl  AC_FUNC_VPRINTF
dnl  AC_CHECK_FUNCS([endpwent floor getcwd gettimeofday memmove memset mkdir munmap pow select sqrt strcasecmp strchr strdup strncasecmp strpbrk strrchr strstr strtoul atexit fdatasync gethostname])

dnl --------------------------------------------------------------------------
dnl Check for necessary libraries.
dnl Note that this section still needs some work to figure out the best order
dnl of checking. TODO!!!
dnl --------------------------------------------------------------------------
  AC_CHECK_LIBM  dnl Use libtool.m4's provided macro to get the math library 
  AC_SUBST(LIBM)
  AC_CHECK_LIB([z], [gzopen])
  AC_CHECK_LIB([pthread], [pthread_exit])

dnl --------------------------------------------------------------------------
dnl Set default library paths when using the precompiled libraries.
dnl If not available, try to find the one on the system.
dnl --------------------------------------------------------------------------
if test x"$enable_precompiled" = x"yes"; then
  case "$target" in
    *-cygwin*|*-mingw*|*-msvc*|*-mks* )
    ;;
    *sparc* )
    ;;  
    *-*-powerpc )
    ;;
    *-*-irix* )
    ;;
    *-darwin* )
    ;;
    *-*-openbsd* )
    ;;
    *-*-beos* )
    ;;
    *-linux*)
      BLIB_PNG="lib/linux-glibc2.2.5-i386/png/lib/libpng.a"
      BLIB_JPEG="lib/linux-glibc2.2.5-i386/jpeg/lib/libjpeg.a"
      BLIB_SDL=""
      AM_CONDITIONAL(b_png, true)
      AM_CONDITIONAL(b_jpeg, true)
      AM_CONDITIONAL(b_sdl, false)
      AM_PATH_SDL(1.0.0)
      AC_CHECK_LIB([SDL], [SDL_Init])
      LIBS="$LIBS $SDL_LIBS"
    ;;
  esac
  AC_SUBST(BLIB_PNG)
  AC_SUBST(BLIB_JPEG)
  AC_SUBST(BLIB_SDL)
else
  AM_CONDITIONAL(b_png, false)
  AM_CONDITIONAL(b_jpeg, false)
  AM_CONDITIONAL(b_sdl, false)
  AC_CHECK_LIB([png], [png_get_header_version])
  AC_CHECK_LIB([jpeg], [jpeg_destroy])
  AM_PATH_SDL(1.0.0)
  AC_CHECK_LIB([SDL], [SDL_Init])
  LIBS="$LIBS $SDL_LIBS"
fi

dnl --------------------------
dnl Checks for extra libraries
dnl --------------------------

  AC_LANG_PUSH([C++])
  AC_CHECK_LIB([stdc++], [printf]) dnl  Zarquon's knees, this is an evil hack.
  AC_LANG_POP([C++])
  AC_CHECK_LIB([dl], [dlopen])
  AC_CHECK_LIB([util], [openpty])
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

if test x$ac_cv_header_Carbon_Carbon_h != xyes; then
  AC_CHECK_LIB([Xext], [XextFindDisplay],,,$X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
  AC_CHECK_LIB([X11], [XOpenDisplay],,,$X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
  AC_CHECK_LIB([GL], [glIsTexture],,
    AC_MSG_ERROR(OpenGL not found on your system - cannot continue),
        $X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
  AC_CHECK_LIB([GLU], [gluGetString],,
    AC_MSG_ERROR(OpenGL not found on your system - cannot continue),
        $X_LIBS -lXext -lX11 -lm $X_EXTRA_LIBS)
else
  if test x"$enable_quicktime" = x"yes"; then
    CPPFLAGS="$CPPFLAGS -DWITH_QUICKTIME"
  fi
fi

dnl ---------------------
dnl Checks for data types
dnl ---------------------

  AC_C_CONST
  AC_C_INLINE
  AC_TYPE_MODE_T
  AC_TYPE_SIZE_T

dnl ---  eh... i dunno... -PH  ----
if test x$with_gameblender = x"yes"; then
  use_gameblender=1
  CFLAGS="$CFLAGS -DGAMEBLENDER=1"
else
  use_gameblender=0
fi
  AC_DEFINE_UNQUOTED([USE_GAMEBLENDER],[$use_gameblender],[Compile in GameBlender support])

dnl if test x$enable_openal = x"yes"; then
dnl   AC_DEFINE_UNQUOTED([USE_OPENAL],[$use_openal],[Use OpenAL sound API in GameBlender])
dnl fi

AM_CONDITIONAL([USE_OPENAL],[test x$use_openal = x"yes"])

if test "x$enable_international" = xyes; then
  case "$target" in
    *-cygwin*|*-mingw*|*-msvc*|*-mks* )
    ;;
    *sparc* )
    ;;  
    *-*-powerpc )
    ;;
    *-*-irix* )
    ;;
    *-darwin* )
    ;;
    *-*-openbsd* )
    ;;
    *-*-beos* )
    ;;
    *-linux*)
    ;;
  esac
fi

dnl -----------------------------------------
dnl Parse Makefile.in's and other input files
dnl -----------------------------------------

#list content automatically generated by a shell script
AC_CONFIG_FILES([Makefile
		intern/SoundSystem/Makefile
		intern/string/Makefile
		intern/ghost/Makefile
		intern/guardedalloc/Makefile
		intern/bmfont/Makefile
		intern/moto/Makefile
		intern/container/Makefile
		intern/memutil/Makefile
		intern/decimation/Makefile
		intern/iksolver/Makefile
		intern/bsp/Makefile
		intern/Makefile
		source/blender/src/Makefile
		source/blender/avi/Makefile
		source/blender/blenkernel/Makefile
		source/blender/blenlib/Makefile
		source/blender/blenloader/Makefile
		source/blender/blenpluginapi/Makefile
		source/blender/python/Makefile
		source/blender/python/api2_2x/Makefile
		source/blender/deflate/Makefile
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
		source/blender/writeblenfile/Makefile
		source/blender/writestreamglue/Makefile
		source/blender/quicktime/Makefile
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
		source/gameengine/Makefile
		source/icons/Makefile
		source/Makefile
])
AC_OUTPUT
