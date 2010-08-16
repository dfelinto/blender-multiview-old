<%namespace name="util" module="util"/>
<%namespace name="layout" file="Layout.rst"/>
${layout.module_header(module)}

${layout.example(module.name+'.allkeys')}

${layout.module_functions(module)}

% if module.dates:
****************
Keys (Constants)
****************

<%
mouse_keys = {}
keyboard_keys = {}
for key, value in module.dates.items():
	if key.find('MOUSE') == -1:
		keyboard_keys[key] = value
	else:
		mouse_keys[key] = value
%>
% if mouse_keys:
.. _mouse-keys:

==========
Mouse Keys
==========

% for data in util.sorted_dict_values(mouse_keys):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif

% if keyboard_keys:
<%
import re
alpha_keys = {}
number_keys = {}
modifier_keys = {}
arrow_keys = {}
numpad_keys = {}
function_keys = {}
other_keys = {}
number_names = ['ZERO', 'ONE', 'TWO', 'THREE', 'FOUR', 'FIVE', 'SIX', 'SEVEN', 'EIGHT', 'NINE']

for key, value in keyboard_keys.items():
	if key.startswith('PAD'):
		numpad_keys[key] = value
	elif key.find('ARROW') != -1:
		arrow_keys[key] = value
	elif re.match(r'F[0-9]+KEY$', key):
		function_keys[key] = value
	elif re.match(r'[A-Z]KEY$', key):
		alpha_keys[key] = value
	elif key.find('ALT') != -1 or key.find('SHIFT') != -1 or key.find('CTRL') != -1 or key.find('CAPSLOCK') != -1:
		modifier_keys[key] = value
	elif key.upper()[:-3] in number_names:
		number_keys[key] = value
	else:
		other_keys[key] = value
%>
.. _keyboard-keys:

=============
Keyboard Keys
=============

% if alpha_keys:
-------------
Alphabet keys
-------------

% for data in util.sorted_dict_values(alpha_keys):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
% if number_keys:
-----------
Number keys
-----------

% for num in number_names:
% if num + 'KEY' in number_keys:
<%
data = number_keys[num + 'KEY']
%>
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endif
% endfor
% endif
% if modifier_keys:
--------------
Modifiers Keys
--------------

% for data in util.sorted_dict_values(modifier_keys):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
% if arrow_keys:
----------
Arrow Keys
----------

% for data in util.sorted_dict_values(arrow_keys):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
% if numpad_keys:
--------------
Numberpad Keys
--------------

% for data in util.sorted_dict_values(numpad_keys):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
% if function_keys:
-------------
Function Keys
-------------

% for data in util.sorted_dict_values(function_keys):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
% if other_keys:
----------
Other Keys
----------

% for data in util.sorted_dict_values(other_keys):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${layout.object_addons(data, 1)}

% endfor
% endif
% endif
% endif
