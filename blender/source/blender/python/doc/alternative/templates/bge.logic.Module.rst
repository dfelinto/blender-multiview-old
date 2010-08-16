<%namespace name="util" module="util"/>
<%namespace name="layout" file="Layout.rst"/>
${layout.module_header(module)}

:class:`~bge.types.KX_GameObject` and :class:`~bge.types.KX_Camera` or :class:`~bge.types.KX_LightObject` methods are available depending on the type of object.

See the sensor's reference for available methods:

.. hlist::
   :columns: 3

% for name in sorted([_class for _class in modules['bge.types'].classes.keys() if _class.endswith('Sensor')]):
   * :class:`~bge.types.${name}`
% endfor

See the actuator's reference for available methods:

.. hlist::
   :columns: 3
   
% for name in sorted([_class for _class in modules['bge.types'].classes.keys() if _class.endswith('Actuator')]):
   * :class:`~bge.types.${name}`
% endfor

Most logic brick's methods are accessors for the properties available in the logic buttons.
Consult the logic bricks documentation for more information on how each logic brick works.

There is also a function to access the current :class:`bge.types.KX_Scene` : :func:`getCurrentScene`

Matricies as used by the game engine are **row major**
``matrix[row][col] = float``

:class:`bge.types.KX_Camera` has some examples using matricies.

<%
vars = []
dates = []
general_funcs = []
util_funcs = []
util_names = ('expandPath', 'getAverageFrameRate', 'getBlendFileList', 'getRandomFloat', 'PrintGLInfo')
for value in util.sorted_dict_values(module.dates):
	if value.name.upper() == value.name:
		dates.append(value)
	else:
		vars.append(value)
for value in util.sorted_dict_values(module.functions):
	if value.name in util_names:
		util_funcs.append(value)
	else:
		general_funcs.append(value)
%>
% if vars:
*********
Variables
*********

% for data in vars:
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
% if general_funcs:
*****************
General functions
*****************

% for function in general_funcs:
${layout.sphinx_function(function, 0)}

${layout.example(module.name+'.'+function.name, 1)}

% endfor
% endif
% if util_funcs:
*****************
Utility functions
*****************

% for function in util_funcs:
${layout.sphinx_function(function, 0)}

${layout.example(module.name+'.'+function.name, 1)}

% endfor
% endif
% if dates:
<%


%>
*********
Constants
*********

% for data in dates:
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
