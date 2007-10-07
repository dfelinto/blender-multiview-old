#!/usr/bin/python

#------------------------------------------------------------------------------------------------------------------
# This is the (prototype) QuietDune shader compiler for the Renderman Shader Language.
# This is a very recent work in progress, although not unusable, results will probably vary quite a bit...
# Disclaimer again: I'm not a 'real' programmer, and I have never done things like this before,
# there probably are some rather basic errors, wrong assumptions and general stupidities here as always.
# Initially it was based on the basic RSL yacc grammar by Andrew Bromage,
# available here: http://andrew.bromage.org/renderman.html,
# which I converted to python with 'yply' of (and for) the python 'ply' parser.
# This was then further extended and modified.
# Special Thanks go to Alejandro Conty Estevez for patiently explaining some of the basics of parsing,
# as well as kindly creating some example code for me to parse/compile simple math expressions,
# on which pretty much all of this code is based on (especially the use of the visitor pattern) :)
# TODO add correct syntax error handling
#------------------------------------------------------------------------------------------------------------------

import sys
# temporary extended path (relative), ply is not installed
sys.path.insert(0,"../..")

from slcompile import *
#from astsimplify import *

# dictionary that is used to keep track of any declared variables with constant values for direct evaluation, see evalBinop() below
# when exiting braced statements and after any function it is reset to simplify scope handling
# (TODO add scope info as well, now some constant statements will still be executed in code)
var_dict = {}
# set when any parse/syntax error occured
parse_error = False


# NOTE: while the idea was nice and sofar it doesn't seem to have caused any problems for all currently tested shaders,
#       the evalsomething() funcs below can't possibly work properly in a first pass,
#       not when variables are involved at least, so that case is disabled for now, pure number constants are ok.
#       (eg. "i=0; while (i<10) { i += 1; }" would be 'optimized' to "i=0;  while (i<10) { i=1; }" <- infinite loop)

# for the binop expressions, can do 'constant folding/propagation' when both operands are
# either constant numbers or variables with constant values,
# in which case the result node will be another constant number node
def evalBinop(p):
	p1_isnum = isinstance(p[1], astNumConst_t)
	p3_isnum = isinstance(p[3], astNumConst_t)
	p1_isvec = isinstance(p[1], astVecConst_t)
	p3_isvec = isinstance(p[3], astVecConst_t)
	p1_iscvar = (isinstance(p[1], astID_t) and var_dict.has_key(p[1].name))
	p3_iscvar = (isinstance(p[3], astID_t) and var_dict.has_key(p[3].name))
	#if (p1_isnum or p1_isvec or p1_iscvar) and (p3_isnum or p3_isvec or p3_iscvar):
	if (p1_isnum or p1_isvec) and (p3_isnum or p3_isvec):
		if p1_isnum or p1_isvec:
			val1 = p[1].val
		else:
			val1 = var_dict[p[1].name]
			if isinstance(val1, vector_t): p1_isvec = True
		if p3_isnum or p3_isvec:
			val2 = p[3].val
		else:
			val2 = var_dict[p[3].name]
			if isinstance(val2, vector_t): p3_isvec = True
		opr = p[2]
		if opr == '.' or opr == '^':
			if not (p1_isvec and p3_isvec):
				raise Exception("Expected two vectors for dot or cross operator")
			if opr == '.':
				return astNumConst_t(eval(val1.asString() + ".dot(" + val2.asString() + ')'), lexer.lineno)
			else:
				return astVecConst_t(eval(val1.asString() + ".cross(" + val2.asString() + ')'), lexer.lineno)
		if p1_isnum and p3_isvec:
			return astVecConst_t(eval("vector_t(" + str(val1) + ")" + opr + val2.asString()), lexer.lineno)
		elif p1_isvec and p3_isnum:
			return astVecConst_t(eval(val1.asString() + opr + "vector_t(" + str(val2) + ")"), lexer.lineno)
		elif p1_isvec and p3_isvec:
			return astVecConst_t(eval(val1.asString() + opr + val2.asString()), lexer.lineno)
		else:	# both num.
			return astNumConst_t(eval(str(val1) + opr + str(val2)), lexer.lineno)
	else:	# cannot evaluate directly
		return astBinop_t(p[1], p[2], p[3], lexer.lineno)

