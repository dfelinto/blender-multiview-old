<%namespace name="layout" file="Layout.rst"/>
${layout.module_header(module)}
% if module.classes:
***************
Operator Groups
***************

.. toctree::
   :maxdepth: 1
   :glob:

   ${module.name}.*

% endif

