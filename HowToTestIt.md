Instructions
============

* If 3D viewing is not working, make sure to set: User Preferences > System > Windows Draw Method > *Triple Buffer*
* To test the 3D Display go to User Preferences > System and pick your 3D Display type
* To toggle stereo 3D support in a window press "D" (or WM_OT_stereo_toggle)
* Supported areas: Viewport and Image Editor

Testing Image Editor
====================
* Open one of the sample images in the Image Editor
* Turn 3D on with "D"
* Pick "3D" in the view menu (in the header of Image Editor)

Testing Viewport
================
* Install the [multiview addon](https://github.com/dfelinto/multiview_addon) to help setting up your cameras
* Create two cameras in your scene, corresponding to left and right
* In the (new) Render Views panel enable the "left" and "right" views and set their cameras
* Make sure you are in the camera mode (Numpad 0)
* Turn 3D on with "D"

Testing Rendering
=================
* Setup the scene as described in "Testing Viewport"
* Render
* Visualize the result in "3D" as described in "Testing Image Editor"

Saving Files
============
If you want to save all views in the same file, use EXR with the "MultiView" option.
Otherwise the views will be saved individually replacing the char "%" in the filename with the view label
e.g. //render/###%.jpg will create //render/001_L.jpg and //render/001_R.jpg

You can get images samples from:
https://github.com/openexr/openexr-images/tree/master/MultiView