# and the same for the assignment arith.expressions for the case that rhs is a constant (num. or var.)
# eg. "a *= b" becomes "a = result", where result is the result of "a *= b"
def evalAssignExpr(p):
	p3_isnum = isinstance(p[3], astNumConst_t)
	p3_isvec = isinstance(p[3], astVecConst_t)
	p1_iscvar = (isinstance(p[1], astID_t) and var_dict.has_key(p[1].name))
	p3_iscvar = (isinstance(p[3], astID_t) and var_dict.has_key(p[3].name))
	if False: #p1_iscvar and (p3_isnum or p3_isvec or p3_iscvar):
		val1 = var_dict[p[1].name]
		if isinstance(val1, vector_t):
			p1_isvec = True
		else:
			p1_isvec = False
		if p3_isnum or p3_isvec:
			val2 = p[3].val
		else:
			val2 = var_dict[p[3].name]
			if isinstance(val2, vector_t): p3_isvec = True
		asgn, oper = '=', p[2][0]
		if p1_isvec and p3_isvec:
			return astAssignExpr_t(p[1], asgn, astVecConst_t(eval(val1.asString() + oper + val2.asString()), lexer.lineno), lexer.lineno)
		elif p1_isvec:
			return astAssignExpr_t(p[1], asgn, astVecConst_t(eval(val1.asString() + oper + "vector_t(" + str(val2) + ")"), lexer.lineno), lexer.lineno)
		else:	# both num.
			try:
				return astAssignExpr_t(p[1], asgn, astNumConst_t(eval(str(val1) + oper + str(val2)), lexer.lineno), lexer.lineno)
			except:
				# some weird error?
				print var_dict
				print val1, oper, val2
				print p[1], p[2], p[3]
				raise Exception("ERROR AT Line " + str(lexer.lineno))
	else:	# cannot evaluate directly
		# in this case lhs if id must be removed from var_dict if in var_dict since it is no longer a constant
		if p1_iscvar: var_dict.pop(p[1].name)
		return astAssignExpr_t(p[1], p[2], p[3], lexer.lineno)

# Similarly, math funcs with constant number (or vars with const.num) arguments can also be evaluated directly.
# With 3 exceptions (abs, mod & atan), all possible funcs have a direct python math module equivalent
def evalMathProc(pcname, pcargs):
	if pcname in ["sin", "cos", "tan", "asin", "acos", "atan", "exp", "sqrt", "log", "pow", "ceil", "floor", "radians", "degrees", "mod", "abs"]:
		import math
		numargs = len(pcargs)
		if numargs == 1:
			arg1 = pcargs[0]
			if isinstance(arg1, astNumConst_t):
				if pcname == "abs":
					func = "math.fabs"
				else:
					func = "math." + pcname
				return astNumConst_t(eval(func + '(' + str(arg1.val) + ')'), lexer.lineno)
# 			elif isinstance(arg1, astID_t) and var_dict.has_key(arg1.name):
# 				if pcname == "abs":
# 					func = "math.fabs"
# 				else:
# 					func = "math." + pcname
# 				return astNumConst_t(eval(func + '(' + str(var_dict[arg1.name]) + ')'), lexer.lineno)
		elif numargs == 2:	# two arg funcs (pow, log, atan & mod)
			arg1, arg2 = pcargs
			if pcname == "atan":
				func = "math.atan2"
			elif pcname == "mod":
				func = "math.fmod"
			else:
				func = "math." + pcname
			# handling constant variables is more complicated here since the two args can be both constant vars, or one can be var and other num and vice versa...
			if isinstance(arg2, astNumConst_t):
				if isinstance(arg1, astNumConst_t):
					return astNumConst_t(eval(func + '(' + str(arg1.val) + ',' + str(arg2.val) + ')'), lexer.lineno)
# 				elif isinstance(arg1, astID_t) and var_dict.has_key(arg1.name):
# 					return astNumConst_t(eval(func + '(' + str(var_dict[arg1.name]) + ',' + str(arg2.val) + ')'), lexer.lineno)
# 			elif isinstance(arg2, astID_t) and var_dict.has_key(arg2.name):
# 				if isinstance(arg1, astNumConst_t):
# 					return astNumConst_t(eval(func + '(' + str(arg1.val) + ',' + str(var_dict[arg2.name]) + ')'), lexer.lineno)
# 				elif isinstance(arg1, astID_t) and var_dict.has_key(arg1.name):
# 					return astNumConst_t(eval(func + '(' + str(var_dict[arg1.name]) + ',' + str(var_dict[arg2.name]) + ')'), lexer.lineno)

#------------------------------------------------------------

start = 'source_file'

