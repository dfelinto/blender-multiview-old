#!/bin/sh
# This is an example call to configure for 
# a SunOS5.8 box.
#
setenv LDFLAGS "-L/soft/gcc-3.2/SunOS5.8/lib -R/soft/gcc-3.2/SunOS5.8/lib"
../blender/configure --prefix=/scratch/irulan/mein/blenderbin \
	--exec-prefix=/scratch/irulan/mein/blenderbin/SunOS5.8 \
        --with-ode=/usr/local \
	--with-freetype2=/usr/local \
	--enable-gameplayer
