version: ActiveX002
date:    August 1, 2001
author:  Maarten Gribnau

This document accompanies the alpha version of the Blender Player Active X control.

Bug reports, remarks, etc. go to:
maarten@blender.nl

The Blender Player Active X control can only be used with Internet Explorer on Windows.
Click on the plug-in to give it keyboards focus.

Installation instructions:
1. Unzip the archive BlenderPlayerActiveX.zip.
2. Run register_WIN2K.bat or register_WIN98_WINME.bat (depending on the Windows version you use) to register
   the control.
3. If there are no errors, double click BlenderPlayer.html to start the plugin.

New features:
1. Full keyboard support (except some keys IE needs such as Alt keys and Tab)
2. Mouse support.
3. Network support.

Known issues:
1. Plug-in gives no indication that a file is not found for loading.
2. After a sound is played, Internet Explorer will crash when closed.
3. Loading animation is not finished (temporary version is included).
4. Plug-in sometimes crashes on first download (restarting it works).
5. Plug-in is slower than the stand-alone player.
6. Camera settings should obey render frame settings in Blender.