tokens =  ['PSEUDO_LOW_PRECEDENCE', 'IDENTIFIER', 'STRINGCONST', 'FLOATCONST', 'INTCONST', 'LIGHT', 'SURFACE', 'VOLUME', 'DISPLACEMENT', 'IMAGER', 'VARYING', 'UNIFORM', 'OUTPUT', 'EXTERN', 'VOID', 'FLOAT', 'STRING', 'POINT', 'COLOR', 'VECTOR', 'NORMAL', 'MATRIX', 'IF', 'ELSE', 'BREAK', 'CONTINUE', 'RETURN', 'WHILE', 'FOR', 'SOLAR', 'ILLUMINATE', 'ILLUMINANCE', 'TEXTURE', 'ENVIRONMENT', 'SHADOW', 'PLUSEQ', 'MINUSEQ', 'MULTEQ', 'DIVEQ', 'EQ', 'NEQ', 'LEQ', 'GEQ', 'AND', 'OR', 'FILTERSTEP', 'ROTATE', 'FRESNEL']

precedence =  [('nonassoc', 'PSEUDO_LOW_PRECEDENCE') , ('nonassoc', 'IF', 'ELSE')]

# -------------- RULES ----------------

def p_source_file(p):
	'''source_file : definitions'''
	p[0] = astProgram_t(p[1], lexer.lineno)

def p_definitions(p):
	'''definitions :
	               | definitions shader_definition
	               | definitions function_definition'''
	if len(p) > 1:
		if isinstance(p[1], list):
			p[1].append(p[2])
			p[0] = p[1]
		else:
			p[0] = [p[2]]	# p[1] always undefined at first, not a list yet

def p_shader_definition(p):
	'''shader_definition : shader_type IDENTIFIER '(' opt_formals ')' body'''
	p[0] = astShaderDef_t(p[1], p[2], p[4], p[6], lexer.lineno)
	# reset the var_dict
	var_dict.clear()
	# default parameter detail for shader parameters is 'uniform'
	if p[4]:
		for f in p[4].formals:
			tps = f.typespec
			if tps[0] == None:
				tps[0] = "uniform"
			elif tps[0] == "varying" and tps[1] == "string":
				# strings are always uniform, so warn and modify if 'varying' specified
				print "WARNING, at line %d: string detail type must always be uniform" % f.lineno
				tps[0] = "uniform"

def p_function_definition(p):
	'''function_definition : opt_type IDENTIFIER '(' opt_func_formals ')' body'''
	p[0] = astFunctionDef_t(p[1], p[2], p[4], p[6], False, lexer.lineno)
	# reset the var_dict
	var_dict.clear()
	# default parameter detail for function parameters is 'varying'
	if p[4]:
		for f in p[4].formals:
			tps = f.typespec
			if tps[0] == None:
				tps[0] = "varying"
			elif tps[0] == "varying" and tps[1] == "string":
				# strings are always uniform, so warn and modify if 'varying' specified
				print "WARNING, at line %d: string detail type must always be uniform" % f.lineno
				tps[0] = "uniform"

# opt_externspec & typespec only seem to be here to solve shift/reduce conflicts, otherwise it should be exactly the same as function_definition above
def p_lexical_function_definition(p):
	'''lexical_function_definition : opt_externspec typespec IDENTIFIER '(' opt_func_formals ')' body'''
	p[0] = astFunctionDef_t(p[2], p[3], p[5], p[7], True, lexer.lineno)
	# constant variables assigned to while in the function are not valid and will cause errors when assigning after it, so clear the var_dict
	var_dict.clear()
	# default parameter detail for function parameters is 'varying'
	if p[5]:
		for f in p[5].formals:
			tps = f.typespec
			if tps[0] == None:
				tps[0] = "varying"
			elif tps[0] == "varying" and tps[1] == "string":
				# strings are always uniform, so warn and modify if 'varying' specified
				print "WARNING, at line %d: string detail type must always be uniform" % f.lineno
				tps[0] = "uniform"

def p_body(p):
	'''body : '{' statements '}' '''
	p[0] = astStatements_t(p[2], lexer.lineno)

def p_shader_type(p):
	'''shader_type : LIGHT
	               | SURFACE
	               | VOLUME
	               | DISPLACEMENT
	               | IMAGER'''
	p[0] = p[1]

# sets flag of the parameters default expressions, and removes the params from the var_dict
def setParamFlag(formals, pname):
	global var_dict
	shader = (pname == "shparam")
	varid = {}
	for param in formals:
		for dexpr in param.exprs:
			if shader:	# shader parameters *must* have a default value assigned to them
				if dexpr.def_init == None:
					raise Exception("SYNTAX ERROR, at line %d: shader parameter '%s' has no default value" % (lexer.lineno, dexpr.id))
			else:	# function parameters however *cannot*
				if dexpr.def_init != None:
					raise Exception("SYNTAX ERROR, at line %d: function parameter '%s', cannot assign default value" % (lexer.lineno, dexpr.id))
			exec("dexpr.%s = True" % pname)
			if shader and var_dict.has_key(dexpr.id): var_dict.pop(dexpr.id)

