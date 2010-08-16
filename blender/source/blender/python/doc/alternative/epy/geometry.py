# geometry module and its subtypes

"""
This new module provides access to a geometry function.
"""

def Intersect(vec1, vec2, vec3, ray, orig, clip=1):
  """
  Return the intersection between a ray and a triangle, if possible, return None otherwise.
  @type vec1: Vector object.
  @param vec1: A 3d vector, one corner of the triangle.
  @type vec2: Vector object.
  @param vec2: A 3d vector, one corner of the triangle.
  @type vec3: Vector object.
  @param vec3: A 3d vector, one corner of the triangle.
  @type ray: Vector object.
  @param ray: A 3d vector, the orientation of the ray. the length of the ray is not used, only the direction.
  @type orig: Vector object.
  @param orig: A 3d vector, the origin of the ray.
  @type clip: integer
  @param clip: if 0, don't restrict the intersection to the area of the triangle, use the infinite plane defined by the triangle.
  @rtype: Vector object
  @return: The intersection between a ray and a triangle, if possible, None otherwise.
  """

def TriangleArea(vec1, vec2, vec3):
  """
  Return the area size of the 2D or 3D triangle defined.
  @type vec1: Vector object.
  @param vec1: A 2d or 3d vector, one corner of the triangle.
  @type vec2: Vector object.
  @param vec2: A 2d or 3d vector, one corner of the triangle.
  @type vec3: Vector object.
  @param vec3: A 2d or 3d vector, one corner of the triangle.
  @rtype: float
  @return: The area size of the 2D or 3D triangle defined.
  """

def TriangleNormal(vec1, vec2, vec3):
  """
  Return the normal of the 3D triangle defined.
  @type vec1: Vector object.
  @param vec1: A 3d vector, one corner of the triangle.
  @type vec2: Vector object.
  @param vec2: A 3d vector, one corner of the triangle.
  @type vec3: Vector object.
  @param vec3: A 3d vector, one corner of the triangle.
  @rtype: float
  @return: The normal of the 3D triangle defined.
  """

def QuadNormal(vec1, vec2, vec3, vec4):
  """
  Return the normal of the 3D quad defined.
  @type vec1: Vector object.
  @param vec1: A 3d vector, the first vertex of the quad.
  @type vec2: Vector object.
  @param vec2: A 3d vector, the second vertex of the quad.
  @type vec3: Vector object.
  @param vec3: A 3d vector, the third vertex of the quad.
  @type vec4: Vector object.
  @param vec4: A 3d vector, the fourth vertex of the quad.
  @rtype: float
  @return: The normal of the 3D quad defined.
  """

def LineIntersect(vec1, vec2, vec3, vec4):
  """
  Return a tuple with the points on each line respectively closest to the other
  (when both lines intersect, both vector hold the same value).
  The lines are evaluated as infinite lines in space, the values returned may not be between the 2 points given for each line.
  @type vec1: Vector object.
  @param vec1: A 3d vector, one point on the first line.
  @type vec2: Vector object.
  @param vec2: A 3d vector, another point on the first line.
  @type vec3: Vector object.
  @param vec3: A 3d vector, one point on the second line.
  @type vec4: Vector object.
  @param vec4: A 3d vector, another point on the second line.
  @rtype: (Vector object, Vector object)
  @return: A tuple with the points on each line respectively closest to the other.
  """

def PolyFill(polylines):
	"""
	Takes a list of polylines and calculates triangles that would fill in the polylines.
	Multiple lines can be used to make holes inside a polyline, or fill in 2 separate lines at once.
	@type polylines: List of lists containing vectors, each representing a closed polyline.
	@rtype: list
	@return: a list if tuples each a tuple of 3 ints representing a triangle indexing the points given.
	@note: 2D Vectors will have an assumed Z axis of zero, 4D Vectors W axis is ignored.
	@note: The order of points in a polyline effect the direction returned triangles face, reverse the order of a polyline to flip the normal of returned faces.
	"""

def LineIntersect2D(vec1, vec2, vec3, vec4):
	"""
	Takes 2 lines vec1, vec2 for the 2 points of the first line and vec2, vec3 for the 2 points of the second line.
	@rtype: Vector
	@return: a 2D Vector for the intersection or None where there is no intersection.
	"""

def ClosestPointOnLine(pt, vec1, vec2):
	"""
	Takes 2 lines vec1, vec2 for the 2 points of the first line and vec2, vec3 for the 2 points of the second line.
	@rtype: tuple
	@return: a tuple containing a vector and a float, the vector is the closest point on the line, the float is the position on the line, between 0 and 1 the point is on the line.
	"""

def PointInTriangle2D(pt, tri_pt1, tri_pt2, tri_pt3):
	"""
	Takes 4 vectors (one for the test point and 3 for the triangle)
	This is a 2d function so only X and Y are used, Z and W will be ignored.
	@rtype: int
	@return: 1 for a clockwise intersection, -1 for counter clockwise intersection, 0 when there is no intersection.
	"""

def PointInQuad2D(pt, quad_pt1, quad_pt2, quad_pt3):
	"""
	Takes 5 vectors (one for the test point and 5 for the quad)
	This is a 2d function so only X and Y are used, Z and W will be ignored.
	@rtype: int
	@return: 1 for a clockwise intersection, -1 for counter clockwise intersection, 0 when there is no intersection.
	"""

def BoxPack2D(boxlist):
	"""
	Takes a list of 2D boxes and packs them into a square.
	Each box in boxlist must be a list of at least 4 items - [x,y,w,h], after running this script,
	the X and Y values in each box will be moved to packed, non overlapping locations.
	
	@note: Each boxlist item can be longer then 4, the extra items are ignored and stay untouched.
	@rtype: tuple
	@return: a tuple pair - (width, height) of all the packed boxes.
	"""
def BezierInterp(vec_knot_1, vec_handle_1, vec_handle_2, vec_knot_2, resolution):
	"""
	Takes 4 vectors representing a bezier curve and returns a list of vector points.
	@note: any vector size is supported, the largest dimension from the input will be used for all returned vectors/
	@rtype: list
	@return: a list of vectors the size of resolution including the start and end points (vec_knot_1 and vec_knot_2)
	"""