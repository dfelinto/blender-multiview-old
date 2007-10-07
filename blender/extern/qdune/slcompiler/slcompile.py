#-------------------------------------------------------------------------
# The actual shader compiler code
#
# Current code is too complex, should be split into
# separate 'simplify' and 'optimize' procedures.
# Current error checking can be misleading if the code uses include files
# or macros as far as the line number is concerned, since it actually
# relates to the preprocessed sourcecode file.
# Must be adapted to make use of the preprocessor #line declarations, TODO
# Of course if there are no include files, then it usually is ok.
# There is lots of ugly patch work and special case handling, as usual...
# Especially the handling of user-defined functions is a real mess.
# It should all be redone anyway, this is just a prototype after all, TODO
# (or not, all this is probably completely useless to blender...
#  at least, in its current form anyway)
#-------------------------------------------------------------------------

#-------------------------------------
# The following AST nodes are defined:
#-------------------------------------
#  1: astProgram_t(definitions)
#  3: astFormals_t(formals)
#  4: astShaderDef_t(shadertype, id, formals, body)
#  5: astFunctionDef_t(returntype, id, formals, body, lexfunc)
#  6: astNewScope_t(stmts)
#  7: astNumConst_t(val)
#  8: astStringConst_t(val)
#  9: astVecConst_t(val)
# 10: astMtxConst_t(val)
# 11: astIfElse_t(rel, if, else, iscond)
# 12: astExprList_t(expr_list, expr)
# 13: astDefExpression_t(id, array_length, def_init)
# 15: astParameter_t(output, typespec, exprs)
# 16: astVariable_t(extern, typespec, exprs)
# 17: astTuple_t(values)
# 18: astStatements_t(statements)
# 19: astReturn_t(returnval)
# 20: astBreak_t(level)
# 21: astContinue_t(level)
# 22: astFor_t(init_expr, rel, inc_expr, stmt)
# 23: astSolar_t(axis, angle, stmt)
# 24: astIlluminate_t(pos, axis, angle, stmt)
# 25: astIlluminance_t(cat, pos, axis, angle, stmt)
# 26: astID_t(name, array_idx)
# 27: astUnaryExpr_t(typecast, expr)
# 28: astBinop_t(expr1, op, expr2)
# 29: astAssignExpr_t(lhs, asgnop, assign_expr)
# 30: astRelation_t(expr1, relop, expr2)
# 31: astProcedureCall_t(name, args)
# 33: astTexture_t(tex_type, texfilechan, tex_args)
# 34: astTexFileChan_t(id, expr, array_idx)
# 35: astTexArgs_t(args)
# 36: astFilterstep_t(edge, s1, s2, paramlist)
# 37: astRotate_t(arg1, arg2, arg3)
# 38: astFresnel_t(I, N, eta, Kr, Kt, R, T)
#----------------------------------------------
# NOTE: All the above nodes also have an extra
# 'lineno' argument which holds the current
# line number in the source code.
#----------------------------------------------


# base node
class astnode_t(object):
	def visit(self, visitor):
		raise Exception('visit() not defined in ' + type(self).__name__)

# NOTE: 'children' is solely used to pretty print the AST
# (not strictly necessary even for that purpose, since could just use vars() or node.__dict__ directly,
#  but since it returns a dictionary, results are unordered)
# 'lineno' is the line number corresponding to the code line in the sourcecode, used for error reports.
class astProgram_t(astnode_t):
	def __init__(self, definitions, lineno):
		self.definitions = definitions
		self.lineno = lineno
		self.children = definitions
	def visit(self, visitor):
		visitor.visitProgram(self)

class astFormals_t(astnode_t):
	def __init__(self, formals, lineno):
		self.formals = formals
		self.lineno = lineno
		self.children = [formals]
	def visit(self, visitor):
		visitor.visitFormals(self)

class astShaderDef_t(astnode_t):
	def __init__(self, shadertype, _id, formals, body, lineno):
		self.shadertype = shadertype
		self.id = _id
		self.formals = formals
		self.body = body
		self.lineno = lineno
		self.children = [shadertype, _id, formals, body]
	def visit(self, visitor):
		visitor.visitShaderDef(self)

class astFunctionDef_t(astnode_t):
	def __init__(self, returntype, _id, formals, body, lexfunc, lineno):
		self.returntype = returntype
		self.id = _id
		self.formals = formals
		self.body = body
		self.lexfunc = lexfunc
		self.lineno = lineno
		self.children = [returntype, _id, formals, body, lexfunc]
	def visit(self, visitor):
		visitor.visitFunctionDef(self)

# only needed to keep track of scope changes
class astNewScope_t(astnode_t):
	def __init__(self, statements, lineno):
		self.statements = statements
		self.lineno = lineno
		self.children = statements
	def visit(self, visitor):
		visitor.visitNewScope(self)

# used for both floats and integers, val arg must be cast to the correct type
class astNumConst_t(astnode_t):
	def __init__(self, val, lineno):
		self.val = val
		self.lineno = lineno
		self.children = val
	def visit(self, visitor):
		visitor.visitNumConst(self)

class astStringConst_t(astnode_t):
	def __init__(self, val, lineno):
		self.val = val
		self.lineno = lineno
		self.children = val
	def visit(self, visitor):
		visitor.visitStringConst(self)

class astVecConst_t(astnode_t):
	def __init__(self, val, lineno):
		self.val = val
		self.lineno = lineno
		self.children = val
	def visit(self, visitor):
		visitor.visitVecConst(self)

class astMtxConst_t(astnode_t):
	def __init__(self, val, lineno):
		self.val = val
		self.lineno = lineno
		self.children = val
	def visit(self, visitor):
		visitor.visitMtxConst(self)

# 'iscond' designates the conditional operator
class astIfElse_t(astnode_t):
	def __init__(self, rel, _if, _else, iscond, lineno):
		self.rel = rel
		self._if = _if
		self._else = _else
		self.iscond = iscond
		self.lineno = lineno
		self.children = [rel, _if, _else, iscond]
	def visit(self, visitor):
		visitor.visitIfElse(self)

class astExprList_t(astnode_t):
	def __init__(self, expr_list, expr, lineno):
		self.expr_list = expr_list
		self.expr = expr
		self.lineno = lineno
		self.children = [expr_list, expr]
	def visit(self, visitor):
		visitor.visitExprList(self)

class astDefExpression_t(astnode_t):
	def __init__(self, _id, array_length, def_init, lineno):
		self.id = _id
		if array_length == None:
			self.array_length = 0
		else:
			self.array_length = array_length
		self.def_init = def_init
		# flag that indicates that this default expression is either part of a shader or function parameter declaration
		self.shparam = False
		self.lineno = lineno
		self.children = [_id, array_length, def_init]
	def visit(self, visitor):
		visitor.visitDefExpression(self)

class astDefExprList_t(astnode_t):
	def __init__(self, def_exprlist, lineno):
		self.def_exprlist = def_exprlist
		self.lineno = lineno
		self.children = [def_exprlist]
	def visit(self, visitor):
		visitor.visitDefExprList(self)

class astParameter_t(astnode_t):
	def __init__(self, output, typespec, exprs, lineno):
		self.output = output
		self.typespec = typespec
		self.exprs = exprs
		self.lineno = lineno
		self.children = [output, typespec, exprs]
	def visit(self, visitor):
		visitor.visitParameter(self)

class astVariable_t(astnode_t):
	def __init__(self, extern, typespec, expr, lineno):
		self.extern = extern
		self.typespec = typespec
		self.expr = expr
		self.lineno = lineno
		self.children = [extern, typespec, expr]
	def visit(self, visitor):
		visitor.visitVariable(self)

class astTuple_t(astnode_t):
	def __init__(self, values, lineno):
		self.values = values
		self.lineno = lineno
		self.children = values	# already list
	def visit(self, visitor):
		visitor.visitTuple(self)

class astStatements_t(astnode_t):
	def __init__(self, statements, lineno):
		self.statements = statements
		self.lineno = lineno
		self.children = [statements]
	def visit(self, visitor):
		visitor.visitStatements(self)

class astReturn_t(astnode_t):
	def __init__(self, returnval, lineno):
		self.returnval = returnval
		self.lineno = lineno
		self.children = returnval
	def visit(self, visitor):
		visitor.visitReturn(self)

class astBreak_t(astnode_t):
	def __init__(self, level, lineno):
		self.level = level
		self.lineno = lineno
		self.children = level
	def visit(self, visitor):
		visitor.visitBreak(self)

class astContinue_t(astnode_t):
	def __init__(self, level, lineno):
		self.level = level
		self.lineno = lineno
		self.children = level
	def visit(self, visitor):
		visitor.visitContinue(self)

class astFor_t(astnode_t):
	def __init__(self, init_expr, rel, inc_expr, stmt, lineno):
		self.init_expr = init_expr
		self.rel = rel
		self.inc_expr = inc_expr
		self.stmt = stmt
		self.lineno = lineno
		self.children = [init_expr, rel, inc_expr, stmt]
	def visit(self, visitor):
		visitor.visitFor(self)

class astSolar_t(astnode_t):
	def __init__(self, axis, angle, stmt, lineno):
		self.axis = axis
		self.angle = angle
		self.stmt = stmt
		self.lineno = lineno
		self.children = [axis, angle, stmt]
	def visit(self, visitor):
		visitor.visitSolar(self)

class astIlluminate_t(astnode_t):
	def __init__(self, pos, axis, angle, stmt, lineno):
		self.pos = pos
		self.axis = axis
		self.angle = angle
		self.stmt = stmt
		self.lineno = lineno
		self.children = [pos, axis, angle, stmt]
	def visit(self, visitor):
		visitor.visitIlluminate(self)

class astIlluminance_t(astnode_t):
	def __init__(self, cat, pos, axis, angle, stmt, lineno):
		self.cat = cat
		self.pos = pos
		self.axis = axis
		self.angle = angle
		self.stmt = stmt
		self.lineno = lineno
		self.children = [cat, pos, axis, angle, stmt]
	def visit(self, visitor):
		visitor.visitIlluminance(self)

class astID_t(astnode_t):
	def __init__(self, name, array_idx, lineno):
		self.name = name
		self.array_idx = array_idx
		self.lineno = lineno
		self.children = [name, array_idx]
	def visit(self, visitor):
		visitor.visitID(self)

class astUnaryExpr_t(astnode_t):
	def __init__(self, typecast, expr, lineno):
		self.typecast = typecast
		self.expr = expr
		self.lineno = lineno
		self.children = [typecast, expr]
	def visit(self, visitor):
		visitor.visitUnaryExpr(self)

class astBinop_t(astnode_t):
	def __init__(self, expr1, op, expr2, lineno):
		self.expr1 = expr1
		self.op = op
		self.expr2 = expr2
		self.lineno = lineno
		self.children = [expr1, op, expr2]
	def visit(self, visitor):
		visitor.visitBinop(self)

class astAssignExpr_t(astnode_t):
	def __init__(self, lhs, asgnop, assign_expr, lineno):
		self.lhs = lhs
		self.asgnop = asgnop
		self.assign_expr = assign_expr
		self.lineno = lineno
		self.children = [lhs, asgnop, assign_expr]
	def visit(self, visitor):
		visitor.visitAssignExpr(self)

class astRelation_t(astnode_t):
	def __init__(self, expr1, relop, expr2, lineno):
		self.expr1 = expr1
		self.relop = relop
		self.expr2 = expr2
		self.lineno = lineno
		self.children = [expr1, relop, expr2]
	def visit(self, visitor):
		visitor.visitRelation(self)

class astProcedureCall_t(astnode_t):
	def __init__(self, name, args, lineno):
		self.name = name
		self.args = args
		self.lineno = lineno
		self.children = [name, args]
	def visit(self, visitor):
		visitor.visitProcedureCall(self)

class astTexture_t(astnode_t):
	def __init__(self, tex_type, texfilechan, tex_args, lineno):
		self.tex_type = tex_type
		self.texfilechan = texfilechan
		self.tex_args = tex_args
		self.lineno = lineno
		self.children = [tex_type, texfilechan, tex_args]
	def visit(self, visitor):
		visitor.visitTexture(self)