# original opt_formals split into p_opt_formals() for shader parameters (to allow direct eval of constant values), and p_opt_func_formals() for function parameters
def p_opt_formals(p):
	'''opt_formals :
	               | formals
	               | formals ';' '''
	if len(p) > 1:
		p[0] = astFormals_t(p[1], lexer.lineno)
		# Shader parameters now known, set the 'shparam' flag in the def_expression list of each.
		# Also remove any parameters from var_dict, since parameters can only be evaluated directly when they still have default values
		setParamFlag(p[1], "shparam")

def p_opt_func_formals(p):
	'''opt_func_formals :
	                    | formals
	                    | formals ';' '''
	if len(p) > 1:
		p[0] = astFormals_t(p[1], lexer.lineno)
		# Function parameters now known, set the 'funcparam' flag in the def_expression list of each.
		setParamFlag(p[1], "funcparam")

def p_formals(p):
	'''formals : formal_variable_definitions
	           | formals ';' formal_variable_definitions '''
	if len(p) == 2:
		p[0] = [p[1]]
	else:
		p[1].append(p[3])
		p[0] = p[1]

def p_formal_variable_definitions(p):
	'''formal_variable_definitions : opt_outputspec typespec def_expressions'''
	p[0] = astParameter_t(p[1], p[2], p[3], lexer.lineno)

def p_variable_definitions(p):
	'''variable_definitions : opt_externspec typespec def_expressions'''
	# default variable detail if not specified is 'varying', UNLESS it's a string
	if p[2][0] == None:
		if p[2][1] == "string":
			p[2][0] = "uniform"
		else:
			p[2][0] = "varying"
	elif p[2][0] == "varying" and p[2][1] == "string":
		# strings are always uniform, so warn and modify if 'varying' specified
		print "[WARNING] line %d: string detail type must always be uniform" % lexer.lineno
		p[2][0] = "uniform"
	# remove extern declared variables inside functions from the vardict,
	# if the variable is going to be modified, it's no longer constant, so cannot be used for direct eval.
	# (It's possible that the var is actually only read, not modified, but that can't be determined at this stage.
	#  Well, it might be possible, but better would be a separate 'simplify' stage after parse)
	if p[1]:
		for dexpr in p[3]:
			if var_dict.has_key(dexpr.id):
				var_dict.pop(dexpr.id)
	p[0] = astVariable_t(p[1], p[2], p[3], lexer.lineno)

def p_typespec(p):
	'''typespec : opt_detail type'''
	p[0] = [p[1], p[2]]	# !!!

def p_def_expressions(p):
	'''def_expressions : def_expression
	                   | def_expressions ',' def_expression'''
	if len(p) == 2:
		p[0] = [p[1]]
	else:
		p[1].append(p[3])
		p[0] = p[1]

def p_def_expression(p):
	'''def_expression : IDENTIFIER
	                  | IDENTIFIER '=' expression
	                  | IDENTIFIER '[' INTCONST ']'
	                  | IDENTIFIER '[' INTCONST ']' '=' array_init '''
	plen = len(p)
	if plen == 2:
		p[0] = astDefExpression_t(p[1], None, None, lexer.lineno)
	elif p[2] == '=':
		p[0] = astDefExpression_t(p[1], None, p[3], lexer.lineno)
		# keep track of any variable with a constant value, needed for possible direct evaluation
		if isinstance(p[3], astNumConst_t) or isinstance(p[3], astVecConst_t):
			var_dict[p[1]] = p[3].val
	elif plen == 5:
		p[0] = astDefExpression_t(p[1], p[3], None, lexer.lineno)
	else:
		p[0] = astDefExpression_t(p[1], p[3], p[6], lexer.lineno)

def p_array_expr_list(p):
	'''array_expr_list : expression
	                    | array_expr_list ',' expression'''
	if len(p) == 2:
		if isinstance(p[1], astNumConst_t):
			p[0] = [True, [p[1].val]]
		else:
			p[0] = [False, [p[1]]]
	else:
		if isinstance(p[3], astNumConst_t):
			p[1][1].append(p[3].val)
		else:
			p[1][0] = False
			p[1][1].append(p[3])
		p[0] = p[1]

def p_array_init(p):
	'''array_init : '{' array_expr_list '}' '''
	p[0] = array_t(p[2][0], p[2][1])

def p_opt_detail(p):
	'''opt_detail :
	              | VARYING
	              | UNIFORM'''
	if len(p) > 1: p[0] = p[1]

def p_opt_outputspec(p):
	'''opt_outputspec :
	                  | OUTPUT'''
	if len(p) > 1: p[0] = p[1]

def p_opt_externspec(p):
	'''opt_externspec :
	                  | EXTERN'''
	if len(p) > 1: p[0] = p[1]

