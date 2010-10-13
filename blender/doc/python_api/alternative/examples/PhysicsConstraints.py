#  Adding a point constraint  #
###############################

# import BGE internal module
import PhysicsConstraints

# get object list
obj_list = bge.logic.getCurrentScene().objects

# get object named Obj_1
root = obj_list["root"]
obj = obj_list["obj"]

# get object physics ID
phido = obj.getPhysicsId()

# get root physics ID
phidr = root.getPhysicsId()

# want to use point constraint type
constraint_type = 1

# Use bottom right front corner of object for point constraint position
point_pos_x = 1.0
point_pos_y = -1.0
point_pos_z = -1.0

# create a point constraint
const =	PhysicsConstraints.createConstraint( phido, phidr, constraint_type, point_pos_x, point_pos_y, point_pos_z)

# stores the new constraint ID to be used later
obj["constraint_ID"] = const.getConstraintId()	



#  Removing a point constraint  #
#################################

# import BGE internal module
import PhysicsConstraints

# get object list
obj_list = bge.logic.getCurrentScene().objects

# get object 1
obj = obj_list["obj"]

# get constraint ID that was saved as an obj property
# when the constraint was created
constraint_ID = obj["constraint_ID"]

# remove constraint
PhysicsConstraints.removeConstraint(constraint_ID)
