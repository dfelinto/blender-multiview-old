#!/bin/sh
# This is an example call to configure for 
# a OSX box.
#
../blender/configure --prefix=/Users/mein/blenderbin \
        --with-ode=/usr/local \
        --with-libjpeg=/sw \
        --enable-quicktime=yes \
        --enable-gameplayer
