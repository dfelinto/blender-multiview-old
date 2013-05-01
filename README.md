Multi View
==========
clone from https://gitorious.org/blenderprojects/blender

![Alt text](http://wiki.blender.org/uploads/0/0d/Dev-Stereoscopy-MirroredSample.png "BMW model by Mike Pan")

This branch contains the code for the "stereoscopy support in Blender" journey.

Development
-----------
Current Panel:

<img src="http://dalaifelinto.com/ftp/multiview/multiview_panel.jpg" alt="" width="235.5px" height="247.5px"/>

Current status:
* Multiview is rendering fine and antialias is working too.
* Support for MultiPart images (read and write) working fine (OpenEXR 2.0)
* Cycles is working as well
* My next target is Composite

The rules for testing are:
* No Compo
* ~~No FSA~~
* (for saving) EXR MultiLayer (multipart working)

Known bugs:
* When rendering only one view, the 'view' name shouldn't be saved in EXR file (it's probably fixed now, I'll investigate later
* UV/Image Editor has the "Combined" pass even when it shouldn't (I hacked it away, but not nicely)

Current ~~headache~~issues
--------------------------
None at the moment. OpenEXR 2.0 was hard to implement, but it's all working now.
Composite will be "fun" ;)

Links
-----
**Original proposal:** http://wiki.blender.org/index.php/User:Dfelinto/Stereoscopy

**Mailing list discussion:** http://lists.blender.org/pipermail/bf-committers/2013-March/039601.html

Roadmap
-------
 1. ~~Read multiview exr~~
 2. ~~See multiview in UV/image editor as mono~~
 3. ~~Write multiview exr~~
 4. ~~Render in multiview~~
 5. Compo in multiview
 6. See multiview in UV/image editor as stereo
 7. Viewport preview
 8. ?

How to build it
---------------
UPDATE: you know need OpenEXR 2.0 to use this branch.
(so you need to recompile openexr and openimageio and possibly openshadinglanguage)

Rough guide, basically you need to manually copy the addons and addons_contrib folders inside the checkout blender code.

For tips in building Blender for your system refer to to Blender Wiki.

Following instructions are for OSX. Don't use them literally, try to make sense of them first.

 1. $git clone https://github.com/dfelinto/blender.git --single-branch -b multiview blender
 2. $svn checkout https://svn.blender.org/svnroot/bf-extensions/trunk/py/scripts/addons addons
 3. $svn checkout https://svn.blender.org/svnroot/bf-extensions/contrib/py/scripts/addons addons_contrib
 4. $rsync -rv --exclude=.svn addons blender/release/scripts/
 5. $rsync -rv --exclude=.svn addons_contrib blender/release/scripts/
 6. $ln -s ~/blender/lib lib; #HARDCODED folder to match my system, good luck
 7. mkdir release
 8. cd release
 9. ccmake ../blender
 10. make -j7 install
