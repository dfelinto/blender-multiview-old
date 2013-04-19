OceanViz
========

clone from https://gitorious.org/blenderprojects/blender

Introduction
------------
This branch contains the changes we made to the [Blender](http://www.blender.org/) game engine for the *OceanViz* project.

*OceanViz*, is an interactive underwater visualization made with the Blender Game Engine.
The goal of this visualization was to accurately display the amount of marine life in the sea,
based on a scientific modeling program called [Ecopath with Ecosim (EwE)](http://www.ecopath.org/).

![Alt text](http://dalaifelinto.com/ftp/OceanViz.jpg "OceanViz © UBC Fisheries Centre")

In its current iteration, *OceanViz* is implemented as a plug-in for the EwE desktop software,
using the embedding capabilities of the Blenderplayer

![Alt text](http://dalaifelinto.com/ftp/EwEOceanVizPlugin.jpg "OceanViz © UBC Fisheries Centre; EwE Plugin Jeroen Steenbeek and Dalai Felinto")

The *OceanViz* project was conceived by professor [Dr. Villy Christensen](http://www.fisheries.ubc.ca/faculty-staff/villy-christensen).

The Blender team consists of [Dalai Felinto](http://www.dalaifelinto.com/),
with past support from [Mike Pan](http://www.mikepan.com/) and a team lead by Stephen Danic
from the MDM program at the Centre for Digital Media in Vancouver.

The EwE development team that supports the OceanViz efforts are Jeroen Steenbeek and Joe Buszowski,
with past support from Sherman Lai and Sundaran Kumar.

Features
--------
 1. Custom GLSL Uniforms per Object
 2. Multi-Panel system
 3. Resize of embed window following parent
 4. Exit game if parent window is no longer exists
