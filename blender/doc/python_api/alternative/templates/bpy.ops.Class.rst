<%namespace name="layout" file="Layout.rst"/>
<%namespace name="util" module="util"/>
${_class.name} Operators
${'='*(len(_class.name) + 10)}

.. module:: ${_class.parent.name}.${_class.name.lower()}

${layout.object_addons(_class)}

${layout.example(module.name+'.'+_class.name)}

${layout.class_footer(_class, 0)}
