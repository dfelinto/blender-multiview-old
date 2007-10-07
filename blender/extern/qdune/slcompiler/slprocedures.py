# Dictionary of most current possible procedure calls (some are handled by the parser itself), a mapping to the associated VM instruction,
# The very first value of the list is the general return typechar, independent of the instruction.
# If this is not known in advance because the procedure can have different argument types, then the char is 'T' (for 'Template function'),
# in which case the instruction name is not complete either, it will get the return type appended to its name ( eg. func min(float, float, ..) -> instruction minf )
# After that is the list of possible instructions, which consists of the instruction name and its argument type string.
# Return type can be any of '*fvsm' for float, vector, string, or matrix respectively, 'v' (vector) can also generally be used anywhere 'cpn' can be used (color, point or normal),
# For the return type, the character '*' means there is no return value (void).
# '*' is used in the argument typestring as well, but then it means the arguments are not known in advance (variadic function).
# (Reason the asterisk character is used instead of v or V, is because lowercase v designates vector type and uppercase V is reserved for an 'output' vector in user-defined functions)

slprocs = { "radians": ['f', ["radians", "f"]],
						"degrees": ['f', ["degrees", "f"]],
						"sin": ['f', ["sin", "f"]],
						"asin": ['f', ["asin", "f"]],
						"cos": ['f', ["cos","f"]],
						"acos": ['f', ["acos", "f"]],
						"tan": ['f', ["tan", "f"]],
						"atan": ['f', ["atan1", "f"], ["atan2", "ff"]],
						"pow": ['f', ["pow", "ff"]],
						"exp": ['f', ["exp", "f"]],
						"sqrt": ['f', ["sqrt", "f"]],
						"inversesqrt": ['f', ["invsqrt", "f"]],
						"log": ['f', ["log", "f"], ["logb", "ff"]],
						"mod": ['f', ["mod", "ff"]],
						"abs": ['f', ["abs", "f"]],
						"sign": ['f', ["sign", "f"]],
						"min": ['T', ["min", "T*"]],
						"max": ['T', ["max", "T*"]],
						"clamp": ['T', ["clamp", "TTT"]],
						"mix": ['T', ["mix", "TTT"]],
						"floor": ['f', ["floor", "f"]],
						"ceil": ['f', ["ceil", "f"]],
						"round": ['f', ["round", "f"]],
						"step": ['f', ["step", "ff"]],
						"smoothstep": ['f', ["smoothstep", "fff"]],
						# filterstep() handled by parser
						"spline": ['T', ["spline1", "f*"], ["spline2", "sf*"]],
						"Du": ['T', ["Du", "T"]],
						"Dv": ['T', ["Dv", "T"]],
						"Deriv": ['T', ["deriv", "Tf"]],
						"random": ['T', ["random", "T"]],
						"xcomp": ['f', ["xcomp", "v"]],
						"ycomp": ['f', ["ycomp", "v"]],
						"zcomp": ['f', ["zcomp", "v"]],
						"setxcomp": ['*', ["setxcomp", "vf"]],
						"setycomp": ['*', ["setycomp", "vf"]],
						"setzcomp": ['*', ["setzcomp", "vf"]],
						"length": ['f', ["length", "v"]],
						"normalize": ['n', ["normalize", "n"]],
						"distance": ['f', ["distance", "vv"]],
						"ptlined": ['f', ["ptlined", "vvv"]],
						# rotate() handled by parser
						"area": ['f', ["area", "p"]],
						"faceforward": ['v', ["faceforward1", "vv"], ["faceforward2", "vvv"]],
						"reflect": ['v', ["reflect", "vv"]],
						"refract": ['v', ["refract", "vvf"]],
						# fresnel() handled by parser
						# transforms (to make things easier for the programmer, could always just use transform() and let the compiler decide which transform to use based on types)
						"transform": ['p', ["ptransform1", "sp"], ["ptransform2", "ssp"], ["ptransform3", "mp"], ["ptransform4", "smp"]],
						"vtransform": ['v', ["vtransform1", "sv"], ["vtransform2", "ssv"], ["vtransform3", "mv"], ["vtransform4", "smv"]],
						"ntransform": ['n', ["ntransform1", "sn"], ["ntransform2", "ssn"], ["ntransform3", "mn"], ["ntransform4", "smn"]],
						"depth": ['f', ["depth", "p"]],
						"calculatenormal": ['n', ["calcnormal", "p"]],
						"comp": ['f', ["compv", "vf"], ["compm", "mff"]],
						"setcomp": ['*', ["setcompv", "vff"], ["setcompm", "mfff"]],
						"ctransform": ['c', ["ctransform1", "sc"], ["ctransform2", "ssc"]],
						"determinant": ['f', ["determinant", "m"]],
						"translate": ['m', ["translate", "mv"]],
						"scale": ['m', ["scale", "mv"]],
						"concat": ['s', ["concat", "s*"]],
						"printf": ['*', ["printf", "s*"]],
						"format": ['s', ["format", "s*"]],
						"match": ['f', ["match", "ss"]],
						"ambient": ['v', ["ambient", ""]],
						"diffuse": ['v', ["diffuse", "v"]],
						"specular": ['v', ["specular", "vvf"]],
						"specularbrdf": ['v', ["specularbrdf", "vvvf"]],
						"phong": ['v', ["phong", "vvf"]],
						"trace": ['v', ["trace", "vv"]],
						# texture/environment/shadow handled by parser
						# message passing funcs, currently only floats (none of them are in fact even implemented at this point... so will cause problems if used in a shader  TODO)
 						"lightsource": []
# 						"textureinfo": ['f', ["texinfof", "ssf"]],
# 						"atmosphere": ['f', ["atmospheref", "sf"]],
# 						"displacement": ['f', ["displacement", "sf"]],
# 						"lightsource": ['f', ["lightsource", "sf"]],
# 						"surface": ['f', ["surface", "sf"]],
# 						"incident": ['f', ["incident", "sf"]],
# 						"opposite": ['f', ["opposite", "sf"]],
# 						"attribute": ['f', ["attribute", "sf"]],
# 						"option": ['f', ["option", "sf"]],
# 						"rendererinfo": ['f', ["rndrinfo", "sf"]],
# 						"shadername": ['f', ["shadername1", ""], ["shadername2", "s"]]
						}

# These functions require a cast to the required return type (eg. color c = color noise(P); )
slcastprocs = {
						"noise": ['T', ["noise1", "f"], ["noise2", "ff"], ["noise3", "v"], ["noise4", "vf"]],
						"pnoise": ['T', ["pnoise1", "ff"], ["pnoise2", "ffff"], ["pnoise3", "vv"], ["pnoise4", "vfvf"]],
						"cellnoise": ['T', ["cellnoise1", "f"], ["cellnoise2", "ff"], ["cellnoise3", "v"], ["cellnoise4", "vf"]]
}