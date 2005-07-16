# Microsoft Developer Studio Project File - Name="OpenGL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OpenGL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OpenGL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OpenGL.mak" CFG="OpenGL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OpenGL - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OpenGL - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OpenGL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OpenGL___Win32_Release"
# PROP BASE Intermediate_Dir "OpenGL___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "OpenGL___Win32_Release"
# PROP Intermediate_Dir "OpenGL___Win32_Release"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\Glut" /I "..\..\Linearmath" /I "..\..\Bullet" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "OpenGL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "OpenGL___Win32_Debug"
# PROP BASE Intermediate_Dir "OpenGL___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "OpenGL___Win32_Debug"
# PROP Intermediate_Dir "OpenGL___Win32_Debug"
# PROP Target_Dir ""
MTL=midl.exe
LINK32=link.exe -lib
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\Glut" /I "..\..\Linearmath" /I "..\..\Bullet" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ  /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "OpenGL - Win32 Release"
# Name "OpenGL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\BMF_Api.cpp
# End Source File
# Begin Source File

SOURCE=.\BMF_BitmapFont.cpp
# End Source File
# Begin Source File

SOURCE=.\BMF_font_helv10.cpp
# End Source File
# Begin Source File

SOURCE=.\GL_ShapeDrawer.cpp
# End Source File
# Begin Source File

SOURCE=.\GL_Simplex1to4.cpp
# End Source File
# Begin Source File

SOURCE=.\GlutStuff.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderTexture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\BMF_Api.h
# End Source File
# Begin Source File

SOURCE=.\BMF_BitmapFont.h
# End Source File
# Begin Source File

SOURCE=.\BMF_FontData.h
# End Source File
# Begin Source File

SOURCE=.\BMF_Fonts.h
# End Source File
# Begin Source File

SOURCE=.\BMF_Settings.h
# End Source File
# Begin Source File

SOURCE=.\DebugCastResult.h
# End Source File
# Begin Source File

SOURCE=.\GL_ShapeDrawer.h
# End Source File
# Begin Source File

SOURCE=.\GL_Simplex1to4.h
# End Source File
# Begin Source File

SOURCE=.\GlutStuff.h
# End Source File
# Begin Source File

SOURCE=.\RenderTexture.h
# End Source File
# End Group
# End Target
# End Project
