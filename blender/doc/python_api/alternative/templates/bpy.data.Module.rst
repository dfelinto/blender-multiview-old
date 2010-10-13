<%namespace name="layout" file="Layout.rst"/>
${module.short_desc} (${module.name})
${'='*(len(module.short_desc) +len(module.name) + 3)}

*****
Intro
*****

${module.description}

${layout.object_addons(module, 1)}

.. module:: bpy

${layout.example(module.name)}

**********
Attributes
**********

.. attribute:: data

   Access to blenders internal data
   
   :type: :class:`bpy.types.Main`