class astTexFileChan_t(astnode_t):
	def __init__(self, _id, expr, array_idx, lineno):
		self.id = _id
		self.expr = expr
		self.array_idx = array_idx
		self.lineno = lineno
		self.children = [_id, expr, array_idx]
	def visit(self, visitor):
		visitor.visitTexFileChan(self)

class astTexArgs_t(astnode_t):
	def __init__(self, args, lineno):
		self.args = args
		self.lineno = lineno
		self.children = args
	def visit(self, visitor):
		visitor.visitTexArgs(self)

class astFilterstep_t(astnode_t):
	def __init__(self, edge, s1, s2, paramlist, lineno):
		self.edge = edge
		self.s1 = s1
		self.s2 = s2
		self.paramlist = paramlist
		self.lineno = lineno
		self.children = [edge, s1, s2, paramlist]
	def visit(self, visitor):
		visitor.visitFilterstep(self)

class astRotate_t(astnode_t):
	def __init__(self, arg1, arg2, arg3, lineno):
		self.arg1 = arg1
		self.arg2 = arg2
		self.arg3 = arg3
		self.arg4 = arg4
		self.lineno = lineno
		self.children = [arg1, arg2, arg3, arg4]
	def visit(self, visitor):
		visitor.visitRotate(self)

class astFresnel_t(astnode_t):
	def __init__(self, I, N, eta, Kr, Kt, R, T, lineno):
		self.I = I
		self.N = N
		self.eta = eta
		self.Kr = Kr
		self.Kt = Kt
		self.R = R
		self.T = T
		self.lineno = lineno
		self.children = [I, N, eta, Kr, Kt, R, T]
	def visit(self, visitor):
		visitor.visitFresnel(self)

#------------------------------------------------------------

class visitor_t(object):
	def visitProgram(self, node): raise Exception('visitProgram not defined in ' + type(self).__name__)
	def visitFormals(self, node): raise Exception('visitFormals not defined in ' + type(self).__name__)
	def visitShaderDef(self, node): raise Exception('visitShaderDef not defined in ' + type(self).__name__)
	def visitFunctionDef(self, node): raise Exception('visitFunctionDef not defined in ' + type(self).__name__)
	def visitNewScope(self, node): raise Exception('visitNewScope not defined in ' + type(self).__name__)
	def visitNumConst(self, node): raise Exception('visitNumConst not defined in ' + type(self).__name__)
	def visitStringConst(self, node): raise Exception('visitStringConst not defined in ' + type(self).__name__)
	def visitVecConst(self, node): raise Exception('visitVecConst not defined in ' + type(self).__name__)
	def visitMtxConst(self, node): raise Exception('visitMtxConst not defined in ' + type(self).__name__)
	def visitIfElse(self, node): raise Exception('visitIfElse not defined in ' + type(self).__name__)
	def visitExprList(self, node): raise Exception('visitExprList not defined in ' + type(self).__name__)
	def visitDefExpression(self, node): raise Exception('visitDefExpression not defined in ' + type(self).__name__)
	def visitDefExprList(self, node): raise Exception('visitDefExprList not defined in ' + type(self).__name__)
	def visitParameter(self, node): raise Exception('visitParameter not defined in ' + type(self).__name__)
	def visitVariable(self, node): raise Exception('visitVariable not defined in ' + type(self).__name__)
	def visitTuple(self, node): raise Exception('visitTuple not defined in ' + type(self).__name__)
	def visitStatements(self, node): raise Exception('visitStatements not defined in ' + type(self).__name__)
	def visitReturn(self, node): raise Exception('visitReturn not defined in ' + type(self).__name__)
	def visitBreak(self, node): raise Exception('visitBreak not defined in ' + type(self).__name__)
	def visitContinue(self, node): raise Exception('visitContinue not defined in ' + type(self).__name__)
	def visitFor(self, node): raise Exception('visitFor not defined in ' + type(self).__name__)
	def visitSolar(self, node): raise Exception('visitSolar not defined in ' + type(self).__name__)
	def visitIlluminate(self, node): raise Exception('visitIlluminate not defined in ' + type(self).__name__)
	def visitIlluminance(self, node): raise Exception('visitIlluminance not defined in ' + type(self).__name__)
	def visitID(self, node): raise Exception('visitID not defined in ' + type(self).__name__)
	def visitUnaryExpr(self, node): raise Exception('visitUnaryExpr not defined in ' + type(self).__name__)
	def visitBinop(self, node): raise Exception('visitBinop not defined in ' + type(self).__name__)
	def visitAssignExpr(self, node): raise Exception('visitAssignExpr not defined in ' + type(self).__name__)
	def visitRelation(self, node): raise Exception('visitRelation not defined in ' + type(self).__name__)
	def visitProcedureCall(self, node): raise Exception('visitProcedureCall not defined in ' + type(self).__name__)
	def visitTexture(self, node): raise Exception('visitTexture not defined in ' + type(self).__name__)
	def visitTexFileChan(self, node): raise Exception('visitTexFileChan not defined in ' + type(self).__name__)
	def visitTexArgs(self, node): raise Exception('visitTexArgs not defined in ' + type(self).__name__)
	def visitFilterstep(self, node): raise Exception('visitFilterstep not defined in ' + type(self).__name__)
	def visitRotate(self, node): raise Exception('visitRotate not defined in ' + type(self).__name__)
	def visitFresnel(self, node): raise Exception('visitFresnel not defined in ' + type(self).__name__)

#------------------------------------------------------------

# built-in functions
import slprocedures

#------------------------------------------------------------

class vector_t(object):
	def __init__(self, *args):
		L = len(args)
		if L == 0:	# defaults
			self.v = (0.0, 0.0, 0.0)
		elif L == 1:	# single float, assign to all
			self.v = (args[0], args[0], args[0])
		elif L == 3:
			self.v = args
		else:
			raise Exception("vector_t: Expected 3 values got %d -> %s" % (L, args))
		self.detail = None
		self.idx = 0
	def __iter__(self): return self
	def next(self):
		if self.idx == 3:
			self.idx = 0
			raise StopIteration
		self.idx += 1
		return self.v[self.idx - 1]
	# next math ops for direct evaluation where possible while building ast
	def __add__(self, o):
		return vector_t(self.v[0] + o.v[0], self.v[1] + o.v[1], self.v[2] + o.v[2])
	def __sub__(self, o):
		return vector_t(self.v[0] - o.v[0], self.v[1] - o.v[1], self.v[2] - o.v[2])
	def __mul__(self, o):
		if isinstance(o, float) or isinstance(o, int):
			return vector_t(self.v[0] * o, self.v[1] * o, self.v[2] * o)
		else:
			return vector_t(self.v[0] * o.v[0], self.v[1] * o.v[1], self.v[2] * o.v[2])
	def __div__(self, o):
		if isinstance(o, float) or isinstance(o, int):
			return vector_t(self.v[0] / o, self.v[1] / o, self.v[2] / o)
		else:
			return vector_t(self.v[0] / o.v[0], self.v[1] / o.v[1], self.v[2] / o.v[2])
	def __eq__(self, o):
		if self.v[0] == o.v[0] and self.v[1] == o.v[1] and self.v[2] == o.v[2]: return True
		return False
	def __ne__(self, o):
		if self.v[0] == o.v[0] or self.v[1] == o.v[1] or self.v[2] == o.v[2]: return False
		return True
	def dot(self, o):
		return self.v[0]*o.v[0] + self.v[1]*o.v[1] + self.v[2]*o.v[2]
	def cross(self, o):
		return vector_t(self.v[1]*o.v[2] - self.v[2]*o.v[1], self.v[2]*o.v[0] - self.v[0]*o.v[2], self.v[0]*o.v[1] - self.v[1]*o.v[0])
	# the following three funcs might seem odd, but asString() is used for direct evaluation using eval()
	# __str__() is used to print the values in a format needed for the compiler
	# and __repr__() is just useful as general object info, like when used with print
	def asString(self):
		return "vector_t(%g, %g, %g)" % (self.v[0], self.v[1], self.v[2])
	def __str__(self):
		return "%s %s %s" % self.v
	def __repr__(self):
		return "<vector_t %g %g %g>" % self.v

class matrix_t(object):
	def __init__(self, *args):
		L = len(args)
		if L == 0:	# defaults
			self.m = (0.0, 0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0,  0.0, 0.0, 0.0, 0.0)
		elif L == 1:	# single float, assign to mtx.diagonal
			self.m = (args[0], 0.0, 0.0, 0.0,  0.0, args[0], 0.0, 0.0,  0.0, 0.0, args[0], 0.0,  0.0, 0.0, 0.0, args[0])
		elif L == 16:
			self.m = args
		else:
			raise Exception("matrix_t: Expected 16 values got %d" % L)
		self.detail = None
		self.idx = 0
	def __iter__(self): return self
	def next(self):
		if self.idx == 16:
			self.idx = 0
			raise StopIteration
		self.idx += 1
		return self.m[self.idx - 1]
	def __str__(self):
		return "%s %s %s %s  %s %s %s %s  %s %s %s %s  %s %s %s %s" % self.m
	def __repr__(self):
		return "<matrix_t %g %g %g %g  %g %g %g %g  %g %g %g %g  %g %g %g %g>" % self.m

# function parameter flags used in the constant_t, register_t, variable_t class below
FF_OUTPUT = 1	# 'output' declared variable, so may be modified by function
FF_FPARAM = 2	# variable is a function parameter

# register class
# type is one char of 'fbmvs' for float, bool, matrix, vector and string respectively (vector is also used for color/point & normal)
# detail is either 'u' or 'v', for uniform  or varying
# number is the register number used as its id
class register_t(object):
	def __init__(self, _type, detail, number):
		self.type = _type
		self.detail = detail
		self.number = number
		# function flags, not set directly here in ctor
		self.flags = 0	# NOTE: mutable, do not use for comparisons or hashing!
	def __eq__(self, other):
		if type(self) != type(other): return False
		if other.type == self.type and other.detail == self.detail and other.number == self.number: return True
		return False
	def __ne__(self, other):
		if type(self) != type(other): return True
		if other.type != self.type and other.detail != self.detail and other.number != self.number: return True
		return False
	def  __hash__(self):
		return hash(self.type + self.detail) ^ self.number
	# String representation for registers and constants are somewhat complicated, both can be created and 'deleted' at any time.
	# This means that a simple '$number' representation will not look right, multiple registers/constants with the same name might be the result,
	# (They are still unique since its hash value is based on all its members, only the names will look the same)
	# Therefore here the name is created using all the important members (besides aforementioned reason, it also makes it more readable for 'visual debugging' purposes ;),
	# optionally after the code has been built all can be converted to the simpler name if desired ('simplify_regnames' option).
	def __str__(self):
		return "$r%s%d" % (self.type + self.detail, self.number)
	def __repr__(self):
		if self.flags:
			return "<register_t FUNC $r%s%d>" % (self.type + self.detail, self.number)
		return "<register_t $r%s%d>" % (self.type + self.detail, self.number)

# constant value register class
# type is one char of 'fbmvs' for float, bool, matrix, vector and string respectively (vector is also used for color/point & normal)
# number is the register number used as its id
# val is its assigned value
# refc is its refrerence count (numbers can be used more than once in the program, so it can not be removed until the ref.count has reached 0)
# constants are always uniform, and also have global scope
class constant_t(object):
	def __init__(self, _type, number, val):
		self.type = _type
		self.number = number
		self.val = val
		self.refc = 1	# NOTE: mutable, do not use for comparisons or hashing!
		self.flags = 0	# NOTE: mutable, do not use for comparisons or hashing!
	def __eq__(self, other):
		if type(self) != type(other): return False
		if other.type == self.type and other.number == self.number and other.val == self.val: return True
		return False
	def __ne__(self, other):
		if type(self) != type(other): return True
		if other.type != self.type and other.number != self.number and other.val != self.val: return True
		return False
	def  __hash__(self):
		return hash(self.type) ^ self.number ^ hash(self.val)
	def __str__(self):
		return "$c%s%d" % (self.type, self.number)
	def __repr__(self):
		return "<constant_t $c%s%d>" % (self.type, self.number)