def p_opt_type(p):
	'''opt_type :
	            | type'''
	if len(p) > 1: p[0] = p[1]

def p_type(p):
	'''type : FLOAT
	        | STRING
	        | COLOR
	        | POINT
	        | VECTOR
	        | NORMAL
	        | MATRIX
	        | VOID'''
	p[0] = p[1]

def p_statements(p):
	'''statements :
	              | statements statement
	              | statements variable_definitions ';'
	              | statements lexical_function_definition'''
	if len(p) > 1:
		if isinstance(p[1], list):
			p[1].append(p[2])
			p[0] = p[1]
		else:
			p[0] = [p[2]]	# p[1] always undefined at first, not a list yet

def p_statement_1(p):
	'''statement : assignexpr ';' '''
	p[0] = p[1]

def p_statement_2(p):
	'''statement : procedurecall ';' '''
	p[0] = p[1]

def p_statement_3(p):
	'''statement : RETURN expression ';' '''
	p[0] = astReturn_t(p[2], lexer.lineno)

def p_statement_4(p):
	'''statement : BREAK opt_integer ';' '''
	p[0] = astBreak_t(p[2], lexer.lineno)

def p_statement_5(p):
	'''statement : CONTINUE opt_integer ';' '''
	p[0] = astContinue_t(p[2], lexer.lineno)

def p_statement_6(p):
	'''statement : IF '(' relation ')' statement %prec PSEUDO_LOW_PRECEDENCE'''
	p[0] = astIfElse_t(p[3], p[5], None, False, lexer.lineno)

def p_statement_7(p):
	'''statement : IF '(' relation  ')' statement ELSE statement'''
	p[0] = astIfElse_t(p[3], p[5], p[7], False, lexer.lineno)

def p_statement_8(p):
	'''statement : WHILE '(' relation ')' statement'''
	p[0] = astFor_t(None, p[3], None, p[5], lexer.lineno)

def p_statement_9(p):
	'''statement : FOR '(' expression ';' relation ';' expression ')' statement'''
	p[0] = astFor_t(p[3], p[5], p[7], p[9], lexer.lineno)

def p_statement_10(p):
	'''statement : SOLAR '(' ')' statement'''
	p[0] = astSolar_t(None, None, p[4], lexer.lineno)

def p_statement_11(p):
	'''statement : SOLAR '(' expression ',' expression ')' statement'''
	p[0] = astSolar_t(p[3], p[5], p[7], lexer.lineno)

def p_statement_12(p):
	'''statement : ILLUMINATE '(' expression ')' statement'''
	p[0] = astIlluminate_t(p[3], None, None, p[5], lexer.lineno)

def p_statement_13(p):
	'''statement : ILLUMINATE '(' expression ',' expression ',' expression ')' statement'''
	p[0] = astIlluminate_t(p[3], p[5], p[7], p[9], lexer.lineno)

def p_statement_14(p):
	'''statement : ILLUMINANCE '(' expression ')' statement'''
	p[0] = astIlluminance_t(None, p[3], None, None, p[5], lexer.lineno)

def p_statement_15(p):
	'''statement : ILLUMINANCE '(' expression ',' expression ',' expression ')' statement'''
	p[0] = astIlluminance_t(None, p[3], p[5], p[7], p[9], lexer.lineno)

def p_statement_16(p):
	'''statement : ILLUMINANCE '(' STRINGCONST ',' expression ')' statement'''
	p[0] = astIlluminance_t(p[3], p[5], None, None, p[7], lexer.lineno)

def p_statement_17(p):
	'''statement : ILLUMINANCE '(' STRINGCONST ',' expression ',' expression ',' expression ')' statement'''
	p[0] = astIlluminance_t(p[3], p[5], p[7], p[9], p[11], lexer.lineno)

def p_statement_18(p):
	'''statement : '{' statements '}' opt_semicol '''
	# only need node to indicate that scope changes at this point
	p[0] = astNewScope_t(astStatements_t(p[2], lexer.lineno), lexer.lineno)
	# reset var_dict
	var_dict.clear()

# doesn't do anything, only needed when macros are used with braced statement that have semicolon at end of line
def p_opt_semicol(p):
	'''opt_semicol :
	               | ';' '''
	pass

#-------------------------------------------------------------------------------------------------------------
# extra statement rules to simplify handling of a few procedures

def p_statement_19(p):
	'''statement : ROTATE '(' expression ',' expression ',' expression ')' ';'
	             | ROTATE '(' expression ',' expression ',' expression ',' expression ')' ';' '''
	if len(p) == 9:
		p[0] = astRotate_t(p[3], p[5], p[7], None, lexer.lineno)
	else:
		p[0] = astRotate_t(p[3], p[5], p[7], p[9], lexer.lineno)

