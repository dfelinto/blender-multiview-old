# $Id$
# ***** BEGIN GPL/BL DUAL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version. The Blender
# Foundation also sells licenses for use in proprietary software under
# the Blender License.  See http://www.blender.org/BL/ for information
# about this.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# The Original Code is Copyright (C) 2006, Blender Foundation
# All rights reserved.
#
# The Original Code is: none
#
# Contributor(s): Enrico Fracasso.
#
# ***** END GPL/BL DUAL LICENSE BLOCK *****

MESSAGE(STATUS "Configuring WebPlugin")

# Enable build flag
STRING(TOUPPER ${WEBPLUGIN_SANDBOX_MODE} WEBPLUGIN_SANDBOX_MODE_UPPER)

IF (WEBPLUGIN_SANDBOX_MODE_UPPER STREQUAL "APPARMOR")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWITH_APPARMOR ")
ENDIF (WEBPLUGIN_SANDBOX_MODE_UPPER STREQUAL "APPARMOR")

IF (WEBPLUGIN_SANDBOX_MODE_UPPER STREQUAL "PRIVSEP")
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWITH_PRIVSEP ")
	ADD_EXECUTABLE(blenderplayer-wrapper blenderplayer-wrapper.c)
ENDIF (WEBPLUGIN_SANDBOX_MODE_UPPER STREQUAL "PRIVSEP")

# set output path
SET(LIBRARY_OUTPUT_PATH  ${CMAKE_BINARY_DIR}/webplugin)

SET(INC
	${GECKO_DIR}/include/
)

SET(SRC 
	npunix.c 
	UnixShell.c 
)

INCLUDE_DIRECTORIES(${INC})
ADD_LIBRARY(blender_plugin SHARED ${SRC} )


