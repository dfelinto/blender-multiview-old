# You can add new properties using the [] operator.

group['a float!'] = 0.0
group['an int!'] = 0
group['a string!'] = "hi!"
group['an array!'] = [0, 0, 1.0, 0]

group['a subgroup!'] = {"float": 0.0, "an int": 1.0, "an array": [1, 2],
	"another subgroup": {"a": 0.0, "str": "bleh"}}

# You can delete properties with the del operator.

del group['property']

# To get the type of a property, use the type() operator.

if type(group['bleh']) == str: pass

# To tell if the property is a group or array type,
# import the Blender.Types module and test
# against IDGroupType and IDArrayType

from Blender.Types import IDGroupType, IDArrayType

if type(group['bleghr']) == IDGroupType: pass
