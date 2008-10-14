To build the Active X game player use either the MT DLL Debug or MT DLL Release targets.
This makes sure that the gameplayer source is compiled with the correct run time library.

After building, copy the python 1.5 DLL (for the debug target or the python 2.0 DLL
(for the release target) into the output directory of the project.
Register the Active X control with the system. Use the MSVC command
(Tools->Register Control) or use RegSvr32.exe directly.

To test the control, use the Active X Test Container from the MSVC Tools menu.
Alternatively, use Internet Explorer with the test HTML file BlenderPlayer.html.
A setting is included in the project files that works for my system but you need
to change it for yours.

DLLs used on top of IE in debug mode:
'C:\WINNT\system32\opengl32.dll'
'C:\WINNT\system32\glu32.dll'
'C:\WINNT\system32\ddraw.dll'
'C:\WINNT\system32\dciman32.dll'
'C:\WINNT\system32\MFC42D.DLL'
'C:\WINNT\system32\MSVCRTD.DLL'
'C:\WINNT\system32\MFCO42D.DLL'
'C:\WINNT\system32\MSVCP60D.DLL'