def p_statement_20(p):
	'''statement : FRESNEL '(' expression ',' expression ',' expression ',' expression ',' expression ')' ';'
	             | FRESNEL '(' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ')' ';' '''
	if len(p) == 14:
		p[0] = astFresnel_t(p[3], p[5], p[7], p[9], p[11], None, None, lexer.lineno)
	else:
		p[0] = astFresnel_t(p[3], p[5], p[7], p[9], p[11], p[13], p[15], lexer.lineno)

# filterstep handled separately as well because of optional parameterlist
def p_filterstep(p):
	'''filterstep : FILTERSTEP '(' expression ',' expression opt_paramlist ')'
	              | FILTERSTEP '(' expression ',' expression ',' expression opt_paramlist ')' '''
	if len(p) == 8:
		p[0] = astFilterstep_t(p[3], p[5], None, p[6], lexer.lineno);
	else:
		p[0] = astFilterstep_t(p[3], p[5], p[7], p[8], lexer.lineno)

def p_opt_paramlist(p):
	'''opt_paramlist :
	                 | param
	                 | opt_paramlist param'''
	lp = len(p)
	if lp > 1:
		if lp == 2:
			p[0] = [p[1]]
		else:
			p[1].append(p[2])
			p[0] = p[1]

def p_param(p):
	'''param : ',' STRINGCONST ',' expression'''
	p[0] = [p[2], p[4]]

#-------------------------------------------------------------------------------------------------------------

def p_opt_integer(p):
	'''opt_integer :
	               | INTCONST'''
	if len(p) > 1: p[0] = p[1]

def p_primary_1(p):
	'''primary : INTCONST'''
	p[0] = astNumConst_t(p[1], lexer.lineno)

def p_primary_2(p):
	'''primary : FLOATCONST'''
	p[0] = astNumConst_t(p[1], lexer.lineno)

def p_primary_3(p):
	'''primary : texture
	           | filterstep '''
	p[0] = p[1]

def p_primary_4(p):
	'''primary : IDENTIFIER'''
	if p[1] == "PI":
		p[0] = astNumConst_t(3.1415926535897932384626433832795029, lexer.lineno)
	else:
		p[0] = astID_t(p[1], None, lexer.lineno)

def p_primary_5(p):
	'''primary : IDENTIFIER '[' expression ']' '''
	p[0] = astID_t(p[1], p[3], lexer.lineno)

def p_primary_6(p):
	'''primary : STRINGCONST'''
	p[0] = astStringConst_t(p[1], lexer.lineno)

def p_primary_7(p):
	'''primary : procedurecall'''
	p[0] = p[1]

def p_primary_8(p):
	'''primary : '(' expression ')' '''
	p[0] = p[2]

def p_primary_9(p):
	'''primary : '(' expression ',' expression ',' expression ')'
             | '(' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ',' expression ')' '''
	const = True
	for i in range(2, len(p)-1, 2):
		const &= isinstance(p[i], astNumConst_t)
	if const:
		tp = []
		for i in range(2, len(p)-1, 2):
			tp.append(p[i].val)
		if len(tp) == 3:
			p[0] = astVecConst_t(vector_t(*tp), lexer.lineno)
		else:
			p[0] = astMtxConst_t(matrix_t(*tp), lexer.lineno)
		return
	t = []
	for i in range(2, len(p)-1, 2):
		t.append(p[i])
	p[0] = astTuple_t(t, lexer.lineno)

def p_unary_expr(p):
	'''unary_expr : primary
	              | '-' unary_expr
	              | typecast unary_expr'''
	if len(p) == 2:
		p[0] = p[1]
	elif p[1] == '-':
		if isinstance(p[2], astNumConst_t):
			p[2].val = -p[2].val
			p[2].children = -p[2].children	# completely unnecessary, just for tree printing
			p[0] = p[2]
		else:
			p[0] = astUnaryExpr_t(p[1], p[2], lexer.lineno)
	else:
		p[0] = astUnaryExpr_t(p[1], p[2], lexer.lineno)

def p_dot_expr(p):
	'''dot_expr : unary_expr
	            | dot_expr '.' unary_expr'''
	if len(p) == 2:
		p[0] = p[1]
	else:
		#p[0] = astBinop_t(p[1], p[2], p[3], lexer.lineno)
		p[0] = evalBinop(p)

def p_multiplicative_expr(p):
	'''multiplicative_expr : dot_expr
	                       | multiplicative_expr '*' dot_expr
	                       | multiplicative_expr '/' dot_expr'''
	if len(p) == 2:
		p[0] = p[1]
	else:
		#p[0] = astBinop_t(p[1], p[2], p[3], lexer.lineno)
		p[0] = evalBinop(p)

