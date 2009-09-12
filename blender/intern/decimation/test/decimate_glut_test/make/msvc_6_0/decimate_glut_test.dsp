# Microsoft Developer Studio Project File - Name="decimate_glut_test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=decimate_glut_test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "decimate_glut_test.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "decimate_glut_test.mak" CFG="decimate_glut_test - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "decimate_glut_test - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "decimate_glut_test - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "decimate_glut_test - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /Ob2 /I "..\..\..\..\..\..\lib\windows\decimation\include\\" /I "..\..\..\..\..\..\lib\windows\memutil\include\\" /I "..\..\..\..\..\..\lib\windows\ghost\include" /I "..\..\..\..\..\..\lib\windows\moto\include\\" /I "../../../../../../lib/windows/string/include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x413 /d "NDEBUG"
# ADD RSC /l 0x413 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 libdecimation.a libmoto.a libghost.a libstring.a glu32.lib opengl32.lib user32.lib gdi32.lib /nologo /subsystem:console /machine:I386 /nodefaultlib:"LIBC.lib" /libpath:"..\..\..\..\..\..\lib\windows\decimation\lib" /libpath:"..\..\..\..\..\..\lib\windows\moto\lib" /libpath:"..\..\..\..\..\..\lib\windows\string\lib" /libpath:"..\..\..\..\..\..\lib\windows\ghost\lib"

!ELSEIF  "$(CFG)" == "decimate_glut_test - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\..\..\lib\windows\decimation\include\\" /I "..\..\..\..\..\..\lib\windows\memutil\include\\" /I "..\..\..\..\..\..\lib\windows\ghost\include" /I "..\..\..\..\..\..\lib\windows\moto\include\\" /I "../../../../../../lib/windows/string/include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x413 /d "_DEBUG"
# ADD RSC /l 0x413 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libdecimation.a libmoto.a libghost.a libstring.a glu32.lib opengl32.lib user32.lib gdi32.lib /nologo /subsystem:console /map /debug /machine:I386 /nodefaultlib:"LIBC.lib" /pdbtype:sept /libpath:"..\..\..\..\..\..\lib\windows\decimation\lib" /libpath:"..\..\..\..\..\..\lib\windows\moto\lib" /libpath:"..\..\..\..\..\..\lib\windows\string\lib" /libpath:"..\..\..\..\..\..\lib\windows\ghost\lib"

!ENDIF 

# Begin Target

# Name "decimate_glut_test - Win32 Release"
# Name "decimate_glut_test - Win32 Debug"
# Begin Group "intern"

# PROP Default_Filter ""
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\intern\common\GlutDrawer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\intern\common\GlutDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\intern\common\GlutKeyboardManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\intern\common\GlutKeyboardManager.h
# End Source File
# Begin Source File

SOURCE=..\..\intern\common\GlutMouseManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\intern\common\GlutMouseManager.h
# End Source File
# End Group
# Begin Group "ply"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\intern\ply\ply.h
# End Source File
# Begin Source File

SOURCE=..\..\intern\ply\plyfile.c

!IF  "$(CFG)" == "decimate_glut_test - Win32 Release"

# ADD CPP /W4

!ELSEIF  "$(CFG)" == "decimate_glut_test - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\intern\GlutMeshDrawer.h
# End Source File
# Begin Source File

SOURCE=..\..\intern\LOD_GhostTestApp.cpp
# End Source File
# Begin Source File

SOURCE=..\..\intern\LOD_GhostTestApp.h
# End Source File
# Begin Source File

SOURCE=..\..\intern\main.cpp

!IF  "$(CFG)" == "decimate_glut_test - Win32 Release"

# ADD CPP /W3

!ELSEIF  "$(CFG)" == "decimate_glut_test - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\intern\MyGlutKeyHandler.h
# End Source File
# Begin Source File

SOURCE=..\..\intern\MyGlutMouseHandler.h
# End Source File
# End Group
# End Target
# End Project
