# $Id: CMakeLists.txt 23630 2009-10-05 07:08:59Z campbellbarton $
# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# The Original Code is Copyright (C) 2006, Blender Foundation
# All rights reserved.
#
# The Original Code is: all of this file.
#
# Contributor(s): Jacques Beaurai, Erwin Coumans
#
# ***** END GPL LICENSE BLOCK *****

SET(INC . common common/newsparse eltopo3d common/tunicate common/meshes)

FILE(GLOB SRC
	common/*.cpp
	common/newsparse/*.cpp
	common/tunicate/*.cpp
	common/sparse/*.cpp
	eltopo3d/*.cpp
	./*.cpp
)

ADD_DEFINITIONS(-DNOGUI)
ADD_DEFINITIONS(-DUSE_FORTRAN_BLAS)

blender_add_lib(extern_eltopo "${SRC}" "${INC}")