# variable class
# type is one char of 'fmvcpns' for float, matrix, vector, color, point, normal, and string respectively (bool not used here, temp or const register only)
# detail is either 'u' or 'v', for uniform  or varying
# name is the name of the variable
# arlen is an optional array length
# scope is an integer indicating the scope level of the variable
# flags is an integer bitmask, see above FF_ list
class variable_t(object):
	def __init__(self, _type, detail, name, arlen, scope):
		self.type = _type
		self.detail = detail
		self.name = name
		self.arlen = arlen
		self.scope = scope
		self.flags = 0	# NOTE: mutable, do not use for comparisons or hashing!
	def __eq__(self, other):
		if type(self) != type(other): return False
		if other.type == self.type and other.detail == self.detail and other.name == self.name and other.arlen == self.arlen and other.scope == self.scope: return True
		return False
	def __ne__(self, other):
		if type(self) != type(other): return True
		if other.type != self.type and other.detail != self.detail and other.name != self.name and other.arlen != self.arlen and other.scope != self.scope: return True
		return False
	def __hash__(self):
		return hash(self.name + self.type + self.detail) ^ self.arlen ^ hash(self.scope)
	def __str__(self):
		#if self.scope: return "%s_%d" % (self.name, self.scope)
		return self.name
	def __repr__(self):
		ar = ""
		if self.arlen: ar = '[' + str(self.arlen) + ']'
		st = ""
		if self.flags: st = "FUNC"
		if self.scope: return "<variable_t %s '%s_%d%s'>" % (st, self.name, self.scope, ar)
		return "<variable_t %s '%s'>" % (st, self.name + ar)

# array class
class array_t(object):
	def __init__(self, isconst, values):
		self.isconst = isconst
		self.values = values
		self.idx = 0
	def __iter__(self): return self
	def next(self):
		if self.idx == len(self.values):
			self.idx = 0
			raise StopIteration
		self.idx += 1
		return self.values[self.idx - 1]
	def __repr__(self):
		return "<array_t %s>" % self.values