def p_cross_expr(p):
	'''cross_expr : multiplicative_expr
	              | cross_expr '^' multiplicative_expr'''
	if len(p) == 2:
		p[0] = p[1]
	else:
		#p[0] = astBinop_t(p[1], p[2], p[3], lexer.lineno)
		p[0] = evalBinop(p)

def p_additive_expr(p):
	'''additive_expr : cross_expr
	                 | additive_expr '+' cross_expr
	                 | additive_expr '-' cross_expr'''
	if len(p) == 2:
		p[0] = p[1]
	else:
		#p[0] = astBinop_t(p[1], p[2], p[3], lexer.lineno)
		p[0] = evalBinop(p)

def p_conditional_expr(p):
	'''conditional_expr : additive_expr
	                    | relation '?' additive_expr ':' conditional_expr'''
	if len(p) == 2:
		p[0] = p[1]
	else:
		p[0] = astIfElse_t(p[1], p[3], p[5], True, lexer.lineno)

def p_assignexpr(p):
	'''assignexpr : lhs asgnop assign_expr'''
	# skip direct evaluation here, see comments in evalAssignExpr() above
	p[0] = evalAssignExpr(p)
	#p[0] = astAssignExpr_t(p[1], p[2], p[3], lexer.lineno)

def p_asgnop(p):
	'''asgnop : '='
	          | PLUSEQ
	          | MINUSEQ
	          | MULTEQ
	          | DIVEQ'''
	p[0] = p[1]

def p_lhs(p):
	'''lhs : IDENTIFIER
	       | IDENTIFIER '[' expression ']' '''
	if len(p) == 2:
		p[0] = astID_t(p[1], None, lexer.lineno)
	else:
		p[0] = astID_t(p[1], p[3], lexer.lineno)

def p_assign_expr(p):
	'''assign_expr : conditional_expr
	               | assignexpr'''
	p[0] = p[1]

def p_expression(p):
	'''expression : assign_expr'''
	p[0] = p[1]

def p_relation_1(p):
	'''relation : relation2 OR relation'''
	p[0] = astRelation_t(p[1], p[2], p[3], lexer.lineno)

def p_relation_2(p):
	'''relation : relation2'''
	p[0] = p[1]

def p_relation2_1(p):
	'''relation2 : relation3 AND relation2'''
	p[0] = astRelation_t(p[1], p[2], p[3], lexer.lineno)

def p_relation2_2(p):
	'''relation2 : relation3'''
	p[0] = p[1]

def p_relation3_1(p):
	'''relation3 : '!' relation3'''
	p[0] = astRelation_t(None, p[1], p[2], lexer.lineno)

def p_relation3_2(p):
	'''relation3 : relation4'''
	p[0] = p[1]

def p_relation4_1(p):
	'''relation4 : '(' relation ')' '''
	p[0] = p[2]

def p_relation4_2(p):
	'''relation4 : additive_expr relop additive_expr'''
	p[0] = astRelation_t(p[1], p[2], p[3], lexer.lineno)

def p_relop(p):
	'''relop : '>'
	         | GEQ
	         | '<'
	         | LEQ
	         | EQ
	         | NEQ'''
	p[0] = p[1]

def p_procedurecall(p):
	'''procedurecall : IDENTIFIER '(' opt_proc_arguments ')' '''
	direval = evalMathProc(p[1], p[3])
	if direval:
		p[0] = direval
	else:
		p[0] = astProcedureCall_t(p[1], p[3], lexer.lineno)

def p_typecast(p):
	'''typecast : FLOAT
	            | STRING
	            | COLOR opt_spacetype
	            | POINT opt_spacetype
	            | VECTOR opt_spacetype
	            | NORMAL opt_spacetype
	            | MATRIX opt_spacetype'''
	if len(p) == 2:
		p[0] = [p[1], None]
	else:
		p[0] = [p[1], p[2]]

def p_opt_spacetype(p):
	'''opt_spacetype :
	                 | STRINGCONST'''
	if len(p) > 1: p[0] = astStringConst_t(p[1], lexer.lineno)

def p_opt_proc_arguments(p):
	'''opt_proc_arguments :
	                      | proc_arguments'''
	if len(p) > 1: p[0] = p[1]

def p_proc_arguments(p):
	'''proc_arguments : expression
	                  | proc_arguments ',' expression'''
	if len(p) == 2:
		p[0] = [p[1]]
	else:
		p[1].append(p[3])
		p[0] = p[1]

def p_texture(p):
	'''texture : texture_type '(' texture_filename_with_opt_channel opt_texture_arguments ')' '''
	p[0] = astTexture_t(p[1], p[3], p[4], lexer.lineno)

