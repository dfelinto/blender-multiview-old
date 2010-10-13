<%namespace name="util" module="util"/>
<%!
from bpy import app
%>
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 Blender Documentation contents
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

This document is an API reference for Blender ${util.get_version_string()}. built ${app.build_date}.

An introduction to Blender and Python can be found at <http://wiki.blender.org/index.php/Dev:2.5/Py/API/Intro>

`A PDF version of this document is also available <blender_python_reference_250.pdf>`__

.. warning:: The Python API in Blender is **UNSTABLE**, It should only be used for testing, any script written now may break in future releases.
   
   The following areas are subject to change.
      * operator names and arguments
      * render api
      * function calls with the data api (any function calls with values accessed from bpy.data), including functions for importing and exporting meshes
      * class registration (Operator, Panels, Menus, Headers)
      * modules: bpy.props, blf)
      * members in the bpy.context have to be reviewed
      * python defined modal operators, especially drawing callbacks are highly experemental
   
   These parts of the API are relatively stable and are unlikely to change significantly
      * data API, access to attributes of blender data such as mesh verts, material color, timeline frames and scene objects
      * user interface functions for defining buttons, creation of menus, headers, panels
      * modules: bgl, mathutils and geometry

===================
Application Modules
===================

.. toctree::
   :maxdepth: 1

% for module in [key for key in modules.keys() if key.startswith('bpy')]:
   ${module}.rst

% endfor

==================
Standalone Modules
==================

.. toctree::
   :maxdepth: 1

% for module in [key for key in modules.keys() if not key.startswith('bpy') and not modules[key].short_desc.startswith('Game')]:
   ${module}.rst

% endfor

===================
Game Engine Modules
===================

.. toctree::
   :maxdepth: 1

% for module in [key for key in modules.keys() if modules[key].short_desc.startswith('Game')]:
   ${module}.rst

% endfor
