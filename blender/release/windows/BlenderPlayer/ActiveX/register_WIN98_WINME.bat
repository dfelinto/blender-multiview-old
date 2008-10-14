rem
rem $Id$
rem
rem ***** BEGIN GPL LICENSE BLOCK *****
rem
rem This program is free software; you can redistribute it and/or
rem modify it under the terms of the GNU General Public License
rem as published by the Free Software Foundation; either version 2
rem of the License, or (at your option) any later version.
rem
rem This program is distributed in the hope that it will be useful,
rem but WITHOUT ANY WARRANTY; without even the implied warranty of
rem MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
rem GNU General Public License for more details.
rem
rem You should have received a copy of the GNU General Public License
rem along with this program; if not, write to the Free Software Foundation,
rem Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
rem
rem The Original Code is Copyright (C) 2001-2002 by NaN Holding BV.
rem All rights reserved.
rem
rem The Original Code is: all of this file.
rem
rem Contributor(s): none yet.
rem
rem ***** END GPL LICENSE BLOCK *****
IF NOT EXIST C:\TEMP MKDIR C:\TEMP
COPY Blender3DPlugin.ocx C:\TEMP
COPY python20.dll C:\TEMP
C:
CD C:\TEMP
C:\WINDOWS\SYSTEM\REGSVR32 Blender3DPlugin.ocx