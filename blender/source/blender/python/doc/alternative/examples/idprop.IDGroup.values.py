group['some_property'] = new_value

# . . .is correct, while,

values = group.values()
values[0] = new_value

# . . .is wrong.