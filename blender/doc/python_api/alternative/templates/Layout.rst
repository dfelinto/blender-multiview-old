<%namespace name="util" module="util"/>

<%!
import os
%>

<%def name="class_header(_class)">
% if _class.base:
${_class.name}(${_class.base.name})
${'='*(len(_class.name) + len(_class.base.name) + 2)}
% else:
${_class.name}
${'='*len(_class.name)}
% endif

.. module:: ${_class.parent.name}

% if _class.base:
class tree --- ${' < '.join(util.get_class_tree(_class))}

.. class:: ${_class.name}(${_class.base.name})
% else:
.. class:: ${_class.name}
% endif

${util.indent(_class.description, 1)}

${object_addons(_class, 1)}

${example(module.name+'.'+_class.name, 1)}

</%def>

<%def name="class_footer(_class, indent=1)">
<%
methods = util.get_all_methods(_class)
attributes = util.get_all_attributes(_class)
%>
% if methods:
% for method, inheritedfrom in methods:
% if inheritedfrom:
${util.indent('.. {0}method:: {1}'.format(method.decorator, util.get_signature(method)), indent)}

${util.indent('Inherited from :class:`{0}.{1}`'.format(inheritedfrom, method.name), indent + 1)}
% else:
${sphinx_function(method, indent, True)}

${example(method.fullname(), indent + 1)}
% endif
% endfor
% endif
% if attributes:
% for attribute, inheritedfrom in attributes:
${util.indent('.. attribute:: {0}'.format(attribute.name), indent)}

% if inheritedfrom:
${util.indent('Inherited from :class:`{0}.{1}`; {2}'.format(inheritedfrom, attribute.name, attribute.description), indent + 1)}
% else:
${util.indent(attribute.description, indent + 1)}
% endif
% if attribute._type:

% if attribute.readonly:
${util.indent(':type: {0} (readonly)'.format(attribute._type), indent + 1)}
% else:
${util.indent(':type: {0}'.format(attribute._type), indent + 1)}
% endif
% endif

${object_addons(attribute, indent + 1)}

${example(attribute.fullname(), indent + 1)}

% endfor
% endif
</%def>

<%def name="sphinx_function(function, indent=0, method=False)">
% if method:
${util.indent('.. {0}method:: {1}'.format(function.decorator, util.get_signature(function)), indent)}
% else:
${util.indent('.. function:: ' + util.get_signature(function), indent)}
% endif

${util.indent(function.description, indent + 1)}

% if function.arguments:
% for argument in function.arguments:
${util.indent(":arg {0}: {1}".format(argument.name, argument.description), indent + 1)}
${util.indent(":type {0}: {1}".format(argument.name, argument._type), indent + 1)}
% endfor
% endif
% if function.return_values:
% if len(function.return_values) == 1:
% for return_value in function.return_values:
${util.indent(":return: {0}".format(return_value.description), indent + 1)}
${util.indent(":rtype: {0}".format(return_value._type), indent + 1)}
% endfor
% else:
% for return_value in function.return_values:
${util.indent(":return {0}: {1}".format(return_value.name, return_value.description), indent + 1)}
${util.indent(":rtype {0}: {1}".format(return_value.name, return_value._type), indent + 1)}
% endfor
% endif
% endif
% if function.location:
${util.indent(":file: `{0} <https://svn.blender.org/svnroot/bf-blender/trunk/blender/release/scripts/{0}>`_:{1}".format(function.location[0], function.location[1]), indent + 1)}
% endif

${object_addons(function, indent + 1)}
</%def>

<%def name="object_addons(obj, indent=0)">
% for note in obj.notes:
${util.indent(".. note:: {0}".format(note), indent)}
% endfor
% for warning in obj.warnings:
${util.indent(".. warning:: {0}".format(warning), indent)}
% endfor
% for seealso in obj.seealsos:
${util.indent(".. seealso:: {0}".format(seealso), indent)}
% endfor
% for deprecated in obj.deprecateds:
${util.indent(".. deprecated:: {0}".format(deprecated), indent)}
% endfor
</%def>

<%def name="example(name, indent=0)">
% if os.path.isfile('examples/'+name+'.py'):
${code(name, indent)}
% endif
</%def>

<%def name="code(name, indent=0)">
${util.indent('.. code-block:: python', indent)}

${util.indent(open('examples/'+name+'.py').read(), indent+1)}
</%def>

<%def name="module_header(module)">
${module.short_desc} (${module.name})
${'='*(len(module.short_desc) +len(module.name) + 3)}

*****
Intro
*****

${module.description}

${object_addons(module, 1)}

.. module:: ${module.name}

${example(module.name)}
</%def>

<%def name="module_classes(module)">
% if module.classes:
*******
Classes
*******

.. toctree::
   :maxdepth: 1
   :glob:

   ${module.name}.*

% endif
</%def>

<%def name="module_functions(module)">
% if module.functions:
*********
Functions
*********

% for function in util.sorted_dict_values(module.functions):
${sphinx_function(function, 0)}

${example(module.name+'.'+function.name, 1)}
% endfor
% endif
</%def>

<%def name="module_dates(module)">
% if module.dates:
*********
Constants
*********

% for data in util.sorted_dict_values(module.dates):
.. data:: ${data.name}

${util.indent(data.description, 1)}

${object_addons(data, 1)}

% endfor
% endif
</%def>

<%def name="module_footer(module)">
${module_classes(module)}
${module_functions(module)}
${module_dates(module)}
</%def>