class compile_t(visitor_t):
	# used for conversion cpn types to v
	cpn_set = "cpn"
	# dictionary of all possible typecasts.
	# this is more tolerant than most, since basically the typecast is based on the type size,
	# not its actual type, so a color can be cast to a normal for instance.
	typecast_dict = {'f': "fcpvnm", 's': 's', 'c': "cpvn", 'p': "cpvn", 'v': "cpvn", 'n': "cpvn", 'm': 'm'}
	# probably more correct version of the above, though only difference is that a color cannot be cast to anything but itself, but not currently used
	#typecast_dict = {'f': "fcpvnm", 's': 's', 'c': 'c', 'p': "pvn", 'v': "pvn", 'n': "pvn", 'm': 'm'}
	# dictionary to expand type character to full type name
	fulltype = {'f': "float", 'b': "bool", 's': "string", 'c': "color", 'p': "point", 'v': "vector", 'n': "normal", 'm': "matrix", '*': "void"}
	# same for detail type
	fulldetail = {'u': "uniform", 'v': "varying"}
	# relation operator
	relop_dict = {'<': "lt", "<=": "le", "==" : "eq", "!=": "ne", ">=": "ge", '>': "gt"}
	# 'not' of relation operator, needed for relation expression of for(){} loops
	notrelop_dict = {'<': "ge", "<=": "gt", "==" : "ne", "!=": "eq", ">=": "lt", '>': "le"}
	# as above, but using the actual operator name as key, only used in visitFor()
	w_notrelop_dict = {"lt": "ge", "le": "gt", "eq" : "ne", "ne": "eq", "ge": "lt", "gt": "le"}

	def __init__(self, optimize = True, simplify_regnames = True):
		# the value of all variable dicts is a string designating the type and detail,
		# it consists of the type character (one of 'cpvnfsm') followed by the detail type, 'v' or 'u' for either 'varying' or 'uniform'

		# set of all registers used (sets require python version >= 2.4, though could just as well use dictionary with any arbitrary value)
		self.tempregs = set()
		# dictionary to keep track of register usage count, if 0, can be removed, used for optimization code
		self.reguse = {}
		# set of all variables defined in the shader
		self.variables = set()
		# this dict is used to make sure variable names are unique, since it is possible that variables have same names but different types/detail/scope
		self.allnames = {}
		# the set of global variables used by the shader
		self.globals_used = set()
		# list of lists of variable dictionaries ordered according to scope, global first, top is current local scope
		self.scopes = []
		# dict. of constants, one where value is the key, and for the other the key is its associated register
		self.const_byval = {}
		self.const_byreg = {}
		# dictionary of all parameters, value is a list containing the type and detail string and its default value if it has one
		self.params = {}
		# initialization code
		self.initcode = []
		# main code
		self.code = []
		# dictionary of user-defined functions
		self.userfuncs = {}
		# temporary, a list, only available while processing function parameters list to keep the parameters in order
		self.funcparams = None
		# dictionary of variables declared 'extern', only used when processing functions
		self.vars_extern ={}
		# flags that are set when inside a userdefined function body and handling 'extern' var.declarations
		self.infuncbody = False
		self.islexfunc = False
		self.externvar = False
		# the intermediate stack, from which values are popped for a register based representation
		# NOTE: only call newreg() *after* first popping all args of the stack (in reverse order!)
		self.stack = []
		# registers are unlimited and denoted by '$number', so to keep track of all currently 'allocated' registers, only need a simple counter
		self.curreg = 0
		# since constants are handled separately, they need their own counter
		self.const_curreg = 0
		# label counter
		self.curlabel = 0
		# current type and detail of a default declaration as a two-char string
		self.typedet = None
		# current scope level
		self.curscope = 0
		# flag that is set when currently evaluating the lhs of an assignment (needed because of array element assignment, lhs/rhs are handled differently)
		self.asgn_lhs = False
		# flag when set indicates rhs of assigment is also an assignment (eg. 'a = b = c = d = 1.23;')
		self.chain_asgn = False
		# the name of the shader which is also used as the name of the output file + ".sqd"
		self.shader_name = None
		# stack that keeps track of start & end labels in for/while loops, needed for 'continue' & 'break' statements
		self.looplabels = []
		# If 'optimize' is true, try to optimize the code a bit
		self.optimize = optimize
		# 'simplify_regnames' when true will simplify the register names to a simple '$number' representation, see register_t and constant_t classes for more info
		self.simplify_regnames = simplify_regnames

	# return new register name of type t and detail d, where t is 'f', 's', 'v' or 'm' (c, p, or n are converted to vector), and d is either 'u' or 'v'
	def newreg(self, t, d):
		if t in self.cpn_set: t = 'v'
		self.curreg += 1
		reg = register_t(t, d, self.curreg)
		# update or add register usage count
		if reg in self.tempregs:
			self.reguse[reg] += 1
		else:
			self.reguse[reg] = 0
		self.tempregs.add(reg)
		return reg

	# 'delete' register
	def delreg(self):
		self.curreg -= 1
		assert self.curreg >= 0	# if not positive, there must be some error somewhere

	# return a new constant register of type t and value v
	def newconst(self, t, v):
		if self.const_byval.has_key(v): # already defined, inc ref.count
			creg = self.const_byval[v]
			creg.refc += 1
		else:	# new constant
			self.const_curreg += 1
			creg = constant_t(t, self.const_curreg, v)
			self.const_byval[v] = creg
			self.const_byreg[creg] = v
		return creg

	# 'delete' a constant register, returns its value
	def delconst(self, creg):
		val = self.const_byreg[creg]
		creg.refc -= 1
		if creg.refc == 0:
			self.const_byreg.pop(creg)
			self.const_byval.pop(val)
			self.const_curreg -= 1
		return val

	# returns type and detail as two char pair of given variable
	# converts 'cpn' to 'v' if 'convert' true
	def typeof(self, var, convert = True):
		if isinstance(var, str) and var[0] == '"':
			return 'su'
		elif self.params.has_key(var):
			tp = self.params[var][0].lower()
		elif var in self.tempregs:
			tp = var.type + var.detail
		elif self.const_byreg.has_key(var):
			tp = var.type  + 'u'
		elif isinstance(var, register_t):
			tp = var.type + var.detail
		elif isinstance(var, constant_t):
			tp = var.type + 'u'
		elif isinstance(var, variable_t):
			tp = var.type + var.detail
		else:
			# search all variable dictionaries starting at the current scope
			# first match is returned
			tp = None
			for scopelist in reversed(self.scopes):
				for vardict in reversed(scopelist):
					if vardict.has_key(var):
						tv = vardict[var]
						tp = tv.type + tv.detail
						break
			if tp == None:
				raise Exception("typeof(): Cannot determine type of: '%s' " % var)
		# convert cpn to v
		if convert:
			if tp[0] in self.cpn_set: tp = 'v' + tp[1]
		return tp

	# push new item on stack
	def push(self, item):
		self.stack.append(item)

	# pop and return item from stack
	def pop(self):
		if len(self.stack) == 0:
			raise Exception("Empty stack!")
		item = self.stack.pop()
		# if popping a register (not constant), 'delete' it, UNLESS it is currently a function parameter (must remain on stack until function exit)
		if isinstance(item, register_t) and ((item.flags & FF_FPARAM) == 0):
			self.delreg()
		return item

	# returns top item on stack without popping it
	def top(self):
		return self.stack[-1]

	def visitProgram(self, node):
		for d in node.definitions:
			d.visit(self)

	def visitFormals(self, node):
		if node.formals:
			for f in node.formals:
				f.visit(self)

	def visitShaderDef(self, node):
		self.shader_name = node.id	# used for output file
		self.code.append(node.shadertype + " " + node.id)

		# now that the shader type is known, add the dictionary of predefined variables that are accessible to the shader,
		# (note that for instance in the case of a surface shader, and it redefines the output variables Ci and or Oi in the main scope, the shader will be pretty much useless...)
		if node.shadertype == "surface":
			predefs = {"Cs": "cv", "Os": "cv", "dPdu": "vv", "dPdv": "vv", "P": "pv", "N": "nv", "Ng": "nv",
			           "u": "fv", "v": "fv", "du": "fv", "dv": "fv", "s": "fv", "t": "fv", "L": "vv",
			           "Cl": "cv", "Ol": "cv", "E": "pu", "I": "vv",
			           "ncomps": "fu", "time": "fu", "dtime": "fu", "dPdtime": "fv",
			           "Ci": "cv", "Oi": "cv"}
		elif node.shadertype == "light":
			predefs = {"dPdu": "vv", "dPdv": "vv", "P": "pv", "N": "nv", "Ng": "nv",
			           "u": "fv", "v": "fv", "du": "fv", "dv": "fv", "s": "fv", "t": "fv", "L": "vv",
			           "Ps": "pv", "Cl": "cv", "Ol": "cv", "E": "pu",
			           "ncomps": "fu", "time": "fu", "dtime": "fu"}
		elif node.shadertype == "displacement":
			predefs = {"dPdu": "vv", "dPdv": "vv", "P": "pv", "N": "nv", "Ng": "nv",
			           "u": "fv", "v": "fv", "du": "fv", "dv": "fv", "s": "fv", "t": "fv",
			           "E": "pu", "I": "vv",
			           "ncomps": "fu", "time": "fu", "dtime": "fu",
			           "Ci": "cv", "Oi": "cv"}
		elif node.shadertype == "volume":
			predefs = {"Cs": "cv", "Os": "cv", "dPdu": "vv", "dPdv": "vv", "P": "pv", "N": "nv", "Ng": "nv",
			           "u": "fv", "v": "fv", "du": "fv", "dv": "fv", "s": "fv", "t": "fv", "L": "vv",
			           "Ps": "pv", "Cl": "cv", "Ol": "cv", "E": "pu", "I": "vv",
			           "ncomps": "fu", "time": "fu", "dtime": "fu", "dPdtime": "fv",
			           "alpha": "fu", "Ci": "cv", "Oi": "cv"}
		elif node.shadertype == "imager":
			predefs = {"P": "pv", "ncomps": "fu", "time": "fu", "dtime": "fu",
			           "alpha": "fu", "Ci": "cv", "Oi": "cv"}
		else:	# never happens
			raise Exception("Line %d: Unknown shader type?!? -> '%s'" % node.lineno, node.shadertype)
		d = {}
		for name in predefs:
			td = predefs[name]
			newvar = variable_t(td[0], td[1], name, 0, None)	# note: scope set to None, indicates global var.
			d[name] = newvar
			# add names to variable name list as well to make sure no duplicates are created
			self.allnames[name] = 0	# dict is empty yet, so this is the first entry
		self.scopes.append([d])

		if node.formals: node.formals.visit(self)
		# add the main code start label if initialization code was created,
		# always 1 (or should be anyway, no jumps in initcode possible as far as I can see..)
		if len(self.initcode):
			self.curlabel += 1
			self.code.append(["@%d" % self.curlabel, None])
		if node.body:
			self.curscope = 0	# main scope
			self.scopes.append([{}])
			node.body.visit(self)
			self.scopes.pop()

	def visitFunctionDef(self, node):
		# Store the node into a table, later when the function is called it will be expanded inline.
		# 'argtypes' holds the function parameter types and detail,
		# except the first, which is only the returntype char, detail is not known in advance.
		# Return type 'v' is reserved for vector, '*' is for 'void' (also used to indicate 'variadic' when used in func.args).
		# Uppercase chars mean the variable is an 'output' declared var, so may be modified by the function.
		if node.returntype == "void":
			argtypes = ['*']
		else:
			if node.lexfunc:
				argtypes = [node.returntype[1][0]]
			else:
				argtypes = [node.returntype[0]]
			if argtypes[0] in self.cpn_set: argtypes[0] = 'v'
		varnames = []	# variable names used in the function, only used to map to arguments
		if node.formals:
			# declare list to temporarily store the function parameters in order
			self.funcparams = []
			fdict = {}
			self.scopes.append([fdict])
			node.formals.visit(self)
			for vname in self.funcparams:
				varnames.append(vname)
				var = fdict[vname]
				# if variable is 'output', flag argument typechar for handleUserFunc() below by making it uppercase
				if var.flags & FF_OUTPUT:
					td = self.typeof(var)
					argtypes.append(td[0].upper() + td[1])
				else:
					argtypes.append(self.typeof(var))
			# remove the dictionary again
			self.scopes.pop()
			# temporary var.list can also be removed
			self.funcparams = None
		self.userfuncs[node.id] = [argtypes, varnames, node]

	# used in visitProcedureCall() for userdefined functions
	def handleUserFunc(self, funcdef):
		argtypes, varnames, node = funcdef
		if node.body:
			# new temporary scope, no reference to any previous scope, except for 'extern' declared vars
			funcparams = {}
			for i in range(len(varnames)):
				vname = varnames[i]
				exp_tp = argtypes[i + 1]
				var = self.stack[-(i+1)]	# no pop yet, since arguments are used directly, done after all func.params processed, see below
				var_tp = self.typeof(var)
				output = False
				# type of variable must be exected type
				if exp_tp[0].isupper():
					output = True
					exp_tp = exp_tp[0].lower() + exp_tp[1]
				# types must be same
				if var_tp[0] != exp_tp[0]:
					raise Exception("Line %d: function '%s' argument '%s' type '%s' not same as function parameter '%s' of type '%s'" % (node.lineno, node.id, repr(var), var_tp[0], vname, exp_tp[0]))
				# if parameter detail is uniform, then argument must also be uniform, anything else ok
				if exp_tp[1] == 'u' and var_tp[1] != 'u':
					raise Exception("Line %d: function '%s' argument '%s' is varying but function parameter '%s' expects uniform" % (node.lineno, node.id, repr(var), vname))
				newvar = var
				if not isinstance(var, str):
					newvar.flags |= FF_FPARAM
					if output:
						assert isinstance(var, constant_t) == False	# constants can never be output
						newvar.flags |= FF_OUTPUT
				funcparams[vname] = newvar
			# all params done, now can pop values
			for fp in funcparams.values():
				self.pop()
			# prepare for the function body
			self.curscope += 1
			pifb = self.infuncbody
			self.infuncbody = True
			pilf = self.islexfunc
			self.islexfunc = node.lexfunc
			if node.lexfunc:
				self.scopes[-1].append(funcparams)	# add to parent scope
			else:
				self.scopes.append([funcparams])	# new scope
			last_externs = self.vars_extern.copy()
			node.body.visit(self)
			# reset everything
			self.vars_extern = last_externs
			for fp in funcparams.values():
				if not isinstance(fp, str):
					if fp.flags & FF_FPARAM: fp.flags = 0
			self.islexfunc = pilf
			self.infuncbody = pifb
			if node.lexfunc:
				self.scopes[-1].pop()
			else:
				self.scopes.pop()
			self.curscope -= 1

	def visitNewScope(self, node):
		self.curscope += 1
		self.scopes[-1].append({})
		node.statements.visit(self)
		# delete all variables now out of scope
		self.scopes[-1].pop()
		self.curscope -= 1

	def visitNumConst(self, node):
		self.push(self.newconst('f', node.val))

	def visitVecConst(self, node):
		self.push(self.newconst('v', node.val))

	def visitMtxConst(self, node):
		self.push(self.newconst('m', node.val))

	def visitStringConst(self, node):
		self.push(self.newconst('s', node.val))

	# only used in visitIfElse_t() below, tries to find out type & detail from name string
	def typefromstring(self, s):
		if s[0] == '$':
			if s[1] == 'c':
				return s[2] + 'u'
			return s[2] + s[3]
		else:
			try:
				td = self.typeof(s)
			except:
				# try variable list, have to do linear search, not indexed by name
				for v in self.variables:
					if str(v) == s: return v.type + v.detail
				# nothing...
				raise Exception("Cannot determine type from string '%s'" % s)
			else:
				return td

	def visitIfElse(self, node):
		nrel = node.rel
		nrel.visit(self)
		if node.iscond:	# conditional operator, has both if/else
			node._if.visit(self)
			node._else.visit(self)
			op3, op2, op1 = self.pop(), self.pop(), self.pop()
			op1t, op2t, op3t = self.typeof(op1), self.typeof(op2), self.typeof(op3)
			# op2/op3 must be same type/detail
			if op2t == op3t:
				lhs = self.newreg(op2t[0], op1t[1])	# note: lhs must be detail of boolean here, it indicates detail of the actual statement lhs
				self.code.append(["cmov%s" % op2t[0], "%s %s %s %s" % (lhs, op1, op2, op3)])
				self.push(lhs)
			else:	# not same type/detail, have to do full if/else
				lhs = self.newreg(op2t[0], op2t[1])
				self.code.append(["cond_push", "%s" % op1])
				self.code.append(["mov%s" % (op2t[0] + op2t[0]), "%s %s" % (lhs, op2)])
				self.code.append(["cond_else", None])
				self.code.append(["mov%s" % (op2t[0] + op2t[0]), "%s %s" % (lhs, op3)])
				self.code.append(["cond_pop", None])
				self.push(lhs)
			return
		# simple optimization, if relation expression operands are both uniform, can replace it by single conditional jump
		# (This might in fact not be an optimization at all, performance may actually be worse, have to test, not sure)
		# update: ok, tested, it is definitely faster ('uberlight' shader, about 4 sec. less)
		if self.optimize and nrel.relop != "&&" and nrel.relop != "||" and nrel.relop != '!':	# not logic stmts
			last_codeline = self.code[-1]
			last_oprs = last_codeline[1].split()
			# in this case, have to determine type/detail from actual name string if register name ('$' is first char)
			lop1t, lop2t = self.typefromstring(last_oprs[1]), self.typefromstring(last_oprs[2])
			if lop1t[1] == 'u' and lop2t[1] == 'u':
				self.curlabel += 2
				lab1, lab2 = self.curlabel-1, self.curlabel
				# want 'not' of relation here
				opc = self.notrelop_dict[nrel.relop]
				last_codeline[0], last_codeline[1] = "j%s" % (opc + lop1t[0] + lop2t[0]), "%s %s @%d" % (last_oprs[1], last_oprs[2], lab1)
				# the lhs of the old statement can now be deleted if not still in use
				last_lhs = self.pop()
				if self.reguse[last_lhs] == 0:
					self.tempregs.remove(last_lhs)
				node._if.visit(self)
				if node._else: self.code.append(["jmp", "@%d" % lab2])
				self.code.append(["@%d" % lab1, None])
				if node._else:
					node._else.visit(self)
					self.code.append(["@%d" % lab2, None])
				return
		# 'complex' or relation expression using varyings
		lhs = self.pop()
		if self.typeof(lhs)[1] == 'u':
			# uniform relation, can do a conditional jump
			self.curlabel += 2
			lab1, lab2 = self.curlabel-1, self.curlabel
			self.code.append(["cjmpnot", "%s @%d" % (lhs, lab1)])
			node._if.visit(self)
			if node._else: self.code.append(["jmp", "@%d" % lab2])
			self.code.append(["@%d" % lab1, None])
			if node._else:
				node._else.visit(self)
				self.code.append(["@%d" % lab2, None])
			return
		# varying relation
		self.code.append(["cond_push", "%s" % lhs])
		node._if.visit(self)
		if node._else:
			self.code.append(["cond_else", None])
			node._else.visit(self)
		self.code.append(["cond_pop", None])

	def visitExprList(self, node):
		if node.expr_list: node.expr_list.visit(self)
		node.expr.visit(self)

	# add a new variable to set
	# there can be vars with the same name, but have different type,
	# so this function is used to make sure names are unique
	def addNewVariable(self, var):
		name = str(var)
		if name in self.allnames:
			self.allnames[name] += 1
			var.name += "_%d" % self.allnames[name]
		else:
			self.allnames[name] = 0
		self.variables.add(var)

	# separate code to handle array default expressions
	def handleDefArray(self, node):
		arr = node.def_init
		if arr.isconst:
			if node.shparam:
				# check redefinition
				if self.params.has_key(node.id):
					raise Exception("Line %d: Parameter '%s' already defined" % (node.lineno, node.id))
				self.params[node.id] = [self.typedet, arr.values]
			else:
				tp = self.typedet
				newvar = variable_t(tp[0], tp[1], node.id, node.array_length, self.curscope)
				if self.funcparams == None: self.addNewVariable(newvar)
				for i in range(node.array_length):
					self.code.append(["movta%s" % (tp[0] + tp[0]), "%s %s %s" % (node.id, self.newconst('f', i), self.newconst(tp[0], arr.values[i]))])
		else:
			# array not completely constant, initialization code needed
			lastcodelen = len(self.code)	# used to copy newly added code to initcode segment if required
			if node.shparam:
				tp = self.typedet
				if tp[0] == 'f':
					defvals = [0.0]*node.array_length
				elif tp[0] == 'v':
					defvals = [vector_t() for i in range(node.array_length)]
					for v in defvals: v.detail = tp[1]
				elif tp[0] == 'm':
					defvals = [matrix_t() for i in range(node.array_length)]
					for m in defvals: m.detail = tp[1]
				elif tp[0] == 's':
					defvals = [""]*node.array_length
				else:	# never happens
					raise Exception("Line %d: DefExpression() array, unexpected type!?! -> %s" % (node.lineno, tp))
				for i in range(node.array_length):
					if isinstance(arr.values[i], astnode_t):
						arr.values[i].visit(self)
						self.code.append(["movta%s" % (tp[0] + tp[0]), "%s %s %s" % (node.id, self.newconst('f', i), self.pop())])
					else:
						defvals[i] = arr.values[i]
				# check redefinition
				if self.params.has_key(node.id):
					raise Exception("Line %d: Parameter '%s' already defined" % (node.lineno, node.id))
				self.params[node.id] = [tp, defvals]
				# move newly added codelines to initcode
				lc = len(self.code) - lastcodelen
				if lc != 0:
					for i in range(lc):
						self.initcode.append(self.code[i + lastcodelen])
					for i in range(lc):
						self.code.pop()
			else:
				tp = self.typedet
				newvar = variable_t(tp[0], tp[1], node.id, node.array_length, self.curscope)
				if self.funcparams == None: self.addNewVariable(newvar)
				for i in range(node.array_length):
					if isinstance(arr.values[i], astnode_t):
						arr.values[i].visit(self)
						val = self.pop()
						self.code.append(["movta%s" % (tp[0] + tp[0]), "%s %s %s" % (node.id, self.newconst('f', i), val)])
					else:
						self.code.append(["movta%s" % (tp[0] + tp[0]), "%s %s %s" % (node.id, self.newconst('f', i), self.newconst(tp[0], arr.values[i]))])

		if not node.shparam:
			# check possible redefinition
			curscope = self.scopes[-1][-1]
			if node.id in curscope:
				raise Exception("Line %d: Variable '%s' already defined" % (node.lineno, node.id))
			# if 'extern' declared variable, add to externlist
			if self.externvar:
				self.vars_extern[node.id] = self.typedet
			else:
				# add new variable if not processing function parameters
				newvar = variable_t(self.typedet[0], self.typedet[1], node.id, node.array_length, self.curscope)
				if self.funcparams == None: self.addNewVariable(newvar)
				# also add to current scope
				if not curscope.has_key(node.id):
					curscope[node.id] = newvar
					# add to ordered function parameter list if processing function parameters
					if self.funcparams != None:
						self.funcparams.append(node.id)


	def visitDefExpression(self, node):
		lastcodelen = len(self.code)	# used to copy newly added code to initcode segment if required
		if node.def_init:
			if isinstance(node.def_init, array_t):
				self.handleDefArray(node)
				return
			node.def_init.visit(self)
		need_initcode = False
		isconstparam = False
		if node.shparam:
			val = self.top()
			if isinstance(val, constant_t):
				# constant register, no initcode
				need_initcode = False
				isconstparam = True
			elif isinstance(val, register_t):
				need_initcode = True
			elif val in self.params:
				# parameter ID, replace by its value
				val = self.params[val][1]
				need_initcode = False
				isconstparam = True
			else:	# should never happen
				raise Exception("Line " + str(node.lineno) + ": Don't know what to do with this -> " + repr(val) + " (pytype: %s)" % type(val))
			if need_initcode:
				# since it is initialized later, use a default value for the declaration
				tp = self.typeof(val)
				# check implicit typecast
				if self.typedet[0] not in self.typecast_dict[tp[0]]:
					raise Exception("Line %d: parameter '%s', Illegal implicit typecast of %s to %s" % (node.lineno, node.id, self.fulltype[tp[0]], self.fulltype[self.typedet[0]]))
				if tp[0] == 'f':
					defval = 0.0
				elif tp[0] == 'v':
					defval = vector_t()
					defval.detail = tp[1]
				elif tp[0] == 'm':
					defval = matrix_t()
					defval.detail = tp[1]
				elif tp[0] == 's':
					defval = ""
				else:	# never happens
					raise Exception("Line " + str(node.lineno) + ": DefExpression() Unexpected type!?! -> " + tp)
				# check redefinition
				if self.params.has_key(node.id):
					raise Exception("Line %d: Parameter '%s' already defined" % (node.lineno, node.id))
				self.params[node.id] = [self.typedet, defval]
				# move newly added codelines to initcode
				lc = len(self.code) - lastcodelen
				if lc == 0: # nothing was added, so no initcode
					need_initcode = False
				else:
					for i in range(lc):
						self.initcode.append(self.code[i + lastcodelen])
					for i in range(lc):
						self.code.pop()
			else:	# no initcode
				# if val is a constant register, replace with its actual value,
				# and since this is a parameter, remove the constant value from the dictionary (unless use count != 0),
				# and delete the value just pushed onto the stack as well
				if isconstparam:
					tp = self.typeof(val)[0]
					# check implicit typecast
					if self.typedet[0] not in self.typecast_dict[tp[0]]:
						raise Exception("Line %d: parameter '%s', Illegal implicit typecast of %s to %s" % (node.lineno, node.id, self.fulltype[tp[0]], self.fulltype[self.typedet[0]]))
					reg = self.pop()
					val = self.delconst(reg)
					# a parameter declaration can also require an implicit typecast, eg. color c = 1
					# if so, expand to full v/c/p/n triple
					if (self.typedet[0] == 'v' or (self.typedet[0] in self.cpn_set)) and tp == 'f':
						val = "%g %g %g" % (val, val, val)
				# check redefinition
				if self.params.has_key(node.id):
					raise Exception("Line %d: Parameter '%s' already defined" % (node.lineno, node.id))
				self.params[node.id] = [self.typedet, val]
				# if this is a parameter with a constant default value, there is nothing to do after this, so can return now
				if isconstparam: return

		# 'temp' type, no default value, is assigned in code
		newvar = None
		if not node.shparam:
			curscope = self.scopes[-1][-1]
			# check possible redefinition
			if node.id in curscope:
				raise Exception("Line %d: Variable '%s' already defined" % (node.lineno, node.id))
			# if 'extern' declared variable, add to externlist
			if self.externvar:
				self.vars_extern[node.id] = self.typedet
			else:
				# add new variable set if not processing function parameters
				newvar = variable_t(self.typedet[0], self.typedet[1], node.id, node.array_length, self.curscope)
				if self.funcparams == None: self.addNewVariable(newvar)
				# also add to current scope
				if not curscope.has_key(node.id):
					curscope[node.id] = newvar
					# add to ordered function parameter list if processing function parameters
					if self.funcparams != None:
						self.funcparams.append(node.id)
		# optional default assignment code
		if need_initcode:
			curcode = self.initcode
		else:
			curcode = self.code
		if node.def_init:
			op2 = self.pop()
			# type here is the last defined type
			op1t, op2t = self.typedet, self.typeof(op2)
			# convert op1t to v if cpn
			if op1t[0] in self.cpn_set: op1t = 'v' + op1t[1]
			# simple optimization, if previous line of code is 'opcode op2 ...' and op2 is a register,
			# and this line will be 'mov op1 op2' then concatenate the two, modifying previous codeline to 'opcode op1 ...'
			# the two operands of mov must be same type, but can be different detail, but in case of mixed detail, lhs must be varying
			if self.optimize and isinstance(op2, register_t) and curcode[-1][1]:
				last_codeline = curcode[-1][1]
				last_ops = last_codeline.split()
				op2str = str(op2)
				if last_ops[0] == op2str and op1t[0] == op2t[0] and not (op1t[1] == 'u' and op2t[1] == 'v'):
					if not node.shparam:	# use name of newly created variable
						curcode[-1][1] = str(newvar) + last_codeline[len(op2str):]
					else:
						curcode[-1][1] = node.id + last_codeline[len(op2str):]
					# if op2 is only the lhs of the previous line, remove op from tempregs, result register no longer used
					op2_count = 0
					for opr in last_ops:
						if opr == op2str: op2_count += 1
					if op2_count == 1:
						print "OPTIM 1 rm", op2, "USE", self.reguse[op2]
						if self.reguse[op2] == 0:
							self.tempregs.remove(op2)
					return
			# the actual default initialization, if this is not a parameter OR op2 is a register or constant add a mov instruction
			if (not node.shparam) or isinstance(op2, register_t) or isinstance(op2, constant_t):
				# check implicit typecast
				if op1t[0] not in self.typecast_dict[op2t[0]]:
					raise Exception("Line %d: variable '%s', Illegal implicit typecast of %s to %s" % (node.lineno, node.id, self.fulltype[op2t[0]], self.fulltype[op1t[0]]))
				# check detail, lhs must be varying if rhs is varying
				if op2t[1] == 'v' and op1t[1] != 'v':
					raise Exception("Line %d: variable '%s', cannot assign varying to uniform" % (node.lineno, node.id))
				typest = op1t[0] + op2t[0]
				if newvar:
					curcode.append(["mov%s" % typest, "%s %s" % (newvar, op2)])
				else:
					curcode.append(["mov%s" % typest, "%s %s" % (node.id, op2)])

	def visitDefExprList(self, node):
		for e in node.expr_list:
			e.visit(self)

	def visitParameter(self, node):
		# set type and detail for any possible default expressions, just use first char of each
		ptd = self.typedet
		if node.typespec[1] == "void":	# exception is void since 'v' already reserved for vector, use '*' instead in that case
			self.typedet = '*' + node.typespec[0][0]
		else:
			self.typedet = node.typespec[1][0] + node.typespec[0][0]
		for e in node.exprs:
			e.visit(self)
		# set the 'output' flag if needed
		if node.output:
			csd = self.scopes[-1][-1]
			for dexprs in node.exprs:
				if csd.has_key(dexprs.id):
					csd[dexprs.id].flags |= FF_OUTPUT
				elif self.params.has_key(dexprs.id):
					# shader parameter, make typechar uppercase to flag as output
					ptdval = self.params[dexprs.id]
					self.params[dexprs.id] = [ptdval[0][0].upper() + ptdval[0][1], ptdval[1]]
		# reset type & detail, now unknown until eg. assignment statements etc.
		self.typedet = ptd

	def visitVariable(self, node):
		# set type and detail for any possible default expressions
		ptd = self.typedet
		if node.typespec[1] == "void":	# exception is void since 'v' already reserved for vector, use '*' instead in that case
			self.typedet = '*' + node.typespec[0][0]
		else:
			self.typedet = node.typespec[1][0] + node.typespec[0][0]
		# set extern flag for the duration of the function parameter declarations if required
		if self.infuncbody and node.extern:
			pev = self.externvar
			self.externvar = True
		for e in node.expr:
			e.visit(self)
		if self.externvar: self.externvar = pev
		# reset type & detail, now unknown until eg. assignment statements etc.
		self.typedet = ptd

	def visitTuple(self, node):
		# note: must visit all values first, then pop
		lhs_det = 'u'	# lhs detail is varying if any value of tuple is varying
		for tpval in node.values:
			tpval.visit(self)
			if self.typeof(self.top())[1] == 'v': lhs_det = 'v'
		tup = []
		for tpval in node.values:
			tup.append(self.pop())
		tup.reverse()
		ltp = len(node.values)
		if ltp == 3:
			v = vector_t(*tup)
			v.detail = lhs_det
			# code to 'cast' actual vector to register
			rv = self.newreg('v', v.detail)
			self.code.append(["movvf3", "%s %s" % (rv, v)])
			self.push(rv)
		elif ltp == 16:
			m = matrix_t(*tup)
			m.detail = lhs_det
			# cast to matrix register var
			rm = self.newreg('m', m.detail)
			self.code.append(["movmf16", "%s %s" % (rm, m)])
			self.push(rm)
		else:	# never happens
			raise Exception("Line %d: Expected tuple length of 3 or 16, got %d" % (node.lineno, ltp))

	def visitStatements(self, node):
		if node.statements:
			for s in node.statements:
				s.visit(self)

	def visitReturn(self, node):
		if node.returnval:
			node.returnval.visit(self)
			op1 = self.pop()
			op1t = self.typeof(op1)
			lhs = self.newreg(op1t[0], op1t[1])
			if lhs != op1:	# don't add code if move-to-self
				# don't have to check detail here, since both have same type/detail
				self.code.append(["mov%s" % (op1t[0] + op1t[0]), "%s %s" % (lhs, op1)])
			self.push(lhs)

	def visitBreak(self, node):
		# must do this differently, maybe add separate VM instruction for this, disabled for now
		raise Exception("Line %d: 'break' statement not implemented yet" % node.lineno)
		# jump to end of loop
		numlab = len(self.looplabels)
		if numlab != 0:
			self.code.append(["cond_reset", None])
			if node.level:
				if node.level > numlab:
					raise Exception("Line %d: illegal level in 'break' statement" % node.lineno)
				self.code.append(["jmp", "@%d" % (self.looplabels[-node.level][1])])
			else:
				self.code.append(["jmp", "@%d" % (self.looplabels[-1][1])])

	def visitContinue(self, node):
		# must do this differently, maybe add separate VM instruction for this, disabled for now
		raise Exception("Line %d: 'continue' statement not implemented yet" % node.lineno)
		# jump to start of loop
		numlab = len(self.looplabels)
		if numlab != 0:
			self.code.append(["cond_reset", None])
			if node.level:
				if node.level > numlab:
					raise Exception("Line %d: illegal level in 'continue' statement" % node.lineno)
				self.code.append(["jmp", "@%d" % (self.looplabels[-node.level][0])])
			else:
				self.code.append(["jmp", "@%d" % (self.looplabels[-1][0])])

	def visitFor(self, node):
		if node.init_expr:	# while stmt has no init.expr
			node.init_expr.visit(self)
		self.curlabel += 2
		lab1, lab2 = self.curlabel-1, self.curlabel
		self.looplabels.append([lab1, lab2])
		self.code.append(["@%d" % lab1, None])
		nrel = node.rel
		vary = False
		nrel.visit(self)
		op1 = self.pop()
		op1t = self.typeof(op1)
		# this doesn't always work for some reason, not entirely sure why yet (shadowedclouds.sl infinite loop, maybe uni/vary cond. mixup?), so disabled for now...
		"""
		# as in visitIfElse(), simplify code to a compare and jump if simple relation expression with uniform operands
		if self.optimize and op1t[1] == 'u' \
			and not isinstance(nrel.expr1, astRelation_t) and not isinstance(nrel.expr2, astRelation_t) \
			and nrel.relop != "&&" and nrel.relop != "||" and nrel.relop != '!':	# not logic stmts
			last_codeline = self.code[-1]
			ops = last_codeline[1].split()
			last_codeline[0] = "j" + self.w_notrelop_dict[last_codeline[0][2:4]] + last_codeline[0][4:]
			last_codeline[1] = "%s %s @%d" % (ops[1], ops[2], lab2)
			# can remove lhs if not in use (not indexed by name, so have to do a search here)
			last_lhs = None
			for reg in self.reguse.keys():
				if str(reg) == ops[0]:
					last_lhs = reg
					break
			if last_lhs and self.reguse[last_lhs] == 0:
				print "OPTIM 6 rm", last_lhs, "USE", self.reguse[last_lhs]
				self.tempregs.remove(last_lhs)
		else:	# varying, or not possible to optimize
		"""
		self.code.append(["cjmpnot", "%s @%d" % (op1, lab2)])
		if op1t[1] == 'v':
			self.code.append(["cond_push", str(op1)])
			vary = True
		node.stmt.visit(self)
		if vary: self.code.append(["cond_pop", None])
		if node.inc_expr:	# while stmt has no incr.expr
			node.inc_expr.visit(self)
		self.code.append(["jmp", "@%d" % lab1])
		self.code.append(["@%d" % lab2, None])
		self.looplabels.pop()

	def visitSolar(self, node):
		if node.axis:
			node.axis.visit(self)
			node.angle.visit(self)
			op2, op1 = self.pop(), self.pop()
			self.code.append(["solar2", "%s %s" % (op1, op2)])
		else:
			self.code.append(["solar1", None])
		node.stmt.visit(self)
		self.code.append(["end_solar", None])

	def visitIlluminate(self, node):
		if node.pos: node.pos.visit(self)
		if node.axis: node.axis.visit(self)
		if node.angle: node.angle.visit(self)
		if node.pos and ((node.axis == None) and (node.angle == None)):
			self.code.append(["illuminate1", "%s" % self.pop()])
		else:
			op3, op2, op1 = self.pop(), self.pop(), self.pop()
			self.code.append(["illuminate2", "%s %s %s" % (op1, op2, op3)])
		if node.stmt: node.stmt.visit(self)
		self.code.append(["end_illuminate", None])

	def visitIlluminance(self, node):
		if node.cat:	# category, not used yet
			node.cat.visit(self)
			category = self.pop()
		node.pos.visit(self)
		if node.axis: node.axis.visit(self)
		if node.angle: node.angle.visit(self)
		self.curlabel += 2
		lab1, lab2 = self.curlabel-1, self.curlabel
		self.code.append(["@%d" % lab1, None])
		if node.pos and ((node.axis == None) and (node.angle == None)):
			self.code.append(["illuminance1", "%s @%d" % (self.pop(), lab2)])
		else:
			op3, op2, op1 = self.pop(), self.pop(), self.pop()
			self.code.append(["illuminance2", "%s %s %s @%d" % (op1, op2, op3, lab2)])
		node.stmt.visit(self)
		self.code.append(["jmp", "@%d" % lab1])
		self.code.append(["@%d" % lab2, None])

	def getID(self, node):
		# if currently inside a function, only search local scope, unless there are any 'extern' declared variables
		if self.infuncbody:
			for vardict in reversed(self.scopes[-1]):	# last first
				if vardict.has_key(node.name):
					return vardict[node.name]
			# not found, search for possibly extern declared vars (global scope)
			if self.vars_extern and self.vars_extern.has_key(node.name):
				# found, but declared type must be same as that of the global variable
				dtp, etp = self.vars_extern[node.name], self.typeof(node.name, False)
				if dtp != etp:
					raise Exception("Line %d: 'extern' declared variable '%s' is wrong type, declared as '%s', expected '%s'" % (node.lineno, node.name, self.fulltype[dtp[0]], self.fulltype[etp[0]]))
				if self.islexfunc:
					# function declared inside shader or other function, can also access 'parent' scope
					scl = len(self.scopes[-1])
					for vardict in reversed(self.scopes[-1]):
						scl -= 1
						if vardict.has_key(node.name):
							var = vardict[node.name]
							# if the variable was found in the very first scope dictionary, then it was a global predefined shader variable
							if scl == 0: self.globals_used.add(node.name)
							return var
					# not found, try parameters
					if self.params.has_key(node.name):
						return node.name
					# nothing found
					raise Exception("Line %d: Unknown 'extern' declared variable '%s'" % (node.lineno, node.name))
				else:
					# function outside main scope, only search global
					if self.scopes[0][0].has_key(node.name):
						# add to global list
						self.globals_used.add(node.name)
						return self.scopes[0][0][node.name]
					# not found
					raise Exception("Line %d: Unknown 'extern' declared variable '%s'" % (node.lineno, node.name))
			# nothing found
			raise Exception("Line %d: Unknown function variable '%s' (forgot 'extern' declaration?)" % (node.lineno, node.name))
		# Not a function
		# search the parameter list first, since these are stored separately
		# if found, push its name, not the value
		if node.name in self.params: return node.name
		# search the scopelist from current to older ones for the identifier
		scl = len(self.scopes)
		for scopelist in reversed(self.scopes):
			scl -= 1
			for vardict in reversed(scopelist):
				if vardict.has_key(node.name):
					var = vardict[node.name]
					# if the variable was found in the very first scope dictionary, then it was a global predefined shader variable
					if scl == 0: self.globals_used.add(node.name)
					return var
		# not found, try parameter name match
		if self.params.has_key(node.name):
			return node.name
		# unknown, error
		raise Exception("Line %d: Unknown identifier '%s'" % (node.lineno, node.name))

	def visitID(self, node):
		# value bound to id
		self.push(self.getID(node))
		# if array index specified, add a move-from-array instruction, unless ID is on the left hand side (move-to-array handled in visitAssignExpr())
		if node.array_idx and not self.asgn_lhs:
			op1 = self.pop()
			node.array_idx.visit(self)
			op2 = self.pop()	# array index
			op1t, op2t = self.typeof(op1), self.typeof(op2)
			lhs = self.newreg(op2t[0], op2t[1])
			self.code.append(["movfa%s" % (op2t[0] + op2t[0]), "%s %s %s" % (lhs, op1, op2)])
			self.push(lhs)

	def visitUnaryExpr(self, node):
		if node.typecast:
			if node.typecast == '-':	# not typecast, but a negation
				node.expr.visit(self)
				op1 = self.pop()
				op1t = self.typeof(op1)
				lhs = self.newreg(op1t[0], op1t[1])
				self.code.append(["neg%s" % (op1t[0]*2), "%s %s" % (lhs, op1)])
				self.push(lhs)
				return
			else:	# actual typecast
				vartype, spacetype = node.typecast[0], node.typecast[1]
				newtype = vartype[0]
				if newtype in self.cpn_set: newtype = 'v'
				# temporarily reset the current type
				ptd = self.typedet
				if ptd: self.typedet = newtype + self.typedet[1]
				node.expr.visit(self)
				self.typedet = ptd
				# nothing to do further if no transform required and expression result is already correct type
				if (spacetype == None) and (newtype == self.typeof(self.top())[0]): return	# NOTE: do not pop! value must remain on stack in this case
				op2 = self.pop()
				op2t = self.typeof(op2)
				# check explicit cast validity
				if vartype[0] not in self.typecast_dict[op2t[0]]:
					raise Exception("Line %d: Illegal typecast of %s to %s" % (node.lineno, self.fulltype[op2t[0]], vartype))
				# add code for implicit type cast if needed (eg. point p = 1)
				cast = None
				if op2t[0] == 'f' and (newtype == 'v' or newtype == 'm'):
					lhs = self.newreg(newtype, op2t[1])
					# don't have to check detail here, since both have same detail
					self.code.append(["mov%s" % (newtype + 'f'), "%s %s" % (lhs, op2)])
					op2 = lhs
					op2t = self.typeof(op2)
					cast = True
				if spacetype:	# transform code needed
					spacetype.visit(self)
					lhs = self.newreg(vartype[0], op2t[1])
					self.code.append(["%stocurr" % vartype[0], "%s %s %s" % (lhs, self.pop(), op2)])
					self.push(lhs)
				else:	# only implicit type case or nothing to be done at all, push last expression result (note: have to 're-alloc' to keep regcount in sync!)
					if cast:
						self.push(op2)
					else:
						self.push(self.newreg(op2t[0], op2t[1]))

	def visitBinop(self, node):
		node.expr1.visit(self)
		node.expr2.visit(self)
		# lhs type depends on operand types
		op2, op1 = self.stack[-1], self.stack[-2]	# cannot pop yet because of possible typecast, must be done after new register request !
		op1t, op2t = self.typeof(op1), self.typeof(op2)
		# for the 'fv' or 'vf' cases need extra code to make float a vector
		if op1t[0] == 'f' and op2t[0] == 'v':
			if node.op == '*':
				op2, op1 = self.pop(), self.pop()
				# multiply, swap operands if type order 'fv', want 'vf'
				op1t, op2t = op2t, op1t
				op1, op2 = op2, op1
			else:	# for all others need extra mov to make float a vector
				lhs = self.newreg('v', op1t[1])
				op2, op1 = self.pop(), self.pop()
				# no detail check needed, both same
				self.code.append(["movvf", "%s %s" % (lhs, op1)])
				op1 = lhs
				op1t = 'v' + op1t[1]
		elif op1t[0] == 'v' and op2t[0] == 'f' and node.op != '*' and node.op != '/':	# need extra mov to make float a vector (unless mul/div)
			lhs = self.newreg('v',  op2t[1])
			op2, op1 = self.pop(), self.pop()
			# no detail check needed, both same
			self.code.append(["movvf", "%s %s" % (lhs, op2)])
			op2 = lhs
			op2t = 'v' + op2t[1]
		else:
			# nothing, just pop
			op2, op1 = self.pop(), self.pop()
		# check implicit typecast
		rop1t, rop2t = self.typeof(op1, False), self.typeof(op2, False)	# 'real' types
		if rop1t[0] not in self.typecast_dict[rop2t[0]]:
			raise Exception("Line %d: Illegal implicit typecast of %s to %s" % (node.lineno, self.fulltype[rop2t[0]], self.fulltype[rop1t[0]]))
		if node.op == '*':
			opc = "mul"
		elif node.op == '/':
			opc = "div"
		elif node.op == '+':
			opc = "add"
		elif node.op == '-':
			opc = "sub"
		elif node.op == '^':
			opc = "vcross"
		elif node.op == '.':
			opc = "vdot"
		else:	# never happens
			raise Exception("Line %d: Unknown binary operator '%s'?" % (node.lineno, node.op))
		# lhs type, use type of first operand (should always be ok, I think...)
		if node.op == '.':	# vdot returns float
			lhs_type = 'f' + op1t[1]
		else:
			if op1t[1] == 'v' and op2t[1] == 'v':	# both varying, lhs varying
				lhs_type = op1t
			elif op1t[1] == 'v' or op2t[1] == 'v':	# either varying, lhs varying
				lhs_type = op1t[0] + 'v'
			else:	# both uniform
				lhs_type = op1t[0] + 'u'
		lhs = self.newreg(lhs_type[0], lhs_type[1])
		# last three char part of instruction which consists of the argument types, but only if not vdot or vcross, which don't have any
		if node.op != '.' and node.op != '^':
			opc += lhs_type[0] + op1t[0] + op2t[0]
		# simple optimization, if last codeline was a multiply of the form 'mul x x y' and this line will be '(add|sub) z z x'
		# then concatenate the two, modifying previous to 'm(add|sub) z x y' form
		last_codeline = self.code[-1]
		optim = False
		if self.optimize and (node.op == '+' or node.op == '-') and last_codeline[0][:3] == "mul":
			last_ops = last_codeline[1].split()
			op2str = str(op2)
			if last_ops[0] == last_ops[1] and last_ops[0] == op2str and lhs == op1:
				# if op2 is only the lhs of the previous line, remove op from tempregs, result register no longer used
				op2_count = 0
				for opr in last_ops:
					if opr == op2str: op2_count += 1
				if op2_count == 1:
					print "OPTIM 2 rm", op2, "USE", self.reguse[op2]
					if self.reguse[op2] == 0:
						self.tempregs.remove(op2)
				# modify codeline
				if node.op == '+':
					last_codeline[0] = "madd" + last_codeline[0][3:]	# same type extension as previous code
				else:
					last_codeline[0] = "msub" + last_codeline[0][3:]	# same type extension as previous code
				last_codeline[1] = str(lhs) + last_codeline[1][len(op2str):]
				optim = True
		if not optim:
			self.code.append(["%s" % opc, "%s %s %s" % (lhs, op1, op2)])
		self.push(lhs)

	def visitAssignExpr(self, node):
		self.asgn_lhs = True	# needs to be set here to only add a move-from-array instuction when appearing on rhs
		node.lhs.visit(self)
		self.asgn_lhs = False	# and reset again
		op1 = self.pop()
		op1t = self.typeof(op1)
		# set current type and detail to type of lhs
		ptd = self.typedet
		self.typedet = op1t
		# set flag if rh.expr is assignment as well
		pca = self.chain_asgn
		if isinstance(node.assign_expr, astAssignExpr_t):
			self.chain_asgn = True
		node.assign_expr.visit(self)
		# reset to previous
		self.chain_asgn = pca
		# reset type & detail
		self.typedet = ptd
		op2 = self.pop()
		op2t = self.typeof(op2)
		# for the 'fv' or 'vf' cases need extra code to make float a vector
		if op1t[0] == 'f' and op2t[0] == 'v':
			if node.asgnop == "*=":
				# multiply, swap operands if type order 'fv', want 'vf'
				op1t, op2t = op2t, op1t
				op1, op2 = op2, op1
			else:	# for all others need extra mov to make float a vector
				lhs = self.newreg('v', op1t[1])
				# no detail check needed, both same
				self.code.append(["movvf", "%s %s" % (lhs, op1)])
				op1 = lhs
				op1t = 'v' + op1t[1]
		elif op1t[0] == 'v' and op2t[0] == 'f' and node.asgnop != '*' and node.asgnop != '/':	# need extra mov to make float a vector (unless mul/div)
			lhs = self.newreg('v',  op2t[1])
			# no detail check needed, both same
			self.code.append(["movvf", "%s %s" % (lhs, op2)])
			op2 = lhs
			op2t = 'v' + op2t[1]
		# check implicit typecast
		rop1t, rop2t = self.typeof(op1, False), self.typeof(op2, False)	# 'real' types
		if rop1t[0] not in self.typecast_dict[rop2t[0]]:
			raise Exception("Line %d: Illegal implicit typecast of %s to %s" % (node.lineno, self.fulltype[rop2t[0]], self.fulltype[rop1t[0]]))
		# insctruction lhs type depends on operand types
		# use type of first operand (should always be ok, I think...)
		if op1t[1] == 'v' and op2t[1] == 'v':	# both varying, lhs varying
			lhs_type = op1t
		elif op1t[1] == 'v' or op2t[1] == 'v':	# either varying, lhs varying
			lhs_type = op1t[0] + 'v'
		else:	# both uniform
			lhs_type = op1t[0] + 'u'
		# last two (mov) or three char part of instruction which consists of the argument types
		if node.asgnop == '=':
			# check for invalid assigment to function parameters that are not declared 'output'
			if op1 in self.params:
				# shader parameter can be used for output too, typechar is then uppercase
				if not self.params[op1][0][0].upper():
					raise Exception("Line %d: Cannot assign to shader parameter '%s', not declared 'output'" % (node.lineno, op1))
			elif (op1.flags & FF_FPARAM) and ((op1.flags & FF_OUTPUT) == 0):
				raise Exception("Line %d: Cannot assign to function parameter '%s', not declared 'output'" % (node.lineno, op1.name))
			# simple optimization, if previous line of code is 'opcode op2 ...'
			# and this line will be 'mov op1 op2' (but not move-to-array!) then concatenate the two, modifying previous codeline to 'opcode op1 ...'
			# the two operands of mov must be same type, but can be different detail, but in case of mixed detail, lhs must be varying
			# exception here is if the previous codeline was a madd/msub instruction (eg. x += y*z, cannot replace x)
			last_codeline = self.code[-1]
			op2str = str(op2)
			L = len(op2str)
			if self.optimize and not (last_codeline[0][:4] == "madd" or last_codeline[0][:4] == "msub") and not node.lhs.array_idx \
				               and isinstance(op2, register_t) and op1t[0] == op2t[0] and not (op1t[1] == 'u' and op2t[1] == 'v') \
				               and last_codeline[1] and last_codeline[1][:L] == op2str:
				# if op2 is only the lhs of the previous line, remove op from tempregs, result register no longer used
				op2_count = 0
				for opr in last_codeline[1].split():
					if opr == op2str: op2_count += 1
				if op2_count == 1:
					print "OPTIM 3 rm", op2, "USE", self.reguse[op2]
					if self.reguse[op2] == 0:
						self.tempregs.remove(op2)
				self.code[-1][1] = str(op1) + last_codeline[1][L:]
				# if chained assignment, push lhs on stack
				if self.chain_asgn: self.push(op1)
				return
			else:
				# check detail, lhs must be varying if rhs is varying
				if op2t[1] == 'v' and op1t[1] != 'v':
					raise Exception("Line %d: cannot assign varying to uniform" % node.lineno)
				typest = op1t[0] + op2t[0]
				if node.lhs.array_idx:	# assign to array element
					node.lhs.array_idx.visit(self)	# !!!
					self.code.append(["movta%s" % typest, "%s %s %s" % (op1, self.pop(), op2)])
				else:
					self.code.append(["mov%s" % typest, "%s %s" % (op1, op2)])
					# if chained assignment, push lhs on stack
					if self.chain_asgn: self.push(op1)
		elif node.asgnop == "+=" or node.asgnop == "-=":
			# simple optimization, if last codeline was a multiply of the form 'mul r y z' and this line will be '(add|sub) x x r', 'r' being a register,
			# then concatenate the two, modifying previous to 'm(add|sub) x y z' form. 'r' can then also be removed, unless r was not only lhs of the prev.line
			last_codeline = self.code[-1]
			optim = False
			if self.optimize and last_codeline[0][:3] == "mul" and isinstance(op2, register_t):
				last_ops = last_codeline[1].split()
				op2str = str(op2)
				if last_ops[0] == op2str:
					# if op2 is only the lhs of the previous line, remove op from tempregs, result register no longer used
					op2_count = 0
					for opr in last_ops:
						if opr == op2str: op2_count += 1
					if op2_count == 1:
						print "OPTIM 4 rm", op2, "USE", self.reguse[op2]
						if self.reguse[op2] == 0:
							self.tempregs.remove(op2)
					# modify codeline
					if node.asgnop == "+=":
						last_codeline[0] = "madd" + last_codeline[0][3:]	# same type extension as previous code
					else:
						last_codeline[0] = "msub" + last_codeline[0][3:]	# same type extension as previous code
					last_codeline[1] = str(op1) + last_codeline[1][len(op2str):]
					optim = True
			if not optim:
				typest = lhs_type[0] + op1t[0] + op2t[0]
				if node.asgnop == "+=":
					self.code.append(["add%s" % typest, "%s %s %s" % (op1, op1, op2)])
				elif node.asgnop == '-=':
					self.code.append(["sub%s" % typest, "%s %s %s" % (op1, op1, op2)])
		else:
			typest = lhs_type[0] + op1t[0] + op2t[0]
			if node.asgnop == "*=":
				self.code.append(["mul%s" % typest, "%s %s %s" % (op1, op1, op2)])
			elif node.asgnop == '/=':
				self.code.append(["div%s" % typest, "%s %s %s" % (op1, op1, op2)])
			else:	# never happens
				raise Exception("Line %d: Unknown assignment operator '%s'?!?" % (node.lineno, node.asgnop))
		# NOTE: Not entirely sure about this, but the result here might have to be pushed again on to the stack,
		# the grammar suggests that an expression of the form: a = (b += c) would also be valid,
		# currently this will not compile, TODO (Pixie does produce code, but result is incorrect, not sure about 3dl)

	def visitRelation(self, node):
		if node.expr1: node.expr1.visit(self)
		node.expr2.visit(self)
		opc = None
		if node.relop == '!':
			op1 = self.pop()
			lhs = self.newreg('b', self.typeof(op1)[1])
			self.code.append(["not", "%s %s" % (lhs, op1)])
			self.push(lhs)
			return
		elif node.relop == "&&":
			opc = "and"
		elif node.relop == "||":
			opc = "or"
		else:
			try:
				opc = self.relop_dict[node.relop]
			except:	# never happens
				raise Exception("Line %d: Unknown relop '%s'?!?" % (node.lineno, node.relop))
		op2, op1 = self.pop(), self.pop()
		op1t, op2t = self.typeof(op1), self.typeof(op2)
		if op1t[1] == 'v' and op2t[1] == 'v':	# both varying, lhs varying
			lhs_type = 'bv'
		elif op1t[1] == 'v' or op2t[1] == 'v':	# either varying, lhs varying
			lhs_type = 'bv'
		else:	# both uniform
			lhs_type = 'bu'
		if opc == "and" or opc == "or":
			# simple optimization, if previous codeline was of the form 'ifxxxx b2 ...', and this line will be '(and|or) b1 b1 b2'
			# AND b1 b2 have the same detail, then concatenate, modifying previous line to "andxxxx b1 ...'
			last_codeline = self.code[-1]
			if self.optimize and last_codeline[0][:2] == 'if':
				last_ops = last_codeline[1].split()
				if (last_ops[0] == str(op2)) and (op1t[1] == op2t[1]):
					last_codeline[0] = opc + last_codeline[0][2:]
					last_codeline[1] = "%s %s %s" % (op1, last_ops[1], last_ops[2])
					# NOTE even though op1 is the 'result', cannot just directly push op1 here,
					# it is necesary to 're-alloc' it anyway, since otherwise reg.count will go out of sync
					self.push(self.newreg(lhs_type[0], lhs_type[1]))
					# op2 can now be removed from tempregs
					print "OPTIM 5 rm", op2, "USE", self.reguse[op2]
					if self.reguse[op2] == 0:
						self.tempregs.remove(op2)
					return
			lhs = self.newreg(lhs_type[0], lhs_type[1])
			self.code.append([opc, "%s %s %s" % (lhs, op1, op2)])
			self.push(lhs)
		else:
			lhs = self.newreg('b', op1t[1])
			self.code.append(["if%s" % (opc + op1t[0] + op2t[0]), "%s %s %s" % (lhs, op1, op2)])
			self.push(lhs)

	# used in visitProcedureCall() below
	def lookup_slproc(self, slpnames, argtypes, convert=False):
		t = None
		for p in slpnames:
			pargs = p[1]
			if convert:
				# convert any cpn to v
				ca = ""
				for s in pargs:
					if s in self.cpn_set:
						ca += 'v'
					else:
						ca += s
				pargs = ca
			# if variadic, only need to match args up to that point
			variadic = pargs[1:].rfind('*')	# '*' after first char -> 'variadic' function
			if variadic != -1:
				if pargs[:variadic] == argtypes[:variadic]:
					t = p
					break
			elif pargs == argtypes:	# otherwise try exact match
				t = p
				break
		return t

	def visitProcedureCall(self, node):
		sts = len(self.stack)
		if node.args:
			for a in reversed(node.args):	# note: must be reverse iteration here
				a.visit(self)
		argtypes, vargs = "", ""
		# get types of all variables pushed on stack
		lhs_det = 'u' # determine lhs detail, only uniform if all operands uniform
		for i in range(len(self.stack) - sts):
			t = self.typeof(self.stack[sts + i], False)
			if t[1] == 'v': lhs_det = 'v'
			argtypes = t[0] + argtypes
			vargs = self.typeof(self.stack[sts + i])[0] + vargs
		cast = False
		if slprocedures.slprocs.has_key(node.name):
			slp = slprocedures.slprocs[node.name]
		elif slprocedures.slcastprocs.has_key(node.name):	# cast function
			slp = slprocedures.slcastprocs[node.name]
			cast = True
		else:	# not a built-in function, maybe user-defined?
			try:
				slp = self.userfuncs[node.name]
			except:
				raise Exception("Line " + str(node.lineno) + ": Unknown procedure name -> " + node.name)
			else:
				# userdefined function found, expand inline
				self.handleUserFunc(slp)
				return
		if len(slp) == 0:
			# dummy function, just pop args, skip functioncall for now, which of course might break things from here...
			for i in range(len(self.stack) - sts):
				self.pop()
			return
		# return type, if cast function, use declared type instead (unless void func.)
		rtp = slp[0]
		tfunc = (rtp == 'T')	# 'template' function
		if cast and rtp != '*': rtp = self.typedet[0]
		# if it has multiple instruction options, pick the one that matches the argument types
		slp = slp[1:]
		if len(slp) == 1:	# nothing to choose, only one option
			slproc = slp[0]
		else:
			slproc = self.lookup_slproc(slp, argtypes)
			if slproc == None:
				# no match found, use the argument string that has all 'cpn' converted to 'v' and try again
				argtypes = vargs
				slproc = self.lookup_slproc(slp, vargs, True)
				# if still nothing, give up
				if slproc == None:
					raise Exception("Line %d: Cannot find a match for procedure '%s' and required argument types '%s' (conv.'%s')" % (node.lineno, node.name, argtypes, vargs))
		# operands
		oprs = ""
		numoprs = len(self.stack) - sts
		trtp = None	# for 'template' functions, use the first template argument type as the return type
		if numoprs:
			for i in range(numoprs):
				if i < len(slproc[1]):
					if slproc[1][i] == 'T' and trtp == None: trtp = self.typeof(self.top())
				oprs += " " + str(self.pop())
		else:
			# no operands, set lhs detail to last defined (if set)
			if self.typedet:
				lhs_det = self.typedet[1]
			else:
				lhs_det = 'v'
		# if function is a 'template' function, use the first template argument type as the return type
		if tfunc:
			# if function has no arguments, use the declared type instead... (this means that not all code will compile properly unless adding casts)
			if trtp:
				rtp = trtp[0]
			else:
				rtp = self.typedet[0]
		# if cast or template function, add return type char to instruction name
		iname = slproc[0]
		if (cast or tfunc) and rtp != '*':
			if rtp in self.cpn_set:
				iname += 'v'
			else:
				iname += rtp
		if rtp == '*':	# void func, no return value
			self.code.append([iname, oprs[1:]])
		else:
			reg = self.newreg(rtp, lhs_det)
			self.code.append([iname, str(reg) + oprs])
			self.push(reg)

	# doesn't always work, texture statement not implemented completely yet in VM
	def visitTexture(self, node):
		# [tex_type, texfilechan, tex_args]
		opc = node.tex_type
		numopr = len(self.stack)
		if node.texfilechan: node.texfilechan.visit(self)
		if node.tex_args: node.tex_args.visit(self)
		numopr = len(self.stack) - numopr
		opt_oprs = ""
		if numopr:
			for i in range(numopr):
				opt_oprs += str(self.pop())
				if i != (numopr - 1): opt_oprs += " "
		if node.texfilechan:
			lhs = self.newreg(self.typedet[0], self.typedet[1])
			ntfc = node.texfilechan
			if ntfc.expr and ntfc.array_idx:
				opc += "chana"
				if opc[:7] == "texture": opc += self.typedet[0]
				self.code.append([opc, "%s %s %s %d %s" % (lhs, ntfc.id, self.pop(), ntfc.array_idx, opt_oprs)])
			elif ntfc.expr:
				opc += "chan"
				if opc[:7] == "texture": opc += self.typedet[0]
				self.code.append([opc, "%s %s %s %s" % (lhs, ntfc.id, self.pop(), opt_oprs)])
			else:
				if opc == "texture": opc += self.typedet[0]
				self.code.append([opc, "%s %s %s" % (lhs, ntfc.id, opt_oprs)])
			self.push(lhs)

	def visitTexFileChan(self, node):
		# [id, expr, array_idx]
		if node.expr: node.expr.visit(self)
		#if node.array_idx: node.array_idx.visit(self)

	def visitTexArgs(self, node):
		if node.args:
			for a in reversed(node.args):	# must do reverse iteration because of how list is built
				a.visit(self)

	def visitFilterstep(self, node):
		# edge, s1, s2, paramlist
		node.edge.visit(self)
		node.s1.visit(self)
		if node.s2: node.s2.visit(self)
		if node.paramlist: node.visit.paramlist(self)
		if node.s2 and node.paramlist:
			raise Exception("Filterstep does not handle paramer lists yet")
		elif node.s2:
			op3, op2, op1 = self.pop(), self.pop(), self.pop()
			lhs = self.newreg('f', self.typeof(op1)[1])
			self.code.append(["filterstep2", "%s %s %s %s" % (lhs, op1, op2, op3)])
			self.push(lhs)
		else:
			op2, op1 = self.pop(), self.pop()
			lhs = self.newreg('f', self.typeof(op1)[1])
			self.code.append(["filterstep1", "%s %s %s" % (lhs, op1, op2)])
			self.push(lhs)

	# TODO
	def visitRotate(self, node):
		raise Exception("rotate() function not implemented yet")

	def visitFresnel(self, node):
		node.I.visit(self)
		node.N.visit(self)
		node.eta.visit(self)
		node.Kr.visit(self)
		node.Kt.visit(self)
		op5, op4, op3, op2, op1 = self.pop(), self.pop(), self.pop(), self.pop(), self.pop()
		# NOTE: all outputs on lhs
		if node.R:	# implies node.T as well
			assert node.T != None
			node.R.visit(self)
			node.T.visit(self)
			op7, op6 = self.pop(), self.pop()
			self.code.append(["fresnel2", "%s %s %s %s %s %s %s" % (op4, op5, op6, op7, op1, op2, op3)])
		else:
			self.code.append(["fresnel1", "%s %s %s %s %s" % (op4, op5, op1, op2, op3)])

	# main, starts ast traversal and builds code
	def visit(self, node):
		# build the code
		node.visit(self)
		# simple optimization, any "cond_pop" instructions at end of code can be removed,
		# since by definition, no code can follow it, it's kind of pointless to execute it anyway...
		if self.optimize:
			while self.code[-1][0] == "cond_pop":
				self.code.pop()
		# check that the stack is now empty, if it ain't, there has to be some serious error somewhere...
		if len(self.stack) != 0:
			raise Exception("Stack is not empty! -> %s" % self.stack)
		# print out code
		assert self.shader_name != None
		fp = open(self.shader_name + ".sqd", 'w')
		fp.write("# QDSLC version 0.0.1-alpha\n")	# which it probably will always remain...
		fp.write("%s\n" % self.code[0])
		# parameters
		for name, val in self.params.iteritems():
			fp.write("param %s %s " % (self.fulldetail[val[0][1]], self.fulltype[val[0][0].lower()]))
			if isinstance(val[1], list):	# array
				st = ""
				for v in val[1]:
					st += str(v) + " "
				fp.write("%s[%d] %s\n" % (name, len(val[1]), st[:-1]))
			else:
				fp.write("%s %s\n" % (name, str(val[1])))
		# variables
		for v in self.variables:
			if v.arlen:
				fp.write("temp %s %s %s[%d]\n" % (self.fulldetail[v.detail], self.fulltype[v.type], str(v), v.arlen))
			else:
				fp.write("temp %s %s %s\n" % (self.fulldetail[v.detail], self.fulltype[v.type], str(v)))
		# temporaries
		# if 'simplify_regnames' set, first change all register names to a simpler representation, '$number'
		if self.simplify_regnames:
			sreg = {}
			numreg = 1
			for r in self.tempregs:
				sreg[str(r)] = "$%d" % numreg
				numreg += 1
			for r in self.tempregs:
				fp.write("temp %s %s %s\n" % (self.fulldetail[r.detail], self.fulltype[r.type], sreg[str(r)]))
		else:
			for r in self.tempregs:
				fp.write("temp %s %s %s\n" % (self.fulldetail[r.detail], self.fulltype[r.type], str(r)))
		# constants
		# also simplify the register names if needed
		if self.simplify_regnames:
			for c in self.const_byval:
				td = self.const_byval[c]
				sreg[str(td)] = "$%d" % numreg
				numreg += 1
			for c in self.const_byval:
				td = self.const_byval[c]
				fp.write("const %s %s %s\n" % (self.fulltype[td.type], sreg[str(td)], str(c)))
		else:
			for c in self.const_byval:
				td = self.const_byval[c]
				fp.write("const %s %s %s\n" % (self.fulltype[td.type], str(td), str(c)))
		# globals
		if len(self.globals_used):
			globstr = "global"
			for v in self.globals_used:
				globstr += " " + v
			fp.write("%s\n" % globstr)
		# code
		if len(self.initcode):
			fp.write("codesegment @1\n")	# should always be 1...
		else:
			fp.write("codesegment\n")
		spacest = " "*16
		# initcode segment
		if len(self.initcode):
			if self.simplify_regnames:
				for cl in self.initcode:
					if cl[1]:
						fp.write("\t%s%s" % (cl[0], spacest[:16-len(cl[0])]))
						for op in cl[1].split():
							if op[0] == '$':
								fp.write(" %s" % sreg[op])
							else:
								fp.write(" %s" % op)
						fp.write('\n')
					else:
						fp.write("\t%s\n" % cl[0])
			else:
				for cl in self.initcode:
					if cl[1]:
						fp.write("\t%s%s%s\n" % (cl[0], spacest[:16-len(cl[0])], cl[1]))
					else:
						fp.write("\t%s\n" % cl[0])
		# main code
		if self.simplify_regnames:
			for cl in self.code[1:]:
				if cl[1]:
					fp.write("\t%s%s" % (cl[0], spacest[:16-len(cl[0])]))
					for op in cl[1].split():
						if op[0] == '$':
							fp.write(" %s" % sreg[op])
						else:
							fp.write(" %s" % op)
					fp.write('\n')
				else:
					if cl[0][0] == '@':	# label, no tab
						fp.write("%s\n" % cl[0])
					else:
						fp.write("\t%s\n" % cl[0])
		else:
			for cl in self.code[1:]:
				if cl[1]:
					fp.write("\t%s%s%s\n" % (cl[0], spacest[:16-len(cl[0])], cl[1]))
				else:
					if cl[0][0] == '@':	# label, no tab
						fp.write("%s\n" % cl[0])
					else:
						fp.write("\t%s\n" % cl[0])
		fp.write("\treturn\n")
		fp.close()

