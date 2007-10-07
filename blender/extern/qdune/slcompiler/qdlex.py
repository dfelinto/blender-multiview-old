#--------------------------------------------------------
# All this based on ANSIC lexer, modified to lex RSL
#--------------------------------------------------------

import sys
sys.path.insert(0, "../..")

import ply.lex as lex

# Reserved words
reserved = ('LIGHT', 'SURFACE', 'VOLUME', 'DISPLACEMENT', 'IMAGER', 'EXTERN',
            'OUTPUT', 'VARYING', 'UNIFORM', 'VOID', 'FLOAT', 'STRING', 'POINT',
            'VECTOR', 'NORMAL', 'COLOR', 'MATRIX', 'IF', 'ELSE', 'BREAK',
            'CONTINUE', 'RETURN', 'WHILE', 'FOR', 'SOLAR', 'ILLUMINATE',
            'ILLUMINANCE', 'TEXTURE', 'ENVIRONMENT', 'SHADOW',
            'FILTERSTEP', 'ROTATE', 'FRESNEL')

tokens = reserved + (
	# Literals (identifier, integer constant, float constant, string constant)
	'IDENTIFIER', 'INTCONST', 'FLOATCONST', 'STRINGCONST',
	# two char operators (||, &&, <=, >=, ==, !=)
	'OR', 'AND', 'LEQ', 'GEQ', 'EQ', 'NEQ',
	# Assignment (*=, /=, +=, -=)
	'MULTEQ', 'DIVEQ', 'PLUSEQ', 'MINUSEQ'
	)

literals = ['(', ')', '[', ']', '{', '}', ';', ',', '=', '-', '.', '*', '/', '^', '+', '?', ':', '!', '>', '<']

# Completely ignored characters
t_ignore           = ' \t\r'

# Newlines
def t_NEWLINE(t):
	r'\n+'
	t.lexer.lineno += t.value.count('\n')

# two char operators
t_OR               = r'\|\|'
t_AND              = r'&&'
t_LEQ              = r'<='
t_GEQ              = r'>='
t_EQ               = r'=='
t_NEQ              = r'!='

# Assignment operators
t_MULTEQ           = r'\*='
t_DIVEQ            = r'/='
t_PLUSEQ           = r'\+='
t_MINUSEQ          = r'-='

# Identifiers and reserved words
reserved_map = { }
for r in reserved:
	reserved_map[r.lower()] = r

def t_IDENTIFIER(t):
	r'[A-Za-z_][\w_]*'
	t.type = reserved_map.get(t.value, "IDENTIFIER")
	return t

# Floating literal, functions instead of string, must try float first
def t_FLOATCONST(t):
	r'((((\d+)(\.\d*))|((\d*)(\.\d+)))([eE](\+|-)?(\d+))?|(\d+)[eE](\+|-)?(\d+))'
	t.value = float(t.value)
	return t

# Integer literal
def t_INTCONST(t):
	r'\d+'
	t.value = int(t.value)
	return t

# String literal
def t_STRINGCONST(t):
	r'\"([^\\\n]|(\\.))*?\"'
	t.value = str(t.value)
	t.lexer.lineno += t.value.count('\n')
	return t

# comments
def t_comment(t):
	r'(/\*(.|\n)*?\*/)|(//.*\n)'
	t.lexer.lineno += t.value.count('\n')

# Preprocessor directive (ignored for now)
def t_preprocessor(t):
	r'\#(.)*?\n'
	t.lexer.lineno += 1

def t_error(t):
	print "Illegal character %s" % repr(t.value[0])
	t.lexer.skip(1)

lexer = lex.lex(optimize = 1)
if __name__ == "__main__":
	data = open(sys.argv[1]).read()
	lexer.input(data)
	while True:
		tok = lexer.token()
		if not tok: break
		print tok
