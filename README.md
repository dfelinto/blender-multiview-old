Multi View
==========
clone from https://gitorious.org/blenderprojects/blender

![BMW model by Mike Pan](http://wiki.blender.org/uploads/0/0d/Dev-Stereoscopy-MirroredSample.png "BMW model by Mike Pan")

This branch contains the code for the "stereoscopy support in Blender" journey.

Beta Builds
-----------
* [OSX](http://graphicall.org/1047) by [Francesco Siddi](http://twitter.com/fsiddi)
* [Linux](http://graphicall.org/1048) by [Alexey Akishin](https://github.com/Alexey-Akishin)
* [Windows 1](http://graphicall.org/1045)
* [Windows 2](http://builder.blender.org/download/) by [Blender Foundation](http://www.blender.org) / [BuildBot](http://buildbot.net)

Implemented Features
-----------
Render Views panel:

<img src="http://dalaifelinto.com/ftp/multiview/multiview_panel.jpg" alt="" width="235.5px" height="247.5px"/>

Switch View node:

<img src="http://dalaifelinto.com/ftp/multiview/multiview_switchview.jpg" alt="" width="161.5px" height="161px"/>

Image node (views options):

<img src="http://dalaifelinto.com/ftp/multiview/multiview_imagenode.jpg" alt="" width="150px" height="262px"/>

3-D display options (User Preferences > System):

<img src="http://dalaifelinto.com/ftp/multiview/multiview_stereodisplay.jpg" alt="" width="225.5px" height="253.5px"/>

3-D preview in Image Editor

<img src="http://dalaifelinto.com/ftp/multiview/multiview_stereo_imageeditor.jpg"/>

Current Status:
---------------
* See [Multiview Addon](https://github.com/dfelinto/multiview_addon) for the camera rig system
* [Youtube recording](http://www.youtube.com/watch?v=X7I6G3uRPkw&hd=1)
* **[Instructions for testing](https://github.com/dfelinto/blender/blob/multiview/HowToTestIt.md)**

**Known bugs:**
* (see the issues in the github tracker)
* Missing listeners - not everything is updating when they should if you change things after rendering

**Compositor elements not yet tackled:**
* Viewer Node

What do I plan to work next?
--------------------------------------

**Built-in Stereo Camera**:
If you read the original code proposal you should remember the original idea of having a builtin stereo-camera.
I didn't drop this idea, but there will be changes.

We will have a new stereo camera with all the nice properties (editable in 3D!).
Then you just assign this camera twice to two views in the Render View panel. The view panel can see the special camera and
give a menu where you can set left or right.

OR we will have addons for that ;)
UPDATE: I'm testing an addon and I think it may work well with it. So no builtin stereo camera for now.


Current issues
--------------------------
* Viewer Node showing only one view
* Sequencer not implemented

Links
-----
**Original proposal:** http://wiki.blender.org/index.php/User:Dfelinto/Stereoscopy

**Mailing list discussion:**
http://lists.blender.org/pipermail/bf-committers/2013-March/039601.html

**3d formats:**
http://news.cnet.com/8301-17938_105-20063310-1/how-3d-content-works-blu-ray-vs-broadcast/

**Active vs Passive:**
http://reviews.cnet.com/8301-33199_7-57437344-221/active-3d-vs-passive-3d-whats-better/

Roadmap
-------
 1. ~~Read multiview exr~~
 2. ~~See multiview in UV/image editor as mono~~
 3. ~~Write multiview exr~~
 4. ~~Render in multiview~~
 5. ~~Compo in multiview~~
 6. ~~See multiview in UV/image editor as stereo~~
 7. ~~Viewport preview~~
 8. Sequencer
 9. ?

How to build it
---------------
UPDATE: you know need OpenEXR 2.0 to use this branch.
(so you need to recompile openexr and openimageio and openshadinglanguage [or disable OSL])
UPDATE2: Blender already has exr2.0 libs for OSX, windows following shortly, linux will be the last one.
UPDATE3: Blender svn OpenEXR for windows are buggy, they will only work if you build a debug Blender.

This code is mirrored (not often) into Blender svn branch system. If you have troubles building from git you can fallback to it:
https://svn.blender.org/svnroot/bf-blender/branches/multiview/


Rough guide, basically you need to manually copy the addons and addons_contrib folders inside the checkout blender code.

For tips in building Blender for your system refer to to Blender Wiki.

Following instructions are for OSX. Don't use them literally, try to make sense of them first.

 1. $git clone https://github.com/dfelinto/blender.git --single-branch -b multiview
 2. cd multiview
 3. $svn checkout https://svn.blender.org/svnroot/bf-extensions/trunk/py/scripts/addons addons
 4. $svn checkout https://svn.blender.org/svnroot/bf-extensions/contrib/py/scripts/addons addons_contrib
 5. $rsync -rv --exclude=.svn addons blender/release/scripts/
 6. $rsync -rv --exclude=.svn addons_contrib blender/release/scripts/
 7. $ln -s ~/blender/lib lib; #HARDCODED folder to match my system, good luck
 8. mkdir release
 9. cd release
 10. ccmake ../blender
 11. make -j7 install
