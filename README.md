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
(see youtube recording: http://www.youtube.com/watch?v=X7I6G3uRPkw&hd=1 )
* Multiview is rendering fine and antialias is working too.
* Support for MultiPart images (read and write) working fine (OpenEXR 2.0)
* Cycles is working as well
* Composite is working with small issues (see below)
* ImageNode is working really nicely btw

The rules for testing are:
* Build without Blenderplayer
* (for saving) EXR MultiLayer (multipart working)

How to save the render file?:
* For non-composite:
Set output type to exr multilayer, and render from command line, or save from the Image Editor (F3).
The Multi part check box is optional and should affect whether the output is multipart (2.0) or multiview (1.7.1). Both support stereo.
* For Composite:
Set output file to exr multilayer.

Known bugs:
* (see the issues in the github tracker)
* Missing listeners - not everything is updating when they should if you change things after rendering

Compositor elements not yet tackled:
* ~~Image Node (right now it shows each view as a socket)~~
* ~~Image Node: to map image views to scene views~~
* FSA
* Viewer Node
* SingleLayer exr file
* Non-exr files (more a design/UI thing)
* Global option to composite/render only one view
* View Filter Node: to run a nodebranch when view =="left" ... [need design]

What do I plan to work next?
--------------------------------------

**Viewport Support**:
We need a per viewport enable/disable option.
On top of that, we need option to choice the mode:
* *Scene views*: to pick the left/right cameras from the scene views.
* *Custom view*: to define a new eye separation and convergence distance
* *Dynamic view*: to choose pivot point, cursor, an object or infinity as convergence depth zero point.

*Scene view* is to explore the look you would get from your render,
*Custom view* to help you determine a new good depth,
*Dynamic view* to work in 3d (sculpting, posing, ...) without a direct relation with the render settings.
I believe Custom view and Dynamic can be merged into the same option.

**Built-in Stereo Camera**:
If you read the original code proposal you should remember the original idea of having a builtin stereo-camera.
I didn't drop this idea, but there will be changes.

We will have a new stereo camera with all the nice properties (editable in 3D!).
Then you just assign this camera twice to two views in the Render View panel. The view panel can see the special camera and
give a menu where you can set left or right.

OR we will have addons for that ;)


Current issues
--------------------------
* Viewer Node showing only one view (waiting to hear back from JB on that).
Need to decide (design-wise) how to handle files output.

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
 5. Compo in multiview
 6. ~~See multiview in UV/image editor as stereo~~
 7. Viewport preview
 8. Sequencer
 9. ?

How to build it
---------------
UPDATE: you know need OpenEXR 2.0 to use this branch.
(so you need to recompile openexr and openimageio and openshadinglanguage [or disable OSL])
UPDATE2: Blender already has exr2.0 libs for OSX, windows following shortly, linux will be the last one.

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