def p_texture_type(p):
	'''texture_type : TEXTURE
	                | ENVIRONMENT
	                | SHADOW'''
	p[0] = p[1]

def p_texture_filename_with_opt_channel_1(p):
	'''texture_filename_with_opt_channel : IDENTIFIER '[' expression ']' '[' INTCONST ']' '''
	p[0] = astTexFileChan_t(p[1], p[3], p[6], lexer.lineno)

def p_texture_filename_with_opt_channel_2(p):
	'''texture_filename_with_opt_channel : STRINGCONST '[' expression ']' '''
	p[0] = astTexFileChan_t(p[1], p[3], None, lexer.lineno)

def p_texture_filename_with_opt_channel_3(p):
	'''texture_filename_with_opt_channel : IDENTIFIER '[' expression ']' '''
	p[0] = astTexFileChan_t(p[1], p[3], None, lexer.lineno)

def p_texture_filename_with_opt_channel_4(p):
	'''texture_filename_with_opt_channel : IDENTIFIER'''
	p[0] = astTexFileChan_t(p[1], None, None, lexer.lineno)

def p_texture_filename_with_opt_channel_5(p):
	'''texture_filename_with_opt_channel : STRINGCONST'''
	p[0] = astTexFileChan_t(p[1], None, None, lexer.lineno)

def p_opt_texture_arguments(p):
	'''opt_texture_arguments :
	                         | ',' proc_arguments'''
	if len(p) > 1: p[0] = astTexArgs_t(p[2], lexer.lineno)

#### Catastrophic error handler
def p_error(p):
	global parse_error
	if p:
		print "SYNTAX ERROR AT LINE %d BEFORE TOKEN: '%s'" % (p.lineno, p.value)
		parse_error = True
		yacc.errok()
	else:
		print "SYNTAX ERROR AT EOF"

# -------------- RULES END ----------------

from ply import *
from qdlex import *
slparser = yacc.yacc(optimize=0, debug=1)

def slparse(data):
	slparser.error = 0
	p = slparser.parse(data)
	if slparser.error: return None
	return p

# returns tuple of the entire ast for PrettyPrinter
def makelist(ast):
	sl = [type(ast).__name__]
	if not isinstance(ast.children, list):	# if not list, node is leaf
		if isinstance(ast.children, astnode_t):
			sl.append(makelist(ast.children))
		else:
			sl.append(ast.children)
	elif len(ast.children):
		for n in ast.children:
			if isinstance(n, astnode_t):
				sl.append(makelist(n))
			elif isinstance(n, list):
				for n2 in n:
					if isinstance(n2, astnode_t):
						sl.append(makelist(n2))
					else:
						sl.append(n2)
			else:
				sl.append(n)
	return tuple(sl)

if __name__ == '__main__':
	argc = len(sys.argv)
	if argc == 1:
		print "Usage: %s [options] inputfile" % sys.argv[0]
		print "possible options:"
		print "-O optimize code (currently braindead optimization, don't expect miracles ;)"
		print "-S simplify register names"
		print "-P print original source and 'pretty print' the resulting AST"
	else:
		optimize, simplify, dopp = False, False, False
		if argc > 2:
			for arg in sys.argv[1:]:
				if arg[0] == '-':
					if arg[1] == 'O':
						optimize = True
					elif arg[1] == 'S':
						simplify = True
					elif arg[1] == 'P':
						dopp = True
				else:	# filename, the end
					fname = arg
					break
		else:
			fname = sys.argv[1]
		print "OPTIMIZE    :", optimize
		print "SIMPLIFY    :", simplify
		print "PRETTYPRINT :", dopp
		print "filename    : '%s'\n" % fname
		# try cpp pass
		import os
		tempname = fname + "_tmp"
		try:
			os.system("cpp %s %s" % (fname, tempname))
		except:
			print "[WARNING]: could not execute preprocessor, program might not compile!"
		else:
			fname = tempname
		# read preprocessed file and compile
		try:
			cppfile = open(tempname, 'r')
		except IOError:
			print "Could not open file"
		except:
			raise Exception()
		else:
			data = cppfile.read()
			cppfile.close()
			if dopp:
				print data, '\n'
			ast = slparse(data)
			if ast and not parse_error:
				if dopp:	# 'pretty print' the AST
					print
					import pprint
					pp = pprint.PrettyPrinter()
					pp.pprint(makelist(ast))
				#print "\nsimplify\n"
				#astsimplify_t().visit(ast)
				#if dopp:	# 'pretty print' the AST
				#	pp.pprint(makelist(ast))
				print "\nbuild\n"
				compile_t(optimize, simplify).visit(ast)
			else:
				print "Error(s) occured"
			print "\nDone"
			os.remove(tempname)
