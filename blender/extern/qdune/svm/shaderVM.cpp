//------------------------------------------------------------------------------
// The Shader Virtual Machine
//------------------------------------------------------------------------------

#include "shaderVM.h"
#include "Mathutil.h"
#include "MicroPolygonGrid.h"
#include "Primitive.h"
#include "qdtl.h"
#include "Noise.h"
#include "ImageTextures.h"
#include "State.h"
#include <cassert>
#include <cstdio>

__BEGIN_QDRENDER

// NOTE in the PR.book the example 'add' shadeop (on which basically all of the code here is based)
// for the case that all operands are uniform it will test the acitivity of processor 0 before executing the operation.
// This doesn't seem to make much sense, since after all, code that uses varying variables
// before this statement may have disabled proc.0 while others are still active,
// and so the uniform variable when it is then used again in any operation which uses
// varying variables will give completely wrong results.
// So instead, I added a some_active() function to the SlRunContext class which tests if
// *any* processor is active, if so, it executes the uniform code.
// So only in the case that all are disabled will the statement not be executed.
// Of course, it could be once again that I misunderstand some things or do something fundamentally wrong.
// Another possible solution for instance is that if the uniform is to be used with varyings,
// the lhs of any instruction must also be varying even when all operands are uniform,
// which would solve the problem too of course, though maybe a bit more complex for the compiler.
// The some_active() solution would seem to be more 'foolproof' too, when for instance used with handwritten assembler shaders...
// (but who would do that anyway... ;)
// (possibly TODO optimize some_active() by boolarray?)

//------------------------------------------------------------------------------
// Macros for the most common operations

// this is for the simple math float ops (FFF) where the only difference
// is the actual binary operator ( + - / * )
// float1 = float2 BINOP float3
#define OP_BINOP(NAME, BINOP)\
RtVoid NAME(SlRunContext& context)\
{\
	RtFloat* lhs = context.getFloat(1);\
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);\
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)\
			if (context.active(i))\
				lhs[i] = op1[op1o] BINOP op2[op2o];\
	}\
	else {\
		assert((op1i | op2i) == 0);\
		if (context.some_active())\
			lhs[0] = op1[0] BINOP op2[0];\
	}\
	context.nextInstruction();\
}

// for function calls with two arguments and no return value
// FUNC(T1, T2)
#define OP_FUNC2(NAME, T1, T2, FUNC)\
RtVoid NAME(SlRunContext& context)\
{\
	Rt##T1* lhs = context.get##T1(1);\
	const Rt##T2* op1 = context.get##T2(2);\
	const int op1i = context.isVarying(2);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i) {\
			if (context.active(i))\
				FUNC(lhs[i], op1[op1o]);\
		}\
	}\
	else if (context.some_active()) {\
		assert(op1i == 0);\
		FUNC(lhs[0], op1[0]);\
	}\
	context.nextInstruction();\
}

// as above, three arguments
// FUNC(T1, T2, T3)
#define OP_FUNC3(NAME, T1, T2, T3, FUNC)\
RtVoid NAME(SlRunContext& context)\
{\
	Rt##T1* lhs = context.get##T1(1);\
	const Rt##T2* op1 = context.get##T2(2);\
	const Rt##T3* op2 = context.get##T3(3);\
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)\
			if (context.active(i))\
				FUNC(lhs[i], op1[op1o], op2[op2o]);\
	}\
	else if (context.some_active()) {\
		assert((op1i | op2i) == 0);\
		FUNC(lhs[0], op1[0], op2[0]);\
	}\
	context.nextInstruction();\
}

// as above, four arguments
// FUNC(T1, T2, T3, T4)
#define OP_FUNC4(NAME, T1, T2, T3, T4, FUNC)\
RtVoid NAME(SlRunContext& context)\
{\
	Rt##T1* lhs = context.get##T1(1);\
	const Rt##T2* op1 = context.get##T2(2);\
	const Rt##T3* op2 = context.get##T3(3);\
	const Rt##T4* op3 = context.get##T4(4);\
	const int op1i = context.isVarying(2), op2i = context.isVarying(3),\
	          op3i = context.isVarying(4);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n;\
		     ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)\
			if (context.active(i))\
				FUNC(lhs[i], op1[op1o], op2[op2o], op3[op3o]);\
	}\
	else if (context.some_active()) {\
		assert((op1i | op2i | op3i) == 0);\
		FUNC(lhs[0], op1[0], op2[0], op3[0]);\
	}\
	context.nextInstruction();\
}

// single operand math functions, result and argument allways float
// float1 = FUNC(float2)
#define OP_MATHFUNC1(NAME, FUNC)\
RtVoid NAME(SlRunContext& context)\
{\
	RtFloat* lhs = context.getFloat(1);\
	const RtFloat* op1 = context.getFloat(2);\
	const int op1i = context.isVarying(2);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)\
			if (context.active(i))\
				lhs[i] = FUNC(op1[op1o]);\
	}\
	else if (context.some_active()) {\
		assert(op1i == 0);\
		lhs[0] = FUNC(op1[0]);\
	}\
	context.nextInstruction();\
}

// two operand math functions, result and arguments always float
// float1 = FUNC(float2, float3)
#define OP_MATHFUNC2(NAME, FUNC)\
RtVoid NAME(SlRunContext& context)\
{\
	RtFloat* lhs = context.getFloat(1);\
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);\
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)\
			if (context.active(i))\
				lhs[i] = FUNC(op1[op1o], op2[op2o]);\
	}\
	else if (context.some_active()) {\
		assert((op1i | op2i) == 0);\
		lhs[0] = FUNC(op1[0], op2[0]);\
	}\
	context.nextInstruction();\
}

//------------------------------------------------------------------------------
// Basic Math
// addition

OP_BINOP( sl_Add_FFF, + )

OP_FUNC3( sl_Add_VVV,
          Vector, Vector, Vector,
          addVVV )

OP_FUNC3( sl_Add_MMM,
          Matrix, Matrix, Matrix,
          addMMM )

//------------------------------------------------------------------------------
// subtraction

OP_BINOP( sl_Sub_FFF, - )

OP_FUNC3( sl_Sub_VVV,
          Vector, Vector, Vector,
          subVVV )

OP_FUNC3( sl_Sub_MMM,
          Matrix, Matrix, Matrix,
          subMMM )

//------------------------------------------------------------------------------
// multiplication

// float1 = float2 * float3
OP_BINOP( sl_Mul_FFF, * )

// vector1 = vector2 * vector3 (component wise multiply, not dotproduct)
OP_FUNC3( sl_Mul_VVV,
          Vector, Vector, Vector,
          mulVVV )

// matrix1 = matrix2 * matrix3
OP_FUNC3( sl_Mul_MMM,
          Matrix, Matrix, Matrix,
          mulMMM )

// vector1 = vector2 * float3
OP_FUNC3( sl_Mul_VVF,
          Vector, Vector, Float,
          mulVVF )

//------------------------------------------------------------------------------
// Division

// float1 = float2 / float3
OP_BINOP( sl_Div_FFF, / )

// vector1 = vector2 / vector3 (component wise division)
OP_FUNC3( sl_Div_VVV,
          Vector, Vector, Vector,
          divVVV )

// matrix1 = matrix2 / matrix2 ( == matrix2 * inverse_matrix3 )
OP_FUNC3( sl_Div_MMM,
          Matrix, Matrix, Matrix,
          divMMM )

// vector1 = vector2 / float3
OP_FUNC3( sl_Div_VVF,
          Vector, Vector, Float,
          divVVF )

//------------------------------------------------------------------------------
// Multiply & Add, madd

// float1 += float2 * float3
RtVoid sl_Madd_FFF(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat* op1 = context.getFloat(2);
	const RtFloat* op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				lhs[i] += op1[op1o] * op2[op2o];
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		lhs[0] += op1[0] * op2[0];
	}
	context.nextInstruction();
}

// vector1 += vector2 * vector3 (component wise multiply, not dotproduct)
OP_FUNC3( sl_Madd_VVV,
          Vector, Vector, Vector,
          maddVVV )

// vector1 += vector2 * float3
OP_FUNC3( sl_Madd_VVF,
          Vector, Vector, Float,
          maddVVF )


//------------------------------------------------------------------------------
// Multiply & Subtract, msub

// float1 -= float2 * float3
RtVoid sl_Msub_FFF(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat* op1 = context.getFloat(2);
	const RtFloat* op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				lhs[i] -= op1[op1o] * op2[op2o];
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		lhs[0] -= op1[0] * op2[0];
	}
	context.nextInstruction();
}

// vector1 -= vector2 * vector3 (component wise multiply, not dotproduct)
OP_FUNC3( sl_Msub_VVV,
          Vector, Vector, Vector,
          msubVVV )

// vector1 -= vector2 * float3
OP_FUNC3( sl_Msub_VVF,
          Vector, Vector, Float,
          msubVVF )

//------------------------------------------------------------------------------
// Vector cross & dot

// vector1 = vector2 ^ vector3  crossproduct
OP_FUNC3( sl_Vcross_VVV,
          Vector, Vector, Vector,
          vcross )

// float1 = vector1 . vector2  dotproduct
OP_FUNC3( sl_Vdot_FVV,
          Float, Vector, Vector,
          vdot )

//------------------------------------------------------------------------------
// Negate

// float1 = -float2
RtVoid sl_Neg_FF(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat* op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i) {
			if (context.active(i))
				lhs[i] = -op1[op1o];
		}
	}
	else {
		assert(op1i == 0);
		lhs[0] = -op1[0];
	}
	context.nextInstruction();
}

// vector1 = -vector2
RtVoid sl_Neg_VV(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtVector* op1 = context.getVector(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i) {
			if (context.active(i))
				lhs[i][0] = -op1[op1o][0], lhs[i][1] = -op1[op1o][1], lhs[i][2] = -op1[op1o][2];
		}
	}
	else {
		assert(op1i == 0);
		lhs[0][0] = -op1[0][0], lhs[0][1] = -op1[0][1], lhs[0][2] = -op1[0][2];
	}
	context.nextInstruction();
}

//------------------------------------------------------------------------------
// Math functions

// float1 = abs(float2)
OP_MATHFUNC1( sl_Abs, ABS )

// float1 = acos(float2)
inline float ACOS(float x)
{
	return (x <= -1.f) ? (float)M_PI : ((x >= 1.f) ? 0.f : acosf(x));
}
OP_MATHFUNC1( sl_Acos, ACOS )

// float1 = asin(float2)
inline float ASIN(float x)
{
	return (x <= -1.f) ? (-0.5f*(float)M_PI) : ((x >= 1.f) ? (0.5f*(float)M_PI) : asinf(x));
}
OP_MATHFUNC1( sl_Asin, ASIN )

// float1 = atan(float2)
OP_MATHFUNC1( sl_Atan1, atanf )

// float1 = atan2(float2, float3)
OP_MATHFUNC2( sl_Atan2, atan2f )

// float1 = ceil(float2)
OP_MATHFUNC1( sl_Ceil, CEILF )

// float1 = clamp(float2, float3, float4)
RtVoid sl_Clampf(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat* op1 = context.getFloat(2);
	const RtFloat* op2 = context.getFloat(3);
	const RtFloat* op3 = context.getFloat(4);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3),
	          op3i = context.isVarying(4);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n;
		     ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i))
				lhs[i] = CLAMP(op1[op1o], op2[op2o], op3[op3o]);
	}
	else if (context.some_active()) {
		assert((op1i | op2i | op3i) == 0);
		lhs[0] = CLAMP(op1[0], op2[0], op3[0]);
	}
	context.nextInstruction();
}

// vector1 = clamp(vector2, vector3, vector4)
RtVoid sl_Clampv(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtVector* op1 = context.getVector(2);
	const RtVector* op2 = context.getVector(3);
	const RtVector* op3 = context.getVector(4);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3),
	          op3i = context.isVarying(4);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n;
		     ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i)) {
				lhs[i][0] = CLAMP(op1[op1o][0], op2[op2o][0], op3[op3o][0]);
				lhs[i][1] = CLAMP(op1[op1o][1], op2[op2o][1], op3[op3o][1]);
				lhs[i][2] = CLAMP(op1[op1o][2], op2[op2o][2], op3[op3o][2]);
			}
	}
	else if (context.some_active()) {
		assert((op1i | op2i | op3i) == 0);
		lhs[0][0] = CLAMP(op1[0][0], op2[0][0], op3[0][0]);
		lhs[0][1] = CLAMP(op1[0][1], op2[0][1], op3[0][1]);
		lhs[0][2] = CLAMP(op1[0][2], op2[0][2], op3[0][2]);
	}
	context.nextInstruction();
}


// float1 = sin(float2)
OP_MATHFUNC1( sl_Sin, sinf )

// float1 = cos(float2)
OP_MATHFUNC1( sl_Cos, cosf )

// float1 = tan(float2)
OP_MATHFUNC1( sl_Tan, tanf )

// float1 = sign(float2)
OP_MATHFUNC1( sl_Sign, SIGN )

// float1 = exp(float2)
OP_MATHFUNC1( sl_Exp, expf )

// float1 = floor(float2)
OP_MATHFUNC1( sl_Floor, FLOORF )

// float1 = fmod(float2, float 3)
OP_MATHFUNC2( sl_Mod, FMOD )

// float1 = log(float2)
OP_MATHFUNC1( sl_Log, logf )

// float1 = logb(float2, float3)	(logarithm base float3 of float2)
inline float logb(float x, float y) { return logf(x)/logf(y); }
OP_MATHFUNC2( sl_Logb, logb )

// TODO min/max must be modified for arbitrary number of args, not just two
// float 1 = min(float2, float3)
OP_MATHFUNC2( sl_Minf, MIN2 )

// float 1 = max(float2, float3)
OP_MATHFUNC2( sl_Maxf, MAX2 )

// float1 = pow(float2, float3)
inline float POW(float x, float y)
{
	if (x < 0.f) return 0.f;
	if (y == 0.f) return 1.f;
	return powf(x, y);
}
OP_MATHFUNC2( sl_Pow, POW )

// float1 = sqrt(float2)
OP_MATHFUNC1( sl_Sqrt, sqrtf )

// float1 = 1.0/sqrt(float2)
inline float invsqrt(float x) { return 1.f/sqrtf(x); }
OP_MATHFUNC1( sl_Invsqrt, invsqrt )

//------------------------------------------------------------------------------
// copy/assigment ('move')

// float1 = float2
RtVoid sl_Mov_FF(SlRunContext& context)
{
	RtFloat *lhs = context.getFloat(1);
	const RtFloat *op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i) {
			if (context.active(i))
				lhs[i] = op1[op1o];
		}
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = op1[0];
	}
	context.nextInstruction();
}

// vector1 = vector2
RtVoid sl_Mov_VV(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1);
	const RtVector *op1 = context.getVector(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i][0] = op1[op1o][0], lhs[i][1] = op1[op1o][1], lhs[i][2] = op1[op1o][2];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0][0] = op1[0][0], lhs[0][1] = op1[0][1], lhs[0][2] = op1[0][2];
	}
	context.nextInstruction();
}

// vector1 = (float2, float2, float2)
RtVoid sl_Mov_VF(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1);
	const RtFloat *op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i][0] = lhs[i][1] = lhs[i][2] = op1[op1o];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0][0] = lhs[0][1] = lhs[0][2] = op1[0];
	}
	context.nextInstruction();
}

// matrix1 = matrix2
RtVoid sl_Mov_MM(SlRunContext& context)
{
	RtMatrix *lhs = context.getMatrix(1);
	const RtMatrix *op1 = context.getMatrix(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				memcpy(lhs[i], op1[op1o], sizeof(RtMatrix));
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		memcpy(lhs[0], op1[0], sizeof(RtMatrix));
	}
	context.nextInstruction();
}

// matrix1 = float2 (diagonal only, eg matrix m = 1, identity)
RtVoid sl_Mov_MF(SlRunContext& context)
{
	RtMatrix *lhs = context.getMatrix(1);
	const RtFloat *op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i)) {
				memset(lhs[i], 0, sizeof(RtMatrix));
				lhs[i][0][0] = lhs[i][1][1] = lhs[i][2][2] = lhs[i][3][3] = op1[op1o];
			}
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		memset(lhs[0], 0, sizeof(RtMatrix));
		lhs[0][0][0] = lhs[0][1][1] = lhs[0][2][2] = lhs[0][3][3] = op1[0];
	}
	context.nextInstruction();
}

// vector1 = (float2, float3, float4)
RtVoid sl_Mov_VF3(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3), *op3 = context.getFloat(4);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3), op3i = context.isVarying(4);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n; ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i))
				lhs[i][0] = op1[op1o], lhs[i][1] = op2[op2o], lhs[i][2] = op3[op3o];
	}
	else if (context.some_active()) {
		assert((op1i | op2i | op3i) == 0);
		lhs[0][0] = op1[0], lhs[0][1] = op2[0], lhs[0][2] = op3[0];
	}
	context.nextInstruction();
}

/* matrix1 = (float_2, float_3, float_4, float_5,
              float_6, float_7, float_8, float_9,
              float10, float11, float12, float13,
              float14, float15, float16, float17)*/
RtVoid sl_Mov_MF16(SlRunContext& context)
{
	RtMatrix *lhs = context.getMatrix(1);
	const RtFloat *op1  = context.getFloat(2),  *op2  = context.getFloat(3),  *op3  = context.getFloat(4),  *op4  = context.getFloat(5),
	              *op5  = context.getFloat(6),  *op6  = context.getFloat(7),  *op7  = context.getFloat(8),  *op8  = context.getFloat(9),
	              *op9  = context.getFloat(10), *op10 = context.getFloat(11), *op11 = context.getFloat(12), *op12 = context.getFloat(13),
	              *op13 = context.getFloat(14), *op14 = context.getFloat(15), *op15 = context.getFloat(16), *op16 = context.getFloat(17);
	const int op1i  = context.isVarying(2),  op2i  = context.isVarying(3),  op3i  = context.isVarying(4),  op4i  = context.isVarying(5),
	          op5i  = context.isVarying(6),  op6i  = context.isVarying(7),  op7i  = context.isVarying(8),  op8i  = context.isVarying(9),
	          op9i  = context.isVarying(10), op10i = context.isVarying(11), op11i = context.isVarying(12), op12i = context.isVarying(13),
	          op13i = context.isVarying(14), op14i = context.isVarying(15), op15i = context.isVarying(16), op16i = context.isVarying(17);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		int op1o  = 0, op2o  = 0, op3o  = 0, op4o = 0,
		    op5o  = 0, op6o  = 0, op7o  = 0, op8o = 0,
		    op9o  = 0, op10o = 0, op11o = 0, op12o = 0,
		    op13o = 0, op14o = 0, op15o = 0, op16o = 0;
		for (int i=0; i<n; ++i) {
			if (context.active(i))
				lhs[i][0][0] = op1[op1o],   lhs[i][0][1] = op2[op2o],   lhs[i][0][2] = op3[op3o],   lhs[i][0][3] = op4[op4o],
				lhs[i][1][0] = op5[op5o],   lhs[i][1][1] = op6[op6o],   lhs[i][1][2] = op7[op7o],   lhs[i][1][3] = op8[op8o],
				lhs[i][2][0] = op9[op9o],   lhs[i][2][1] = op10[op10o], lhs[i][2][2] = op11[op11o], lhs[i][2][3] = op12[op12o],
				lhs[i][3][0] = op13[op13o], lhs[i][3][1] = op14[op14o], lhs[i][3][2] = op15[op15o], lhs[i][3][3] = op16[op16o];
			op1o  += op1i,  op2o  += op2i,  op3o  += op3i,  op4o += op4i,
			op5o  += op5i,  op6o  += op6i,  op7o  += op7i,  op8o += op8i,
			op9o  += op9i,  op10o += op10i, op11o += op11i, op12o += op12i,
			op13o += op13i, op14o += op14i, op15o += op15i, op16o += op16i;
		}
	}
	else if (context.some_active()) {
		assert(( op1i  | op2i  | op3i  | op4i |
		         op5i  | op6i  | op7i  | op8i |
		         op9i  | op10i | op11i | op12i |
		         op13i | op14i | op15i | op16i ) == 0);
		lhs[0][0][0] = op1[0],  lhs[0][0][1] = op2[0],  lhs[0][0][2] = op3[0],  lhs[0][0][3] = op4[0],
		lhs[0][1][0] = op5[0],  lhs[0][1][1] = op6[0],  lhs[0][1][2] = op7[0],  lhs[0][1][3] = op8[0],
		lhs[0][2][0] = op9[0],  lhs[0][2][1] = op10[0], lhs[0][2][2] = op11[0], lhs[0][2][3] = op12[0],
		lhs[0][3][0] = op13[0], lhs[0][3][1] = op14[0], lhs[0][3][2] = op15[0], lhs[0][3][3] = op16[0];
	}
	context.nextInstruction();
}

//--------------------------------------------------------------------------------------------
// assignment from array

// float1 = float2[float3]
RtVoid sl_Movfa_FF(SlRunContext& context)
{
	RtFloat *lhs = context.getFloat(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				lhs[i] = op1[op1o + int(op2[op2o])];
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		lhs[0] = op1[int(op2[0])];
	}
	context.nextInstruction();
}

// vector1 = vector2[float3]
RtVoid sl_Movfa_VV(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1), *op1 = context.getVector(2);
	const RtFloat *op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i)) {
				const int idx = op1o + int(op2[op2o]);
				lhs[i][0] = op1[idx][0], lhs[i][1] = op1[idx][1], lhs[i][2] = op1[idx][2];
			}
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		const int idx = int(op2[0]);
		lhs[0][0] = op1[idx][0], lhs[0][1] = op1[idx][1], lhs[0][2] = op1[idx][2];
	}
	context.nextInstruction();
}

//--------------------------------------------------------------------------------------------
// assignment to array

// float1[float2] = float3
RtVoid sl_Movta_FF(SlRunContext& context)
{
	RtFloat *lhs = context.getFloat(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				lhs[i + int(op1[op1o])] = op2[op2o];
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		lhs[int(op1[0])] = op2[0];
	}
	context.nextInstruction();
}

// vector1[float2] = vector3
RtVoid sl_Movta_VV(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1), *op2 = context.getVector(3);
	const RtFloat *op1 = context.getFloat(2);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i)) {
				const int idx = i + int(op1[op1o]);
				lhs[idx][0] = op2[op2o][0], lhs[idx][1] = op2[op2o][1], lhs[idx][2] = op2[op2o][2];
			}
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		const int idx = int(op1[0]);
		lhs[idx][0] = op2[0][0], lhs[idx][1] = op2[0][1], lhs[idx][2] = op2[0][2];
	}
	context.nextInstruction();
}

// vector1[float2] = vector(float3, float4, float5)
RtVoid sl_Movta_VF3(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3),
	              *op3 = context.getFloat(4), *op4 = context.getFloat(5);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3),
	          op3i = context.isVarying(4), op4i = context.isVarying(5);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0, op4o=0; i<n;
		     ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i, op4o+=op4i)
		{
			if (context.active(i)) {
				const int idx = i + int(op1[op1o]);
				lhs[idx][0] = op2[op2o], lhs[idx][1] = op3[op3o], lhs[idx][2] = op4[op4o];
			}
		}
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		const int idx = int(op1[0]);
		lhs[idx][0] = op2[0], lhs[idx][1] = op3[0], lhs[idx][2] = op4[0];
	}
	context.nextInstruction();
}

//--------------------------------------------------------------------------------------------
// Conditional assignment

// float1 = (bool2 == true) ? float3 : float4
RtVoid sl_Cmov_F(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	boolarray_t& cond = *context.getBoolean(2);
	const RtFloat *op2 = context.getFloat(3), *op3 = context.getFloat(4);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3), op3i = context.isVarying(4);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n; ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i))
				lhs[i] = cond[op1o] ? op2[op2o] : op3[op3o];
	}
	else if (context.some_active()) {
		assert((op1i | op2i | op3i) == 0);
		lhs[0] = cond[0] ? op2[0] : op3[0];
	}
	context.nextInstruction();
}

// vector1 = (bool2 == true) ? vector3 : vector4
RtVoid sl_Cmov_V(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	boolarray_t& cond = *context.getBoolean(2);
	const RtVector *op2 = context.getVector(3), *op3 = context.getVector(4);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3), op3i = context.isVarying(4);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n; ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i)) {
				/*
				if (cond[op1o])
					lhs[i][0] = op2[op2o][0], lhs[i][1] = op2[op2o][1], lhs[i][2] = op2[op2o][2];
				else
					lhs[i][0] = op3[op3o][0], lhs[i][1] = op3[op3o][1], lhs[i][2] = op3[op3o][2];
				*/
				memcpy(lhs + i, cond[op1o] ? (op2 + op2o) : (op3 + op3o), sizeof(RtVector));
			}
	}
	else if (context.some_active()) {
		assert((op1i | op2i | op3i) == 0);
		/*
		if (cond[0])
			lhs[0][0] = op2[0][0], lhs[0][1] = op2[0][1], lhs[0][2] = op2[0][2];
		else
			lhs[0][0] = op3[0][0], lhs[0][1] = op3[0][1], lhs[0][2] = op3[0][2];
		*/
		memcpy(lhs, cond[0] ? op2 : op3, sizeof(RtVector));
	}
	context.nextInstruction();
}

//------------------------------------------------------------------------------
// Unconditional branching

RtVoid sl_Jmp(SlRunContext& context)
{
	context.PC = context.getAddress(1);
}

//------------------------------------------------------------------------------------------------------------------
// Conditional branching, if variable is varying, then condition must evaluate the same for *all* active processors

// jump to address iff string1 == string2
RtVoid sl_Jeq_SS(SlRunContext& context)
{
	// strings are always constant (but still have to take possible array into account, TODO)
	if (!strcmp(*context.getString(1), *context.getString(2)))
		context.PC = context.getAddress(3);
	else
		context.nextInstruction();
}

// jump to address iff string1 != string2
RtVoid sl_Jne_SS(SlRunContext& context)
{
	// strings are always constant (but still have to take possible array into account, TODO)
	if (strcmp(*context.getString(1), *context.getString(2)))
		context.PC = context.getAddress(3);
	else
		context.nextInstruction();
}

// jump to address iff float1 < float2
RtVoid sl_Jlt_FF(SlRunContext& context)
{
	if (context.isVarying(1)) {
		bool c = false;
		RtFloat *fv1 = context.getFloat(1), *fv2 = context.getFloat(2);
		const int op2i = context.isVarying(2);
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				c |= (fv1[i] < fv2[op2o]);
		if (c)
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
	else {
		// second arg must also be uniform
		assert(!context.isVarying(2));
		if (*context.getFloat(1) < *context.getFloat(2))
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
}

// jump to address iff float1 <= float2
RtVoid sl_Jle_FF(SlRunContext& context)
{
	if (context.isVarying(1)) {
		bool c = false;
		RtFloat *fv1 = context.getFloat(1), *fv2 = context.getFloat(2);
		const int op2i = context.isVarying(2);
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				c |= (fv1[i] <= fv2[op2o]);
		if (c)
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
	else {
		// second arg must also be uniform
		assert(!context.isVarying(2));
		if (*context.getFloat(1) <= *context.getFloat(2))
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
}

// jump to address iff float1 == float2
RtVoid sl_Jeq_FF(SlRunContext& context)
{
	if (context.isVarying(1)) {
		bool c = false;
		RtFloat *fv1 = context.getFloat(1), *fv2 = context.getFloat(2);
		const int op2i = context.isVarying(2);
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				c |= (fv1[i] == fv2[op2o]);
		if (c)
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
	else {
		// second arg must also be uniform
		assert(!context.isVarying(2));
		if (*context.getFloat(1) == *context.getFloat(2))
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
}

// jump to address iff float1 != float2
RtVoid sl_Jne_FF(SlRunContext& context)
{
	if (context.isVarying(1)) {
		bool c = false;
		RtFloat *fv1 = context.getFloat(1), *fv2 = context.getFloat(2);
		const int op2i = context.isVarying(2);
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				c |= (fv1[i] != fv2[op2o]);
		if (c)
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
	else {
		// second arg must also be uniform
		assert(!context.isVarying(2));
		if (*context.getFloat(1) != *context.getFloat(2))
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
}

// jump to address iff float1 >= float2
RtVoid sl_Jge_FF(SlRunContext& context)
{
	if (context.isVarying(1)) {
		bool c = false;
		RtFloat *fv1 = context.getFloat(1), *fv2 = context.getFloat(2);
		const int op2i = context.isVarying(2);
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				c |= (fv1[i] >= fv2[op2o]);
		if (c)
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
	else {
		// second arg must also be uniform
		assert(!context.isVarying(2));
		if (*context.getFloat(1) >= *context.getFloat(2))
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
}

// jump to address iff float1 > float2
RtVoid sl_Jgt_FF(SlRunContext& context)
{
	if (context.isVarying(1)) {
		bool c = false;
		RtFloat *fv1 = context.getFloat(1), *fv2 = context.getFloat(2);
		const int op2i = context.isVarying(2);
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				c |= (fv1[i] > fv2[op2o]);
		if (c)
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
	else {
		// second arg must also be uniform
		assert(!context.isVarying(2));
		if (*context.getFloat(1) > *context.getFloat(2))
			context.PC = context.getAddress(3);
		else
			context.nextInstruction();
	}
}

//------------------------------------------------------------------------------
// Conditional branching on boolean value

// jump if boolean value is true
RtVoid sl_Cjmp(SlRunContext& context)
{
	boolarray_t& cond = *context.getBoolean(1);
	if (context.isVarying(1)) {
		bool c = false;
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				c |= cond[i];
		if (c)
			context.PC = context.getAddress(2);
		else
			context.nextInstruction();
	}
	else {
		if (cond[0])
			context.PC = context.getAddress(2);
		else
			context.nextInstruction();
	}
}

// jump if boolean value is false
RtVoid sl_Cjmpnot(SlRunContext& context)
{
	boolarray_t& cond = *context.getBoolean(1);
	if (context.isVarying(1)) {
		bool c = false;
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				c |= cond[i];
		if (not c)
			context.PC = context.getAddress(2);
		else
			context.nextInstruction();
	}
	else {
		if (not cond[0])
			context.PC = context.getAddress(2);
		else
			context.nextInstruction();
	}
}

//------------------------------------------------------------------------------
// SIMD flags state control

RtVoid sl_Cond_reset(SlRunContext& context)
{
	context.SIMD_reset();
	context.nextInstruction();
}

RtVoid sl_Cond_push(SlRunContext& context)
{
	context.SIMD_push(*context.getBoolean(1));
	context.nextInstruction();
}

RtVoid sl_Cond_pop(SlRunContext& context)
{
	context.SIMD_pop();
	context.nextInstruction();
}

RtVoid sl_Cond_else(SlRunContext& context)
{
	context.SIMD_else();
	context.nextInstruction();
}

//------------------------------------------------------------------------------
// boolean logic

RtVoid sl_Or(SlRunContext& context)
{
	boolarray_t &condr = *context.getBoolean(1), &cond1 = *context.getBoolean(2), &cond2 = *context.getBoolean(3);
	const int inc1 = context.isVarying(2), inc2 = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, i1=0, i2=0; i<n; ++i, i1+=inc1, i2+=inc2)
			if (context.active(i))
				condr(i, cond1[i1] or cond2[i2]);
	}
	else if (context.some_active()) {
		assert((inc1 | inc2) == 0);
		condr(0, cond1[0] or cond2[0]);
	}
	context.nextInstruction();
}

RtVoid sl_And(SlRunContext& context)
{
	boolarray_t &condr = *context.getBoolean(1), &cond1 = *context.getBoolean(2), &cond2 = *context.getBoolean(3);
	const int inc1 = context.isVarying(2), inc2 = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, i1=0, i2=0; i<n; ++i, i1+=inc1, i2+=inc2)
			if (context.active(i))
				condr(i, cond1[i1] and cond2[i2]);
	}
	else if (context.some_active()) {
		assert((inc1 | inc2) == 0);
		condr(0, cond1[0] and cond2[0]);
	}
	context.nextInstruction();
}

RtVoid sl_Not(SlRunContext& context)
{
	boolarray_t &condr = *context.getBoolean(1), &cond1 = *context.getBoolean(2);
	const int inc1 = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, i1=0; i<n; ++i, i1+=inc1)
			if (context.active(i))
				condr(i, not cond1[i1]);
	}
	else if (context.some_active()) {
		assert(inc1 == 0);
		condr(0, not cond1[0]);
	}
	context.nextInstruction();
}

//------------------------------------------------------------------------------

// evaluate condition
#define EVALCOND(NAME, T1, T2, OPER)\
RtVoid NAME(SlRunContext& context)\
{\
	boolarray_t &cond = *context.getBoolean(1);\
	const Rt##T1* op1 = context.get##T1(2);\
	const Rt##T2* op2 = context.get##T2(3);\
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)\
			if (context.active(i))\
				cond(i, op1[op1o] OPER op2[op2o]);\
	}\
	else if (context.some_active()) {\
		assert((op1i | op2i) == 0);\
		cond(0, op1[0] OPER op2[0]);\
	}\
	context.nextInstruction();\
}

EVALCOND( sl_Iflt_FF, Float, Float, < )
EVALCOND( sl_Ifle_FF, Float, Float, <= )
EVALCOND( sl_Ifeq_FF, Float, Float, == )
EVALCOND( sl_Ifne_FF, Float, Float, != )
EVALCOND( sl_Ifge_FF, Float, Float, >= )
EVALCOND( sl_Ifgt_FF, Float, Float, > )

RtVoid sl_Ifeq_SS(SlRunContext& context)
{
	// string can never be varying
	assert((context.isVarying(2) | context.isVarying(3)) == 0);
	boolarray_t &cond = *context.getBoolean(1);
	bool bval = (!strcmp(*context.getString(2), *context.getString(3)));
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				cond(i, bval);
	}
	else if (context.some_active())
		cond(0, bval);
	context.nextInstruction();
}

RtVoid sl_Ifne_SS(SlRunContext& context)
{
	// string can never be varying
	assert((context.isVarying(2) | context.isVarying(3)) == 0);
	boolarray_t &cond = *context.getBoolean(1);
	bool bval = (strcmp(*context.getString(2), *context.getString(3)));
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				cond(i, bval);
	}
	else if (context.some_active())
		cond(0, bval);
	context.nextInstruction();
}

// evaluate condition and apply boolean logic at the same time (similar to a 'multiply-add')
#define EVALCOND_LOGIC(NAME, T1, T2, OPER1, OPER2)\
RtVoid NAME(SlRunContext& context)\
{\
	boolarray_t &cond = *context.getBoolean(1);\
	const Rt##T1* op1 = context.get##T1(2);\
	const Rt##T2* op2 = context.get##T2(3);\
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);\
	if (context.isVarying(1)) {\
		const int n = context.SIMD_count;\
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)\
			if (context.active(i))\
				cond(i, cond[i] OPER1 (op1[op1o] OPER2 op2[op2o]));\
	}\
	else if (context.some_active()) {\
		assert((op1i | op2i) == 0);\
		cond(0, cond[0] OPER1 (op1[0] OPER2 op2[0]));\
	}\
	context.nextInstruction();\
}

EVALCOND_LOGIC( sl_Orlt_FF, Float, Float, or, < )
EVALCOND_LOGIC( sl_Orle_FF, Float, Float, or, <= )
EVALCOND_LOGIC( sl_Oreq_FF, Float, Float, or, == )
EVALCOND_LOGIC( sl_Orne_FF, Float, Float, or, != )
EVALCOND_LOGIC( sl_Orge_FF, Float, Float, or, >= )
EVALCOND_LOGIC( sl_Orgt_FF, Float, Float, or, > )

RtVoid sl_Oreq_SS(SlRunContext& context)
{
	// string can never be varying
	assert((context.isVarying(2) | context.isVarying(3)) == 0);
	boolarray_t &cond = *context.getBoolean(1);
	bool bval = (!strcmp(*context.getString(2), *context.getString(3)));
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				cond(i, cond[i] or bval);
	}
	else if (context.some_active())
		cond(0, cond[0] or bval);
	context.nextInstruction();
}

RtVoid sl_Orne_SS(SlRunContext& context)
{
	// string can never be varying
	assert((context.isVarying(2) | context.isVarying(3)) == 0);
	boolarray_t &cond = *context.getBoolean(1);
	bool bval = (strcmp(*context.getString(2), *context.getString(3)));
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				cond(i, cond[i] or bval);
	}
	else if (context.some_active())
		cond(0, cond[0] or bval);
	context.nextInstruction();
}

EVALCOND_LOGIC( sl_Andlt_FF, Float, Float, and, < )
EVALCOND_LOGIC( sl_Andle_FF, Float, Float, and, <= )
EVALCOND_LOGIC( sl_Andeq_FF, Float, Float, and, == )
EVALCOND_LOGIC( sl_Andne_FF, Float, Float, and, != )
EVALCOND_LOGIC( sl_Andge_FF, Float, Float, and, >= )
EVALCOND_LOGIC( sl_Andgt_FF, Float, Float, and, > )

RtVoid sl_Andeq_SS(SlRunContext& context)
{
	// string can never be varying
	assert((context.isVarying(2) | context.isVarying(3)) == 0);
	boolarray_t &cond = *context.getBoolean(1);
	bool bval = (!strcmp(*context.getString(2), *context.getString(3)));
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				cond(i, cond[i] and bval);
	}
	else if (context.some_active())
		cond(0, cond[0] and bval);
	context.nextInstruction();
}

RtVoid sl_Andne_SS(SlRunContext& context)
{
	// string can never be varying
	assert((context.isVarying(2) | context.isVarying(3)) == 0);
	boolarray_t &cond = *context.getBoolean(1);
	bool bval = (strcmp(*context.getString(2), *context.getString(3)));
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				cond(i, cond[i] and bval);
	}
	else if (context.some_active())
		cond(0, cond[0] and bval);
	context.nextInstruction();
}

//------------------------------------------------------------------------------
// shader functions

// float1 = length(vector2)
OP_FUNC2( sl_Length,
          Float, Vector,
          vlength )

// vector1[x] = float2
RtVoid sl_Setxcomp(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1);
	const RtFloat *op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i][0] = op1[op1o];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0][0] = op1[0];
	}
	context.nextInstruction();
}

// vector1[y] = float2
RtVoid sl_Setycomp(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1);
	const RtFloat *op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i][1] = op1[op1o];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0][1] = op1[0];
	}
	context.nextInstruction();
}

// vector1[z] = float2
RtVoid sl_Setzcomp(SlRunContext& context)
{
	RtVector *lhs = context.getVector(1);
	const RtFloat *op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i][2] = op1[op1o];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0][2] = op1[0];
	}
	context.nextInstruction();
}

// float1 = vector2[x]
RtVoid sl_Xcomp(SlRunContext& context)
{
	RtFloat *lhs = context.getFloat(1);
	const RtVector *op1 = context.getVector(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = op1[op1o][0];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = op1[0][0];
	}
	context.nextInstruction();
}

// float1 = vector2[y]
RtVoid sl_Ycomp(SlRunContext& context)
{
	RtFloat *lhs = context.getFloat(1);
	const RtVector *op1 = context.getVector(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = op1[op1o][1];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = op1[0][1];
	}
	context.nextInstruction();
}

// float1 = vector2[z]
RtVoid sl_Zcomp(SlRunContext& context)
{
	RtFloat *lhs = context.getFloat(1);
	const RtVector *op1 = context.getVector(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = op1[op1o][2];
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = op1[0][2];
	}
	context.nextInstruction();
}

// float1 = vector2[float3]
RtVoid sl_Compv(SlRunContext& context)
{
	RtFloat *lhs = context.getFloat(1);
	const RtVector* op1 = context.getVector(2);
	const RtFloat* op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				lhs[i] = op1[op1o][int(op2[op2o])];
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		lhs[0] = op1[0][int(op2[0])];
	}
	context.nextInstruction();
}


// -----------------------------------------------------------------------------
// LIGHTING

// this for lighting from environment maps, TODO
RtVoid sl_Solar1(SlRunContext& context)
{
	context.nextInstruction();
}

// solar(axis, angle)
RtVoid sl_Solar2(SlRunContext& context)
{
	RtVector* axis = context.getVector(1);
	// angle not used yet, TODO, just assume 'sunlight' for now...
	//RtFloat* angle = context.getFloat(2);

	RtVector* L = (RtVector*)(context.grid->findVariable("L"));
	assert(L != NULL);

	// since angle is not used, nothing to do but simply copy axis to L
	if (context.isVarying(1))
		memcpy(L, axis, sizeof(RtVector)*context.SIMD_count);
	else {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			L[i][0] = axis[0][0], L[i][1] = axis[0][1], L[i][2] = axis[0][2];
	}

	context.nextInstruction();
}

RtVoid sl_End_solar(SlRunContext& context)
{
	context.nextInstruction();
}

RtVoid sl_Illuminate1(SlRunContext& context)
{
	RtVector* from = context.getVector(1);

	RtVector* L = (RtVector*)(context.grid->findVariable("L"));
	assert(L != NULL);
	RtPoint* Ps = (RtPoint*)(context.grid->findVariable("Ps"));
	assert(Ps != NULL);

	const int op1i = context.isVarying(1);
	const int n = context.SIMD_count;
	for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
		if (context.active(i)) {
			L[i][0] = Ps[i][0] - from[op1o][0];
			L[i][1] = Ps[i][1] - from[op1o][1];
			L[i][2] = Ps[i][2] - from[op1o][2];
		}

	context.nextInstruction();
}

RtVoid sl_Illuminate2(SlRunContext& context)
{
	RtVector* from = context.getVector(1);
	RtVector* axis = context.getVector(2);
	RtFloat* angle = context.getFloat(3);

	RtVector* L = (RtVector*)(context.grid->findVariable("L"));
	assert(L != NULL);
	RtPoint* Ps = (RtPoint*)(context.grid->findVariable("Ps"));
	assert(Ps != NULL);

	const int op1i = context.isVarying(1), op2i = context.isVarying(2);
	// for now always expect uniform angle arg
	assert(!context.isVarying(3));

	RtFloat ca = cosf(*angle);
	const int n = context.SIMD_count;
	for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
		if (context.active(i)) {
			L[i][0] = Ps[i][0] - from[op1o][0];
			L[i][1] = Ps[i][1] - from[op1o][1];
			L[i][2] = Ps[i][2] - from[op1o][2];
			RtVector Ln = {L[i][0], L[i][1], L[i][2]};
			vnormalize(Ln, Ln);
			if ((Ln[0]*axis[op2o][0] + Ln[1]*axis[op2o][1] + Ln[2]*axis[op2o][2]) < ca)
				context.disable(i);
		}

	context.nextInstruction();
}

RtVoid sl_End_illuminate(SlRunContext& context)
{
	context.nextInstruction();
}

//----------------------------------------------------------------------------------------------------------------------

//*********************************************************************************************************
// The next functions form the main illuminance() loop which iterates through the current lightsource list.
// It has 3 parts, getLight(), runLight() & endLight()
// getLight() returns the next lightsource in the list or null if no (more) lights are available.
// For ambient lights there is a special getAmbientLight() function, since getLight() only returns
// any non-ambient light sources in the list.
// runLight() initializes the lightshader variables, runs the actual lightshader and additionally,
// if required, sets the SIMD flag state according to the axis/angle condition.
// endLight() removes the temporary lightshader grid and resets the light iterator and SIMD state.
//*********************************************************************************************************

// TODO category

SlShaderInstance* _Illuminance_getAmbientLight(SlRunContext& context)
{
	const Attributes* attr = context.grid->get_parent()->getAttributeReference();
	const unsigned int numlights = attr->numlights;
	if ((numlights == 0) || (context.light_iterator == numlights)) return NULL; // no (more) lights
	SlShaderInstance* light = NULL;
	// on first call light_iterator will be 0
	if (context.light_iterator == 0) {
		// first ambient light
		unsigned int lidx = 0;
		for ( ; lidx<numlights; ++lidx) {
			light = attr->lightsources[lidx];
			if (light->shader->isAmbient) break;
		}
		if (lidx == numlights) return NULL; // no non-ambient lights found
		// first lightsource
		context.light_iterator = lidx + 1;	// next light
	}
	else {
		// get the next ambient lightshader
		unsigned int lidx = context.light_iterator;
		for ( ; lidx<numlights; ++lidx) {
			light = attr->lightsources[lidx];
			if (light->shader->isAmbient) break;
		}
		if (lidx == numlights) return NULL; // no more lights
		context.light_iterator = lidx + 1;	// next light
	}
	return light;
}

SlShaderInstance* _Illuminance_getLight(SlRunContext& context)
{
	const Attributes* attr = context.grid->get_parent()->getAttributeReference();
	const unsigned int numlights = attr->numlights;
	if ((numlights == 0) || (context.light_iterator == numlights)) return NULL; // no (more) lights
	SlShaderInstance* light = NULL;
	// on first call light_iterator will be 0
	if (context.light_iterator == 0) {
		// first non-ambient light
		unsigned int lidx = 0;
		for ( ; lidx<numlights; ++lidx) {
			light = attr->lightsources[lidx];
			if (!light->shader->isAmbient) break;
		}
		if (lidx == numlights) return NULL; // no non-ambient lights found
		// first lightsource
		context.light_iterator = lidx + 1;	// next light
	}
	else {
		// get the next non-ambient lightshader
		unsigned int lidx = context.light_iterator;
		for ( ; lidx<numlights; ++lidx) {
			light = attr->lightsources[lidx];
			if (!light->shader->isAmbient) break;
		}
		if (lidx == numlights) return NULL; // no more lights
		context.light_iterator = lidx + 1;	// next light
	}
	return light;
}

void _Illuminance_runLight(SlRunContext& context, SlShaderInstance* light,
                           RtPoint* P, RtVector* axis=NULL, RtFloat* angle=NULL, bool axis_vary = true)
{

	const int numprocs = context.SIMD_count;
	MicroPolygonGrid* lightgrid = context.aux_grid;
	if (lightgrid == NULL) {
		// create temporary lightshader grid & init the variables used by the shader
		lightgrid = context.aux_grid = new MicroPolygonGrid();
		lightgrid->setFrom(*context.grid);
		lightgrid->initVars(light->shader->globals_used);
		// Ps == P of surface shader, if it is not used by the shader, add it, always available (expected in illuminate() & solar())
		// (can just call addVariable() since if already available no allocation is done)
		RtPoint* Ps = (RtPoint*)lightgrid->addVariable("Ps");
		memcpy(Ps, P, sizeof(RtPoint)*numprocs);
		// and L as well, if not used in shader, add it, also expected in illuminate/solar
		lightgrid->addVariable("L");
		// TODO init of all other required lightshader vars (N, P, dPdu, etc..)
	}

	// clear Cl before each run
	RtColor* Cl = (RtColor*)(lightgrid->findVariable("Cl"));
	assert(Cl != NULL);	// should never happen
	memset(Cl, 0, sizeof(RtColor)*numprocs);

	// run the light shader
	light->run(lightgrid);

	// now copy back L & Cl data to surface shader
	RtColor* surf_Cl = (RtColor*)context.grid->findVariable("Cl");
	if (surf_Cl) memcpy(surf_Cl, Cl, sizeof(RtColor)*numprocs);
	RtVector* surf_L = (RtVector*)context.grid->findVariable("L");
	RtVector* light_L = (RtVector*)lightgrid->findVariable("L");
	if (surf_L && light_L) {
		// L in surface shader is the vector pointing *from* surface *to* light, so surf_L == -light_L
		for (int i=0; i<numprocs; ++i)
			surf_L[i][0] = -light_L[i][0], surf_L[i][1] = -light_L[i][1], surf_L[i][2] = -light_L[i][2];
	}

	if (axis && angle && surf_L) {
		// NOTE this is one of the few occasions that the loop does not depend on current state, all processors are reset for each light
		context.SIMD_reset();
		// set simd flags according to (L.axis < angle) state
		const RtFloat cosa = cosf(*angle);
		for (int i=0; i<numprocs; ++i) {
			RtVector Ln = {surf_L[i][0], surf_L[i][1], surf_L[i][2]};
			vnormalize(Ln, Ln);
			const int aidx = axis_vary ? i : 0;
			if ((Ln[0]*axis[aidx][0] + Ln[1]*axis[aidx][1] + Ln[2]*axis[aidx][2]) < cosa)
				context.disable(i);
		}
	}

}

void _Illuminance_endLight(SlRunContext& context)
{
	delete context.aux_grid;
	context.aux_grid = NULL;
	context.light_iterator = 0;
	context.SIMD_reset();
}

//----------------------------------------------------------------------------------------------------------------------

// illuminance(position)
RtVoid sl_Illuminance1(SlRunContext& context)
{
	// must be varying
	assert(context.isVarying(1));
	SlShaderInstance* light = _Illuminance_getLight(context);
	if (light == NULL) {
		// the end
		_Illuminance_endLight(context);
		context.PC = context.getAddress(2);
		return;
	}
	_Illuminance_runLight(context, light, context.getVector(1));
	// to illuminance body
	context.nextInstruction();
}

// illuminance(position, axis, angle)
RtVoid sl_Illuminance2(SlRunContext& context)
{
	// must be varying
	assert(context.isVarying(1));
	// for now always expect uniform angle arg
	assert(!context.isVarying(3));
	SlShaderInstance* light = _Illuminance_getLight(context);
	if (light == NULL) {
		// the end
		_Illuminance_endLight(context);
		context.PC = context.getAddress(4);
		return;
	}
	_Illuminance_runLight(context, light, context.getVector(1), context.getVector(2), context.getFloat(3), context.isVarying(2));
	// to illuminance body
	context.nextInstruction();
}


// varying float = shadow(filename, point, paramlist... )
RtVoid sl_Shadow(SlRunContext& context)
{
	// lhs must be varying
	assert(context.isVarying(1));

	RtString fname = context.getString(2)[0];

	RtFloat* lhs = context.getFloat(1);
	RtPoint* pt = (RtPoint*)context.getVector(3);
	const int pti = context.isVarying(3);

	// the current supported params, for now all must be uniform
	RtFloat *samples = NULL, *blur = NULL, *bias = NULL;
	const RtInt pl = context.getOperandCount();
	for (int i=4; i<pl; i += 2) {
		RtString tk = context.getString(i)[0];
		if (!strcmp(tk, "samples")) {
			samples = context.getFloat(i+1);
			assert(not context.isVarying(i+1));
		}
		else if (!strcmp(tk, "blur")) {
			blur = context.getFloat(i+1);
			assert(not context.isVarying(i+1));
		}
		else if (!strcmp(tk, "bias")) {
			bias = context.getFloat(i+1);
			assert(not context.isVarying(i+1));
		}
	}

	// shadow coords are expected to be in worldspace
	const RtMatrix* mtx = State::Instance()->getNamedCoordSys("world")->getRtMatrixPtr();
	RtMatrix imtx;
	invertMatrix(imtx, *mtx);	// current to world

	float st[8]; // not used yet, area filtering TODO
	const int n = context.SIMD_count;
	for (int i=0; i<n; ++i, pt+=pti) {
		if (context.active(i)) {
			RtPoint tp;
			mulPMP(tp, imtx, *pt);
			lhs[i] = getShadowFactor(fname, st, tp, samples, blur, bias);
		}
	}

	context.nextInstruction();
}

RtVoid sl_Texture_V(SlRunContext& context)
{
	// lhs must be varying
	assert(context.isVarying(1));
	RtColor* lhs = (RtColor*)context.getVector(1);
	
	// default s/t coords
	RtFloat* s_grid = (RtFloat*)context.grid->findVariable("s");
	RtFloat* t_grid = (RtFloat*)context.grid->findVariable("t");
	assert((s_grid != NULL) && (t_grid != NULL));
	RtString texname = context.getString(2)[0];

	// texCoords are not relevant here, should take it into account when dicing s & t only, only used in parametric patches

	float st[8]; // coords of area to filter
	const int xdim = context.grid->get_xdim(), ydim = context.grid->get_ydim();
	int i = 0;
	const int wd = xdim + 1;
	for (int y=0; y<=ydim; ++y) {
		const int yx = y*wd;
		const int yx2 = ((y == ydim) ? (y - 1) : (y + 1))*wd;
		for (int x=0; x<=xdim; ++x, ++i) {
			if (context.active(i)) {
				const int x2 = (x == xdim) ? (x - 1) : (x + 1);
				const float qa = sqrtf(ABS(quad_area(Point2(s_grid[i], t_grid[i]), Point2(s_grid[x2 + yx], t_grid[x2 + yx]),
				                                     Point2(s_grid[x2 + yx2], t_grid[x2 + yx2]), Point2(s_grid[x + yx2], t_grid[x + yx2]))));
				st[0] = s_grid[i] - qa, st[1] = t_grid[i] - qa;
				st[2] = s_grid[i] + qa, st[3] = t_grid[i] - qa;
				st[4] = s_grid[i] - qa, st[5] = t_grid[i] + qa;
				st[6] = s_grid[i] + qa, st[7] = t_grid[i] + qa;
				getTextureColor(lhs[i], texname, st, qa*qa);
			}
		}
	}

	context.nextInstruction();
}

// float = texture(filename, float2, float3)
RtVoid sl_Texture_F(SlRunContext& context)
{
	// lhs/svar/tvar must all be varying
	assert(context.isVarying(1));
	assert(context.isVarying(3));
	assert(context.isVarying(4));

	RtFloat* lhs = (RtFloat*)context.getFloat(1);
	RtFloat* s_grid = (RtFloat*)context.getFloat(3);
	RtFloat* t_grid = (RtFloat*)context.getFloat(4);
	RtString texname = context.getString(2)[0];

	float st[8]; // coords of area to filter
	const int xdim = context.grid->get_xdim(), ydim = context.grid->get_ydim();
	int i = 0;
	const int wd = xdim + 1;
	for (int y=0; y<=ydim; ++y) {
		const int yx = y*wd;
		const int yx2 = ((y == ydim) ? (y - 1) : (y + 1))*wd;
		for (int x=0; x<=xdim; ++x, ++i) {
			if (context.active(i)) {
				const int x2 = (x == xdim) ? (x - 1) : (x + 1);
				const float qa = sqrtf(ABS(gQuad_area(Point2(s_grid[i], t_grid[i]), Point2(s_grid[x2 + yx], t_grid[x2 + yx]),
				                                      Point2(s_grid[x2 + yx2], t_grid[x2 + yx2]), Point2(s_grid[x + yx2], t_grid[x + yx2]))));
				st[0] = s_grid[i] - qa, st[1] = t_grid[i] - qa;
				st[2] = s_grid[i] + qa, st[3] = t_grid[i] - qa;
				st[4] = s_grid[i] - qa, st[5] = t_grid[i] + qa;
				st[6] = s_grid[i] + qa, st[7] = t_grid[i] + qa;
				getTextureFloat(lhs[i], texname, st, qa*qa);
			}
		}
	}

	context.nextInstruction();
}

//---------------------------------------------------------------------------------------------------
// ambient(), diffuse() & specular functions

// Ambient lights only
RtVoid sl_Ambient(SlRunContext& context)
{
	// lhs must be varying
	assert(context.isVarying(1));

	RtVector *lhs = context.getVector(1);
	// reset lhs to zero, is accumulated result of all ambient light
	memset(lhs, 0, sizeof(RtVector)*context.SIMD_count);

	// required for runLight()
	RtPoint* P = (RtPoint*)context.grid->findVariable("P");

	SlShaderInstance* light;
	RtColor* Cl = NULL;
	while ((light = _Illuminance_getAmbientLight(context)) != NULL) {
		_Illuminance_runLight(context, light, P);
		if (Cl == NULL) {	// first light, get Cl from lightshader (might not be used by surface shader)
			Cl = (RtColor*)context.aux_grid->findVariable("Cl");
			assert(Cl != NULL);
		}
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i) {
			if (context.active(i)) {
				lhs[i][0] += Cl[i][0];
				lhs[i][1] += Cl[i][1];
				lhs[i][2] += Cl[i][2];
			}
		}
	}
	_Illuminance_endLight(context);

	context.nextInstruction();
}


RtVoid sl_Diffuse(SlRunContext& context)
{
	// must be varying
	assert(context.isVarying(1));
	RtVector* lhs = context.getVector(1);
	RtVector* axis = context.getVector(2);

	// temporary accumulation grid.
	// cannot directly assign to lhs in case it is the same as operand,
	// though in compiler generated code that probably is not likely to occur
	// (for simplicity probably better not allow this, not sure yet),
	// but for handwritten vm code have to take this possible case into account)
	const int numprocs = context.SIMD_count;
	RtColor* acol = new RtColor[numprocs];
	memset(acol, 0, sizeof(RtColor)*numprocs);
	// 'P' is needed to initialize 'Ps' in illuminance()
	RtPoint* P = (RtPoint*)(context.grid->findVariable("P"));

	SlShaderInstance* light;
	RtFloat angle = (float)M_PI*0.5f;
	RtVector* L = NULL;
	RtColor* Cl = NULL;
	while ((light = _Illuminance_getLight(context)) != NULL) {
		_Illuminance_runLight(context, light, P, axis, &angle, context.isVarying(2));
		if (L == NULL) {	// first light, get L & Cl from lightshader (surface shader might not use them directly, but lightshader does)
			L = (RtVector*)context.aux_grid->findVariable("L");
			assert(L != NULL);
			Cl = (RtColor*)context.aux_grid->findVariable("Cl");
			assert(Cl != NULL);
		}
		// simple lambert (NOTE: accessing L in light shader, so using -L here!)
		RtFloat dotp;
		for (int i=0; i<numprocs; ++i) {
			if (context.active(i)) {
				if ((dotp = -L[i][0]*axis[i][0] - L[i][1]*axis[i][1] - L[i][2]*axis[i][2]) > 0.f) {
					dotp /= sqrtf(L[i][0]*L[i][0] + L[i][1]*L[i][1] + L[i][2]*L[i][2]);
					acol[i][0] += dotp*Cl[i][0];
					acol[i][1] += dotp*Cl[i][1];
					acol[i][2] += dotp*Cl[i][2];
				}
			}
		}
	}

	// done, transfer accumulated result to lhs
	memcpy(lhs, acol, sizeof(RtColor)*numprocs);
	// delete the temporary grid
	delete[] acol;

	_Illuminance_endLight(context);

	context.nextInstruction();
}


RtVoid sl_Specular(SlRunContext& context)
{
	// must be varying
	assert(context.isVarying(1));
	assert(context.isVarying(2));
	assert(context.isVarying(3));

	RtVector* lhs = context.getVector(1);
	RtVector* N = context.getVector(2);
	RtVector* V = context.getVector(3);
	RtFloat* rough = context.getFloat(4);
	// something is not right in the RIspec3.2, to match BMRT a div.factor of about 10 is needed...
	// either BMRT is wrong or the spec is wrong...
	// on the other hand, both pixie and 3delight seem to produce exactly the same result,
	// so I guess it must be me, but I can't see nothing wrong...
	// update: according to a comp.graphics.renderman post by Rick LaMont,
	// apparently the exact number is 8. It seems Paul Gregory had the same problem...
	const RtFloat ir = (rough[0] == 0.f) ? 1.f : (8.f/rough[0]);

	// temporary accumulation grid (see sl_Diffuse)
	const int numprocs = context.SIMD_count;
	RtColor* acol = new RtColor[numprocs];
	memset(acol, 0, sizeof(RtColor)*numprocs);
	// 'P' is needed to initialize 'Ps' in illuminance()
	RtPoint* P = (RtPoint*)(context.grid->findVariable("P"));

	SlShaderInstance* light;
	RtVector* L = NULL;
	RtColor* Cl = NULL;
	while ((light = _Illuminance_getLight(context)) != NULL) {
		_Illuminance_runLight(context, light, P);
		if (L == NULL) {	// first light, get L & Cl from lightshader (surface shader might not use them directly, but lightshader does)
			L = (RtVector*)context.aux_grid->findVariable("L");
			assert(L != NULL);
			Cl = (RtColor*)context.aux_grid->findVariable("Cl");
			assert(Cl != NULL);
		}
		// simple Blinn/Phong specular
		for (int i=0; i<numprocs; ++i) {
			if (context.active(i)) {
				// NOTE: accessing L of light shader! so have to use -L here
				RtVector Ln = {-L[i][0], -L[i][1], -L[i][2]};
				vnormalize(Ln, Ln);
				if ((Ln[0]*N[i][0] + Ln[1]*N[i][1] + Ln[2]*N[i][2]) > 0.f) {
					RtVector H = {Ln[0] + V[i][0], Ln[1] + V[i][1], Ln[2] + V[i][2]};
					vnormalize(H, H);
					RtFloat spec = powf(MAX2(0.f, MIN2(1.f, N[i][0]*H[0] + N[i][1]*H[1] + N[i][2]*H[2])), ir);
					acol[i][0] += spec * Cl[i][0];
					acol[i][1] += spec * Cl[i][1];
					acol[i][2] += spec * Cl[i][2];
				}
			}
		}
	}

	// done, transfer accumulated result to lhs
	memcpy(lhs, acol, sizeof(RtColor)*numprocs);
	// delete the temporary grid
	delete[] acol;

	_Illuminance_endLight(context);

	context.nextInstruction();
}

// this is really a shader function, just returns specular factor, just as in specular()
inline void specularbrdf(RtVector r, const RtVector L, const RtNormal N, const RtVector V, const float roughness)
{
	RtVector H = {L[0] + V[0], L[1] + V[1], L[2] + V[2]};
	vnormalize(H, H);
	const RtFloat ir = (roughness == 0.f) ? 1.f : (8.f/roughness);
	r[0] = r[1] = r[2] = powf(MAX2(0.f, MIN2(1.f, N[0]*H[0] + N[1]*H[1] + N[2]*H[2])), ir);
}
RtVoid sl_SpecularBrdf(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtVector *op1 = context.getVector(2), *op2 = context.getVector(3), *op3 = context.getVector(4);
	const RtFloat* op4 = context.getFloat(5);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3), op3i = context.isVarying(4), op4i = context.isVarying(5);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0, op4o=0; i<n; ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i, op4o+=op4i)
			if (context.active(i))
				specularbrdf(lhs[i], op1[op1o], op2[op2o], op3[op3o], op4[op4o]);
	}
	else if (context.some_active()) {
		assert((op1i | op2i | op3i | op4i) == 0);
		specularbrdf(lhs[0], op1[0], op2[0], op3[0], op4[0]);
	}
	context.nextInstruction();
}

//---------------------------------------------------------------------------------------------------

// float1 = depth(point2)
RtVoid sl_Depth(SlRunContext& context)
{
	// lhs and arg must both be varying
	assert(context.isVarying(1));
	assert(context.isVarying(2));

	RtFloat* d = context.getFloat(1);
	RtPoint* P = (RtPoint*)context.getVector(2);

	// since points are in camspace, only need to copy P.z divided by far clip plane distance
	const float dfar = 1.f / State::Instance()->topOptions().farClip;
	
	const int n = context.SIMD_count;
	for (int i = 0; i<n; ++i)
		if (context.active(i))
			d[i] = P[i][2]*dfar;

	context.nextInstruction();
}

// float1 = distance(point2, point3)
inline void ridistance(float& r, const RtPoint &a, const RtPoint &b)
{
	const float i = a[0] - b[0], j = a[1] - b[1], k = a[2] - b[2];
	r = sqrtf(i*i + j*j + k*k);
}
OP_FUNC3( sl_Distance,
          Float, Vector, Vector,
          ridistance )

// calculates normals of grid using central differencing on the interior points,
// and either second order back- or forward differencing at the border points.
// Because of this, gridsize *must* be at least 2X2 faces, or 3X3 verts
RtVoid sl_Calcnormal(SlRunContext& context)
{
	// lhs and arg must both be varying
	assert(context.isVarying(1) and context.isVarying(2));

	RtVector* lhs = context.getVector(1);
	const RtPoint* pt = context.getVector(2);

	// if in a displacement shader and Ng needs to be updated, and here the normal N is calculated from P,
	// then all that is needed is to copy N to Ng after N is calculated, that way no double work is done.
	bool N_2_Ng = ((context.shdInstance->shader->update_Ng) and
	               (lhs == (RtVector*)context.grid->findVariable("N")) and (pt == (RtPoint*)context.grid->findVariable("P")));

	// NOTE: must alloc temporary normal grid if writing to self! (can be optimized though TODO)
	const char* tempname = NULL;
	if (lhs == pt) {
		tempname = "#tempnormal";	// '#' symbol cannot be used by shader vars so is 'safe'
		lhs = (RtVector*)context.grid->addVariable(tempname, 3);
	}

	// calculate the normals
	context.grid->calculateNormals(lhs, pt);

	if (N_2_Ng) {
		RtNormal* Ng_grid = (RtNormal*)context.grid->findVariable("Ng");
		memcpy(Ng_grid, lhs, sizeof(RtNormal)*context.SIMD_count);
		context.grid->Ng_updated = true;
	}

	// in case temporary grid used, now can copy to actual lhs and delete tempgrid
	if (tempname) {
		memcpy(context.getVector(1), lhs, sizeof(RtVector)*context.SIMD_count);
		context.grid->deleteVariable(tempname);
	}

	context.nextInstruction();
}

// calculates derivative of float value in u direction, same limitations as in sl_Calcnormal() above
RtVoid sl_Du_F(SlRunContext& context)
{
	// lhs and arg must both be varying
	assert(context.isVarying(1) && context.isVarying(2));

	RtFloat* lhs = context.getFloat(1);
	const RtFloat* val = context.getFloat(2);
	// it's possible that lhs == val, create temporary grid in that case
	const char* tempname = NULL;
	if (lhs == val) {
		tempname = "#tempduf";
		lhs = context.grid->addVariable(tempname, 1);
	}

	context.grid->DuF(lhs, val);

	if (tempname) {
		memcpy(context.getFloat(1), lhs, sizeof(RtFloat)*context.SIMD_count);
		context.grid->deleteVariable(tempname);
	}

	context.nextInstruction();
}

// calculates derivative of float value in v direction, same limitations as in sl_Calcnormal() above
RtVoid sl_Dv_F(SlRunContext& context)
{
	// lhs and arg must both be varying
	assert(context.isVarying(1) && context.isVarying(2));

	RtFloat* lhs = context.getFloat(1);
	const RtFloat* val = context.getFloat(2);
	// it's possible that lhs == val, create temporary grid in that case
	const char* tempname = NULL;
	if (lhs == val) {
		tempname = "#tempdvf";
		lhs = context.grid->addVariable(tempname, 1);
	}

	context.grid->DvF(lhs, val);

	if (tempname) {
		memcpy(context.getFloat(1), lhs, sizeof(RtFloat)*context.SIMD_count);
		context.grid->deleteVariable(tempname);
	}

	context.nextInstruction();
}

RtVoid sl_Du_V(SlRunContext& context)
{
	// lhs and arg must both be varying
	assert(context.isVarying(1) && context.isVarying(2));

	RtVector* lhs = context.getVector(1);
	const RtVector* val = context.getVector(2);
	// it's possible that lhs == val, create temporary grid in that case
	const char* tempname = NULL;
	if (lhs == val) {
		tempname = "#tempduv";
		lhs = (RtVector*)context.grid->addVariable(tempname, 3);
	}

	context.grid->DuV(lhs, val);

	if (tempname) {
		memcpy(context.getVector(1), lhs, sizeof(RtVector)*context.SIMD_count);
		context.grid->deleteVariable(tempname);
	}

	context.nextInstruction();
}

RtVoid sl_Dv_V(SlRunContext& context)
{
	// lhs and arg must both be varying
	assert(context.isVarying(1) && context.isVarying(2));

	RtVector* lhs = context.getVector(1);
	const RtVector* val = context.getVector(2);
	// it's possible that lhs == val, create temporary grid in that case
	const char* tempname = NULL;
	if (lhs == val) {
		tempname = "#tempdvv";
		lhs = (RtVector*)context.grid->addVariable(tempname, 3);
	}

	context.grid->DvV(lhs, val);

	if (tempname) {
		memcpy(context.getVector(1), lhs, sizeof(RtVector)*context.SIMD_count);
		context.grid->deleteVariable(tempname);
	}

	context.nextInstruction();
}

RtVoid sl_Deriv_F(SlRunContext& context)
{
	// all args must be varying
	assert(context.isVarying(1) && context.isVarying(2) && context.isVarying(3));
	context.grid->DerivF(context.getFloat(1), context.getFloat(2), context.getFloat(3));
	context.nextInstruction();
}

RtVoid sl_Deriv_V(SlRunContext& context)
{
	// all args must be varying
	assert(context.isVarying(1) && context.isVarying(2) && context.isVarying(3));
	context.grid->DerivV(context.getVector(1), context.getVector(2), context.getFloat(3));
	context.nextInstruction();
}

// float1 = area(point2)
RtVoid sl_Area(SlRunContext& context)
{
	// lhs and arg must both be varying
	assert(context.isVarying(1) and context.isVarying(2));
	context.grid->Area(context.getFloat(1), context.getVector(2));
	context.nextInstruction();
}

RtVoid sl_Faceforward1(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtVector* op1 = context.getVector(2);
	const RtVector* op2 = context.getVector(3);
	// need Ng variable from grid
	RtVector* Ng = (RtVector*)context.grid->findVariable("Ng");
	assert(Ng != NULL);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				faceforward(lhs[i], op1[op1o], op2[op2o], Ng[i]);
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		faceforward(lhs[0], op1[0], op2[0], Ng[0]);
	}
	context.nextInstruction();
}

OP_FUNC4( sl_Faceforward2,
          Vector, Vector, Vector, Vector,
          faceforward )

OP_FUNC2( sl_Normalize,
          Vector, Vector,
          vnormalize )

OP_FUNC3( sl_Step,
          Float, Float, Float,
          step )

//----------------------------------------------------------------------------------------
// filterstep()
// just the simple example box filter based function from AR, no parameter list yet
inline void filteredstep(float& r, const float edge, const float x, const float w)
{
	r = CLAMP((x + w*0.5f - edge)/w, 0.f, 1.f);
}

RtVoid sl_Filterstep1(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	float uvl[4];
	context.grid->getParent()->get_uvlim(uvl);
	const float du = (uvl[1] - uvl[0])/context.grid->get_xdim();
	const float dv = (uvl[3] - uvl[2])/context.grid->get_ydim();
	const float w = sqrtf(du*du + dv*dv);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				filteredstep(lhs[i], op1[op1o], op2[op2o], w);
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		step(lhs[0], op1[0], op2[0]);
	}
	context.nextInstruction();
}

// no idea if this one is correct, just a guess...
RtVoid sl_Filterstep2(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3), *op3 = context.getFloat(4);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3), op3i = context.isVarying(4);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n; ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i))
				filteredstep(lhs[i], op1[op1o], op2[op2o], ABS(op3[op3o] - op2[op2o]));
	}
	else if (context.some_active()) {
		assert((op1i | op2i | op3i) == 0);
		filteredstep(lhs[0], op1[0], op2[0], ABS(op3[0] - op2[0]));
	}
	context.nextInstruction();
}

//----------------------------------------------------------------------------------------

OP_FUNC4( sl_Smoothstep,
          Float, Float, Float, Float,
          smoothstep )

OP_FUNC3( sl_Reflect,
          Vector, Vector, Vector,
          reflect )

OP_FUNC4( sl_Refract,
          Vector, Vector, Vector, Float,
          refract )

//----------------------------------------------------------------------------------------
// Spline interpolation functions

RtVoid sl_Spline1_F(SlRunContext& context)
{
	// TODO
	context.nextInstruction();
}

RtVoid sl_Spline2_F(SlRunContext& context)
{
	// TODO
	context.nextInstruction();
}

RtVoid sl_Spline1_V(SlRunContext& context)
{
	// need at least 4 values to interpolate, ignore for now
	// can be a single array argument TODO
	const int numvals = context.getOperandCount() - 2;
	if (numvals < 4) {
		printf("sl_Spline1_V(): Need at least 4 values to interpolate!n");
		return;
	}

	RtVector* lhs = context.getVector(1);
	RtFloat* op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	RtVector** vals = new RtVector*[numvals];
	RtInt* vinc = new RtInt[numvals];
	for (int i=0; i<numvals; ++i) {
		vals[i] = context.getVector(i + 3);
		vinc[i] = context.isVarying(i + 3);
	}

	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i) {
			if (context.active(i)) {
				float t = MAX2(0.f, MIN2(1.f, op1[op1o]))*(numvals - 3);
				int idx = int(t);
				t -= float(idx);
				if (idx == (numvals - 3)) idx = numvals - 4, t = 1.f;
				SplineV(lhs[i], t, RiCatmullRomBasis, *vals[idx], *vals[idx+1], *vals[idx+2], *vals[idx+3]);
			}
			for (int j=0; j<numvals; ++j)
				vals[j] += vinc[j];
		}
	}
	else if (context.some_active()) {
		for (int j=0; j<numvals; ++j)
			assert(vinc[j] == 0);
		const float t = MAX2(0.f, MIN2(1.f, op1[0]));
		const int idx = int((numvals - 4)*t);
		SplineV(lhs[0], t, RiCatmullRomBasis, *vals[idx], *vals[idx+1], *vals[idx+2], *vals[idx+3]);
	}

	delete[] vinc;
	delete[] vals;
	context.nextInstruction();
}

RtVoid sl_Spline2_V(SlRunContext& context)
{
	// TODO
	context.nextInstruction();
}

//----------------------------------------------------------------------------------------
// Fresnel functions
// NOTE these are a bit exceptional compared to the other functions,
// since they can have either two or four return values

RtVoid sl_Fresnel1(SlRunContext& context)
{
	RtFloat *lhs1 = context.getFloat(1), *lhs2 = context.getFloat(2);
	const RtVector *op1 = context.getVector(3), *op2 = context.getVector(4);
	const RtFloat *op3 = context.getFloat(5);
	const int op1i = context.isVarying(3), op2i = context.isVarying(4), op3i = context.isVarying(5);
	if (context.isVarying(1)) {
		// if lhs1 varying then lhs2 must be varying too
		assert(context.isVarying(2));
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n; ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i))
				fresnel(op1[op1o], op2[op2o], op3[op3o], lhs1[i], lhs2[i]);
	}
	else if (context.some_active()) {
		// if lhs1 uniform then lhs2 must be uniform too
		assert(not context.isVarying(2));
		assert((op1i | op2i | op3i) == 0);
		fresnel(op1[0], op2[0], op3[0], lhs1[0], lhs2[0]);
	}
	context.nextInstruction();
}

RtVoid sl_Fresnel2(SlRunContext& context)
{
	RtFloat *lhs1 = context.getFloat(1), *lhs2 = context.getFloat(2);
	RtVector *lhs3 = context.getVector(3), *lhs4 = context.getVector(4);
	const RtVector *op1 = context.getVector(5), *op2 = context.getVector(6);
	const RtFloat *op3 = context.getFloat(7);
	const int op1i = context.isVarying(5), op2i = context.isVarying(6), op3i = context.isVarying(7);
	if (context.isVarying(1)) {
		// if lhs1 varying then lhs2/3/4 must be varying too
		assert(context.isVarying(2) and context.isVarying(3) and context.isVarying(4));
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0, op3o=0; i<n; ++i, op1o+=op1i, op2o+=op2i, op3o+=op3i)
			if (context.active(i))
				fresnel(op1[op1o], op2[op2o], op3[op3o], lhs1[i], lhs2[i], lhs3[i], lhs4[i]);
	}
	else if (context.some_active()) {
		// if lhs1 uniform then lhs2/3/4 must be uniform too
		assert((context.isVarying(2) | context.isVarying(3) | context.isVarying(4)) == 0);
		assert((op1i | op2i | op3i) == 0);
		fresnel(op1[0], op2[0], op3[0], lhs1[0], lhs2[0], lhs3[0], lhs4[0]);
	}
	context.nextInstruction();
}

//----------------------------------------------------------------------------------------

OP_FUNC4( sl_Mixf,
          Float, Float, Float, Float,
          mixf )

OP_FUNC4( sl_Mixv,
          Vector, Vector, Vector, Float,
          mixv )

OP_FUNC2( sl_Radians,
          Float, Float,
          radians )

OP_FUNC2( sl_Degrees,
          Float, Float,
          degrees )


// probably easier to implement this by only replacing the sl-specific format flags in a string and just printf() it directly TODO
// Well, partly done, but no idea how to make the argument list...
#if 0
RtVoid sl_Printf(SlRunContext& context)
{
	RtInt numargs = context.getOperandCount() + 1;	// must be at least 2, 'printf' plus format string
	RtString str = context.getString(1)[0];	// format string
	int argcnt = 2;	// start after the format string
	// test if any varyings arguments
	bool vary = false;
	for (int i=argcnt; i<numargs; ++i)
		if (context.isVarying(i)) { vary = true; break; }
	// scan the format string for escape chars or sl specific format chars
	RtString tstr = str;
	size_t numchars = 0;
	while (*tstr) {
		if (*tstr == '%') {
			tstr++;
			if (*tstr == 0) break;
			switch (*tstr)
			{
				case 'p':
				case 'c':
				{
					numchars += 10;
					break;
				}
				case 'm':
				{
					numchars += 54;
					break;
				}
				default:
					numchars += 1;
			}
		}
		else if (*tstr == '\\') {	// escape-sequence
			tstr++;
			if (*tstr == 0) break;
			switch (*tstr)
			{
				case 'a':
				case 'b':
				case 'f':
				case 'n':
				case 'r':
				case 't':
				case 'v':
				case '\\':
				case '\"':
				case '\'':
				case '?':
					numchars += 1;
				default:
					// ignore everything else
					break;
			}
		}
		else numchars += 1;
		tstr++;
	}
	// replace with required format strings
	numchars += 1;
	char* nstr = new char[numchars];
	nstr[0] = 0;
	while (*str) {
		if (*str == '%') {
			str++;
			if (*str == 0) break;
			switch (*str)
			{
				case 'p':
				case 'c':
					strcat(nstr, "[%f %f %f]");
					break;
				case 'm':
					strcat(nstr, "[[%f %f %f %f][%f %f %f %f][%f %f %f %f][%f %f %f %f]]");
					break;
				default:
					strcat(nstr, "%");
					strncat(nstr, str, 1);
			}
		}
		else if (*str == '\\') {
			// escape-sequence
			str++;
			if (*str == 0) break;
			switch (*str)
			{
				case 'a':
					strcat(nstr, "\a");
					break;
				case 'b':
					strcat(nstr, "\b");
					break;
				case 'f':
					strcat(nstr, "\f");
					break;
				case 'n':
					strcat(nstr, "\n");
					break;
				case 'r':
					strcat(nstr, "\r");
					break;
				case 't':
					strcat(nstr, "\t");
					break;
				case 'v':
					strcat(nstr, "\v");
					break;
				case '\\':
					strcat(nstr, "\\");
					break;
				case '\"':
					strcat(nstr, "\"");
					break;
				case '\'':
					strcat(nstr, "\'");
					break;
				case '?':
					strcat(nstr, "\?");
					break;
				default:
					// ignore everything else
					break;
			}
		}
		else
			strncat(nstr, str, 1);
		str++;
	}
	// yes ok, got the fmt string, but now what, how do I make a list of the arguments !?!?!
	printf(nstr, ARGSHERE);
	delete[] nstr;
	context.nextInstruction();
}
#endif

void _sl_printf(SlRunContext& context, const char* str, int curidx, int maxargs)
{
	int argcnt = 2;	// start after the format string
	while (*str) {
		if (*str == '%') {
			if (argcnt >= maxargs) {
				printf("(no more args)");
				str++;
				break;
			}
			str++;
			if (*str == 0) break;
			switch (*str)
			{
				case 'f':
				{
					printf("%f", context.getFloat(argcnt)[context.isVarying(argcnt) ? curidx : 0]);
					break;
				}
				case 'g':
				{
					// not required, but useful, same as in c/c++, prints float using exponent if required
					printf("%g", context.getFloat(argcnt)[context.isVarying(argcnt) ? curidx : 0]);
					break;
				}
				case 'p':
				case 'c':
				{
					RtVector& vp = context.getVector(argcnt)[context.isVarying(argcnt) ? curidx : 0];
					printf("[%f %f %f]", vp[0], vp[1], vp[2]);
					break;
				}
				case 'm':
				{
					RtMatrix& mp = context.getMatrix(argcnt)[context.isVarying(argcnt) ? curidx : 0];
					printf("[[%f %f %f %f][%f %f %f %f][%f %f %f %f][%f %f %f %f]",
									mp[0][0], mp[1][0], mp[2][0], mp[3][0],
									mp[0][1], mp[1][1], mp[2][1], mp[3][1],
									mp[0][2], mp[1][2], mp[2][2], mp[3][2],
									mp[0][3], mp[1][3], mp[2][3], mp[3][3]);
					break;
				}
				case 's':
					printf("%s", context.getString(argcnt)[0]);	// always constant
					break;
				case '%':
					putchar('%');
				// everything else ignored
			}
			argcnt++;
		}
		else if (*str == '\\') {
			// escape-sequence
			str++;
			if (*str == 0) break;
			switch (*str)
			{
				case 'a':
					putchar('\a');
					break;
				case 'b':
					putchar('\b');
					break;
				case 'f':
					putchar('\f');
					break;
				case 'n':
					putchar('\n');
					break;
				case 'r':
					putchar('\r');
					break;
				case 't':
					putchar('\t');
					break;
				case 'v':
					putchar('\v');
					break;
				case '\\':
					putchar('\\');
					break;
				case '\"':
					putchar('\"');
					break;
				case '\'':
					putchar('\'');
					break;
				case '?':
					putchar('\?');
					break;
				default:
					// ignore everything else
					break;
			}
		}
		else
			putchar(*str);
		str++;
	}
}

#if 1
RtVoid sl_Printf(SlRunContext& context)
{
	RtInt numargs = context.getOperandCount() + 1;	// must be at least 2, 'printf' plus format string
	RtString fmt = context.getString(1)[0];	// format string
	// test if any varying arguments
	bool vary = false;
	for (int i = 2; i<numargs; ++i)
		if (context.isVarying(i)) { vary = true;  break; }
	if (vary) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				_sl_printf(context, fmt, i, numargs);
	}
	else if (context.some_active())
		_sl_printf(context, fmt, 0, numargs);
	context.nextInstruction();
}
#endif


// NOTE noise functions are not optimized for the non-3D cases TODO

// float1 = Noise(float2)
RtVoid sl_Noise1_F(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat* op1 = context.getFloat(2);
	newPerlin_t ngen;
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = ngen(Point3(op1[op1o], 0.f, 0.f));
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = ngen(Point3(op1[0], 0.f, 0.f));
	}
	context.nextInstruction();
}

// float1 = Noise(vector2)
RtVoid sl_Noise3_F(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtVector* op1 = context.getVector(2);
	newPerlin_t ngen;
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = ngen(Point3(op1[op1o][0], op1[op1o][1], op1[op1o][2]));
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = ngen(Point3(op1[0][0], op1[0][1], op1[0][2]));
	}
	context.nextInstruction();
}

// vector1 = Noise(float2)
RtVoid sl_Noise1_V(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtFloat* op1 = context.getFloat(2);
	newPerlin_t ngen;
	const int op1i = context.isVarying(2);
	const Point3 nofs(2.78, 3.14, 1.41);	// arbitrary noise offset
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i)) {
				const Point3 tp(op1[op1o], 0.f, 0.f);
				lhs[i][0] = ngen(tp);
				lhs[i][1] = ngen(tp + nofs);
				lhs[i][2] = ngen(tp - nofs);
			}
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		const Point3 tp(op1[0], 0.f, 0.f);
		lhs[0][0] = ngen(tp);
		lhs[0][1] = ngen(tp + nofs);
		lhs[0][2] = ngen(tp - nofs);
	}
	context.nextInstruction();
}

RtVoid sl_Noise3_V(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtVector* op1 = context.getVector(2);
	newPerlin_t ngen;
	const int op1i = context.isVarying(2);
	const Point3 nofs(2.78, 3.14, 1.41);	// arbitrary noise offset
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i)) {
				const Point3 tp(op1[op1o][0], op1[op1o][1], op1[op1o][2]);
				lhs[i][0] = ngen(tp);
				lhs[i][1] = ngen(tp + nofs);
				lhs[i][2] = ngen(tp - nofs);
			}
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		const Point3 tp(op1[0][0], op1[0][1], op1[0][2]);
		lhs[0][0] = ngen(tp);
		lhs[0][1] = ngen(tp + nofs);
		lhs[0][2] = ngen(tp - nofs);
	}
	context.nextInstruction();
}


// float1 = Noise(mod(float2, float3))
RtVoid sl_Pnoise1_F(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);
	newPerlin_t ngen;
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				lhs[i] = ngen(Point3(FMOD(op1[op1o], op2[op2o]), 0.f, 0.f));
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		lhs[0] = ngen(Point3(FMOD(op1[0], op2[0]), 0.f, 0.f));
	}
	context.nextInstruction();
}

RtVoid sl_Cellnoise1_F(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat* op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = cellNoiseColor(Point3(op1[op1o], 0.f, 0.f)).r;
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = cellNoiseColor(Point3(op1[0], 0.f, 0.f)).r;
	}
	context.nextInstruction();
}

RtVoid sl_Cellnoise2_F(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				lhs[i] = cellNoiseColor(Point3(op1[op1o], op2[op2o], 0.f)).r;
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		lhs[0] = cellNoiseColor(Point3(op1[0], op2[0], 0.f)).r;
	}
	context.nextInstruction();
}


RtVoid sl_Cellnoise1_V(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtFloat* op1 = context.getFloat(2);
	const int op1i = context.isVarying(2);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i)) {
				const Color c = cellNoiseColor(Point3(op1[op1o], 0.f, 0.f));
				lhs[i][0] = c.r, lhs[i][1] = c.g, lhs[i][2] = c.b;
			}
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		const Color c = cellNoiseColor(Point3(op1[0], 0.f, 0.f));
		lhs[0][0] = c.r, lhs[0][1] = c.g, lhs[0][2] = c.b;
	}
	context.nextInstruction();
}

RtVoid sl_Cellnoise2_V(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	const RtFloat *op1 = context.getFloat(2), *op2 = context.getFloat(3);
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i)) {
				const Color c = cellNoiseColor(Point3(op1[op1o], op2[op2o], 0.f));
				lhs[i][0] = c.r, lhs[i][1] = c.g, lhs[i][2] = c.b;
			}
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		const Color c = cellNoiseColor(Point3(op1[0], op2[0], 0.f));
		lhs[0][0] = c.r, lhs[0][1] = c.g, lhs[0][2] = c.b;
	}
	context.nextInstruction();
}

// ridged fractal noise, not standard SL function (usually itself implemented in SL)
// this could be implemented as general 'fractalnoise' function, using parameter lists, TODO
RtVoid sl_Fn_ridged(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtVector* op1 = context.getVector(2);
	const int op1i = context.isVarying(2);

	const RtFloat H = *context.getFloat(3);
	const RtFloat lacunarity = *context.getFloat(4);
	const RtFloat octaves = *context.getFloat(5);
	const RtFloat offset = *context.getFloat(6);
	const RtFloat gain = *context.getFloat(7);

	newPerlin_t ngen;
	ridgedMFractal_t fn(H, lacunarity, octaves, offset, gain, &ngen);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = fn(Point3(op1[op1o][0], op1[op1o][1], op1[op1o][2]));
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = fn(Point3(op1[0][0], op1[0][1], op1[0][2]));
	}
	context.nextInstruction();
}


RtVoid sl_Fn_hetero(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtVector* op1 = context.getVector(2);
	const int op1i = context.isVarying(2);

	const RtFloat H = *context.getFloat(3);
	const RtFloat lacunarity = *context.getFloat(4);
	const RtFloat octaves = *context.getFloat(5);
	const RtFloat offset = *context.getFloat(6);

	newPerlin_t ngen;
	heteroTerrain_t fn(H, lacunarity, octaves, offset, &ngen);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = fn(Point3(op1[op1o][0], op1[op1o][1], op1[op1o][2]));
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = fn(Point3(op1[0][0], op1[0][1], op1[0][2]));
	}
	context.nextInstruction();
}

RtVoid sl_Fn_fbm(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	const RtVector* op1 = context.getVector(2);
	const int op1i = context.isVarying(2);

	const RtFloat H = *context.getFloat(3);
	const RtFloat lacunarity = *context.getFloat(4);
	const RtFloat octaves = *context.getFloat(5);

	newPerlin_t ngen;
	fBm_t fn(H, lacunarity, octaves, &ngen);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0; i<n; ++i, op1o+=op1i)
			if (context.active(i))
				lhs[i] = fn(Point3(op1[op1o][0], op1[op1o][1], op1[op1o][2]));
	}
	else if (context.some_active()) {
		assert(op1i == 0);
		lhs[0] = fn(Point3(op1[0][0], op1[0][1], op1[0][2]));
	}
	context.nextInstruction();
}

// only difference with Ptransform3() below is a matrix inversion
RtVoid sl_Ptocurr(SlRunContext& context)
{
	// currently only 'current' (=='camera'), 'shader', 'object'
	// & (possibly userdef.) coordsystems recognized
	RtPoint* lhs = (RtPoint*)context.getVector(1);
	RtString op1 = context.getString(2)[0];
	// nothing to do if xform to 'current' space
	if ((!strcmp(op1, "current")) || (!strcmp(op1, "camera"))) {
		context.nextInstruction();
		return;
	}
	const RtPoint* op2 = (RtPoint*)context.getVector(3);
	RtMatrix xform;
	if (!strcmp(op1, "shader")) // current to shader
		memcpy(xform, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
	else if (!strcmp(op1, "object")) // current to object
		memcpy(xform, context.grid->get_parent()->getTransformReference()->getRtMatrixPtr(), sizeof(RtMatrix));
	else {
		// try named coordsys
		Transform* cs = State::Instance()->getNamedCoordSys(op1);
		if (cs==NULL) {	// unknown, skip
			printf("sl_Ptocurr(): Unknown space '%s'\n", op1);
			context.nextInstruction();
			return;
		}
		memcpy(xform, cs->getRtMatrixPtr(), sizeof(RtMatrix));
	}
	// ok
	const int op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				mulPMP(lhs[i], xform, op2[op2o]);
	}
	else if (context.some_active()) {
		assert(op2i == 0);
		mulPMP(lhs[0], xform, op2[0]);
	}
	context.nextInstruction();
}

// only difference with Vtransform3() below is a matrix inversion
RtVoid sl_Vtocurr(SlRunContext& context)
{
	// currently only 'current' (=='camera'), 'shader', 'object'
	// & (possibly userdef.) coordsystems recognized
	RtVector* lhs = context.getVector(1);
	RtString op1 = context.getString(2)[0];
	// nothing to do if xform to 'current' space
	if ((!strcmp(op1, "current")) || (!strcmp(op1, "camera"))) {
		context.nextInstruction();
		return;
	}
	const RtVector* op2 = context.getVector(3);
	RtMatrix xform;
	if (!strcmp(op1, "shader")) // current to shader
		memcpy(xform, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
	else if (!strcmp(op1, "object")) // current to object
		memcpy(xform, context.grid->get_parent()->getTransformReference()->getRtMatrixPtr(), sizeof(RtMatrix));
	else {
		// try named coordsys
		Transform* cs = State::Instance()->getNamedCoordSys(op1);
		if (cs==NULL) {	// unknown, skip
			printf("sl_Vtocurr(): Unknown space '%s'\n", op1);
			context.nextInstruction();
			return;
		}
		memcpy(xform, cs->getRtMatrixPtr(), sizeof(RtMatrix));
	}
	// ok
	const int op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				mulVMV(lhs[i], xform, op2[op2o]);
	}
	else if (context.some_active()) {
		assert(op2i == 0);
		mulVMV(lhs[0], xform, op2[0]);
	}
	context.nextInstruction();
}

RtVoid sl_Ptransform1(SlRunContext& context)
{
	// currently only 'current' (=='camera'), 'shader', 'object'
	// & (possibly userdef.) coordsystems recognized
	RtPoint* lhs = (RtPoint*)(context.getVector(1));
	RtString op1 = (char*)context.getString(2)[0];
	// nothing to do if xform to 'current' space
	if ((!strcmp(op1, "current")) || (!strcmp(op1, "camera"))) {
		context.nextInstruction();
		return;
	}
	const RtPoint* op2 = (RtPoint*)(context.getVector(3));
	RtMatrix xform;
	if (!strcmp(op1, "shader")) // current to shader
		memcpy(xform, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
	else if (!strcmp(op1, "object")) // current to object
		memcpy(xform, context.grid->get_parent()->getTransformReference()->getRtMatrixPtr(), sizeof(RtMatrix));
	else {
		// try named coordsys
		Transform* cs = State::Instance()->getNamedCoordSys(op1);
		if (cs==NULL) {	// unknown, skip
			printf("sl_Ptransform1(): Unknown space '%s'\n", op1);
			context.nextInstruction();
			return;
		}
		memcpy(xform, cs->getRtMatrixPtr(), sizeof(RtMatrix));
	}
	// ok
	invertMatrix(xform, xform); // from current to requested space
	const int op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				mulPMP(lhs[i], xform, op2[op2o]);
	}
	else if (context.some_active()) {
		assert(op2i == 0);
		mulPMP(lhs[0], xform, op2[0]);
	}
	context.nextInstruction();
}

RtVoid sl_Ptransform2(SlRunContext& context)
{
	RtPoint* lhs = (RtPoint*)(context.getVector(1));
	RtString op1 = context.getString(2)[0];
	RtString op2 = context.getString(3)[0];
	const RtPoint* op3 = (RtPoint*)(context.getVector(4));
	RtMatrix xform1;
	// only 'shader' or 'object' to 'current' recognized for now
	if ((!strcmp(op1, "shader")) && (!strcmp(op2, "current")))
		memcpy(xform1, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
	else if ((!strcmp(op1, "object")) && (!strcmp(op2, "current")))
		memcpy(xform1, context.grid->getParent()->getTransformReference()->getRtMatrixPtr(), sizeof(RtMatrix));
	else if ((!strcmp(op1, "NDC")) && (!strcmp(op2, "screen"))) {
		Transform cs = *State::Instance()->getNamedCoordSys(op1);
		memcpy(xform1, cs.getRtMatrixPtr(), sizeof(RtMatrix));
		cs = *State::Instance()->getNamedCoordSys(op2);
		cs.invert();
		mulMMM(xform1, *cs.getRtMatrixPtr(), xform1);
	}
	else if ((!strcmp(op1, "NDC")) && (!strcmp(op2, "camera"))) {
		Transform cs = *State::Instance()->getNamedCoordSys(op1);
		memcpy(xform1, cs.getRtMatrixPtr(), sizeof(RtMatrix));
		cs = *State::Instance()->getNamedCoordSys(op2);
		cs.invert();
		mulMMM(xform1, *cs.getRtMatrixPtr(), xform1);
	}
	else {
		printf("sl_Ptransform2(): Unknown space '%s' to '%s'\n", op1, op2);
		context.nextInstruction();
		return;
	}
	const int op3i = context.isVarying(4);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op3o=0; i<n; ++i, op3o+=op3i)
			if (context.active(i))
				mulPMP(lhs[i], xform1, op3[op3o]);
	}
	else if (context.some_active()) {
		assert(op3i == 0);
		mulPMP(lhs[0], xform1, op3[0]);
	}
	context.nextInstruction();
}

RtVoid sl_Ptransform3(SlRunContext& context)
{
	RtPoint* lhs = (RtPoint*)(context.getVector(1));
	const RtMatrix* op1 = context.getMatrix(2);
	const RtPoint* op2 = (RtPoint*)(context.getVector(3));
	const int op1i = context.isVarying(2), op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op1o=0, op2o=0; i<n; ++i, op1o+=op1i, op2o+=op2i)
			if (context.active(i))
				mulPMP(lhs[i], op1[op1o], op2[op2o]);
	}
	else if (context.some_active()) {
		assert((op1i | op2i) == 0);
		mulPMP(lhs[0], op1[0], op2[0]);
	}
	context.nextInstruction();
}

RtVoid sl_Vtransform1(SlRunContext& context)
{
	// currently only 'current' (=='camera'), 'shader', 'object'
	// & (possibly userdef.) coordsystems recognized
	RtVector* lhs = context.getVector(1);
	RtString op1 = (char*)context.getString(2)[0];
	// nothing to do if xform to 'current' space
	if ((!strcmp(op1, "current")) || (!strcmp(op1, "camera"))) {
		context.nextInstruction();
		return;
	}
	const RtVector* op2 = context.getVector(3);
	RtMatrix xform;
	if (!strcmp(op1, "shader")) // current to shader
		memcpy(xform, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
	else if (!strcmp(op1, "object")) // current to object
		memcpy(xform, context.grid->get_parent()->getTransformReference()->getRtMatrixPtr(), sizeof(RtMatrix));
	else {
		// try named coordsys
		Transform* cs = State::Instance()->getNamedCoordSys(op1);
		if (cs==NULL) {	// unknown, skip
			context.nextInstruction();
			printf("sl_Vtransform1(): Unknown space '%s'\n", op1);
			return;
		}
		memcpy(xform, cs->getRtMatrixPtr(), sizeof(RtMatrix));
	}
	// ok
	invertMatrix(xform, xform); // from current to requested space
	const int op2i = context.isVarying(3);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
			if (context.active(i))
				mulVMV(lhs[i], xform, op2[op2o]);
	}
	else if (context.some_active()) {
		assert(op2i == 0);
		mulVMV(lhs[0], xform, op2[0]);
	}
	context.nextInstruction();
}


RtVoid sl_Vtransform2(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	RtString op1 = context.getString(2)[0];
	RtString op2 = context.getString(3)[0];
	const RtVector* op3 = context.getVector(4);
	RtMatrix xform1;
	// only 'shader' or 'object' to 'current' recognized for now
	if ((!strcmp(op1, "shader")) && (!strcmp(op2, "current"))) {
		memcpy(xform1, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
		const int op3i = context.isVarying(4);
		if (context.isVarying(1)) {
			const int n = context.SIMD_count;
			for (int i=0, op3o=0; i<n; ++i, op3o+=op3i)
				if (context.active(i))
					mulVMV(lhs[i], xform1, op3[op3o]);
		}
		else if (context.some_active()) {
			assert(op3i == 0);
			mulVMV(lhs[0], xform1, op3[0]);
		}
	}
	else if ((!strcmp(op1, "object")) && (!strcmp(op2, "current"))) {
		memcpy(xform1, context.grid->getParent()->getTransformReference()->getRtMatrixPtr(), sizeof(RtMatrix));
		const int op3i = context.isVarying(4);
		if (context.isVarying(1)) {
			const int n = context.SIMD_count;
			for (int i=0, op3o=0; i<n; ++i, op3o+=op3i)
				if (context.active(i))
					mulVMV(lhs[i], xform1, op3[op3o]);
		}
		else if (context.some_active()){
			assert(op3i == 0);
			mulVMV(lhs[0], xform1, op3[0]);
		}
	}
	else
		printf("sl_Vtransform2(): Unknown space '%s' to '%s'\n", op1, op2);
	context.nextInstruction();
}

RtVoid sl_Ntransform2(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	RtString op1 = context.getString(2)[0];
	RtString op2 = context.getString(3)[0];
	const RtVector* op3 = context.getVector(4);
	RtMatrix xform1;
	// only 'shader' or 'object' to 'current' recognized for now
	if ((!strcmp(op1, "shader")) && (!strcmp(op2, "current"))) {
		memcpy(xform1, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
		const int op3i = context.isVarying(4);
		if (context.isVarying(1)) {
			const int n = context.SIMD_count;
			for (int i=0, op3o=0; i<n; ++i, op3o+=op3i)
				if (context.active(i))
					mulNMN(lhs[i], xform1, op3[op3o]);
		}
		else if (context.some_active()) {
			assert(op3i == 0);
			mulNMN(lhs[0], xform1, op3[0]);
		}
	}
	else if ((!strcmp(op1, "object")) && (!strcmp(op2, "current"))) {
		memcpy(xform1, context.grid->getParent()->getTransformReference()->getRtMatrixPtr(), sizeof(RtMatrix));
		const int op3i = context.isVarying(4);
		if (context.isVarying(1)) {
			const int n = context.SIMD_count;
			for (int i=0, op3o=0; i<n; ++i, op3o+=op3i)
				if (context.active(i))
					mulNMN(lhs[i], xform1, op3[op3o]);
		}
		else if (context.some_active()) {
			assert(op3i == 0);
			mulNMN(lhs[0], xform1, op3[0]);
		}
	}
	else
		printf("sl_Ntransform2(): Unknown space '%s' to '%s'\n", op1, op2);
	context.nextInstruction();
}

RtVoid sl_Mtransform3(SlRunContext& context)
{
	RtMatrix* lhs = context.getMatrix(1);
	RtString op1 = context.getString(2)[0];
	RtMatrix* op2 = context.getMatrix(3);
	RtMatrix xform;
	// only 'shader' recognized for now
	if (!strcmp(op1, "shader")) {
		memcpy(xform, context.shdInstance->xform->getRtMatrixPtr(), sizeof(RtMatrix));
		const int op2i = context.isVarying(3);
		if (context.isVarying(1)) {
			const int n = context.SIMD_count;
			for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
				if (context.active(i))
					mulMMM(lhs[i], xform, op2[op2o]);
		}
		else if (context.some_active()) {
			assert(op2i == 0);
			mulMMM(lhs[0], xform, op2[0]);
		}
	}
	context.nextInstruction();
}

//--------------------------------------------------------------------------------------------------
// color transforms

// currently only hsl supported
RtVoid sl_Ctransform1(SlRunContext& context)
{
	if (!strcmp(context.getString(2)[0], "hsl")) {
		RtVector *lhs = context.getVector(1), *op2 = context.getVector(3);
		const int op2i = context.isVarying(3);
		if (context.isVarying(1)) {
			const int n = context.SIMD_count;
			for (int i=0, op2o=0; i<n; ++i, op2o+=op2i)
				if (context.active(i)) {
					const Color HSL = rgb2hsl(Color(op2[op2o][0], op2[op2o][1], op2[op2o][2]));
					lhs[i][0] = HSL[0], lhs[i][1] = HSL[1], lhs[i][2] = HSL[2];
				}
		}
		else if (context.some_active()) {
			assert(op2i == 0);
			const Color HSL = rgb2hsl(Color(op2[0][0], op2[0][1], op2[0][2]));
			lhs[0][0] = HSL[0], lhs[0][1] = HSL[1], lhs[0][2] = HSL[2];
		}
	}
	context.nextInstruction();
}

// currently hsl to rgb only
RtVoid sl_Ctransform2(SlRunContext& context)
{
	if (!strcmp(context.getString(2)[0], "hsl") and !strcmp(context.getString(3)[0], "rgb")) {
		RtVector *lhs = context.getVector(1), *op3 = context.getVector(4);
		const int op3i = context.isVarying(4);
		if (context.isVarying(1)) {
			const int n = context.SIMD_count;
			for (int i=0, op3o=0; i<n; ++i, op3o+=op3i)
				if (context.active(i)) {
					const Color RGB = hsl2rgb(Color(op3[op3o][0], op3[op3o][1], op3[op3o][2]));
					lhs[i][0] = RGB[0], lhs[i][1] = RGB[1], lhs[i][2] = RGB[2];
				}
		}
		else if (context.some_active()) {
			assert(op3i == 0);
			const Color RGB = hsl2rgb(Color(op3[0][0], op3[0][1], op3[0][2]));
			lhs[0][0] = RGB[0], lhs[0][1] = RGB[1], lhs[0][2] = RGB[2];
		}
	}
	context.nextInstruction();
}
//--------------------------------------------------------------------------------------------------


// random numbers
RtVoid sl_RandomF(SlRunContext& context)
{
	RtFloat* lhs = context.getFloat(1);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				lhs[i] = frand();
	}
	else if (context.some_active())
		lhs[0] = frand();
	context.nextInstruction();
}

// random colors/points
RtVoid sl_RandomV(SlRunContext& context)
{
	RtVector* lhs = context.getVector(1);
	if (context.isVarying(1)) {
		const int n = context.SIMD_count;
		for (int i=0; i<n; ++i)
			if (context.active(i))
				lhs[i][0] = frand(), lhs[i][1] = frand(), lhs[i][2] = frand();
	}
	else if (context.some_active())
		lhs[0][0] = frand(), lhs[0][1] = frand(), lhs[0][2] = frand();
	context.nextInstruction();
}

// opcode table
// 'types' is a string of chars which specify the operand types,
//  in order of return type (if it has one), followed by operand-types.
// NOTE the fresnel function has in fact either 2 or 4 return values!
// for several opcodes this string is also part of the instruction.
// (eg. "mulvvf", vector result of a vector multiplied by float)
// string length of 'types' is also used to determine operand count
// f = float
// v = vector (also used for color/normal/point where applicable)
// p = point (only used for matrix funcs, everything else uses general vector version)
// n = normal (only used where the general vector routines are not applicable)
// c = color (only used where the general vector routines are not applicable)
// m = matrix
// s = string
// b = bool	(extra type for internal use only, not in shading language)
// special:
// L = label (branch instruction only)
// u = unknown number of args, variadic function, num of args determined at load time
_opcode opcodeTable[] = {
	// basic math ops
	// add
	{"addfff", "fff", sl_Add_FFF},
	{"addvvv", "vvv", sl_Add_VVV},
	{"addmmm", "mmm", sl_Add_MMM},
	// sub
	{"subfff", "fff", sl_Sub_FFF},
	{"subvvv", "vvv", sl_Sub_VVV},
	{"submmm", "mmm", sl_Sub_MMM},
	// mul
	{"mulfff", "fff", sl_Mul_FFF},
	{"mulvvv", "vvv",  sl_Mul_VVV},
	{"mulmmm", "mmm",  sl_Mul_MMM},
	{"mulvvf", "vvf",  sl_Mul_VVF},
	// div
	{"divfff", "fff", sl_Div_FFF},
	{"divvvv", "vvv", sl_Div_VVV},
	{"divmmm", "mmm", sl_Div_MMM},
	{"divvvf", "vvf", sl_Div_VVF},
	// madd (multiply & add)
	{"maddfff", "fff", sl_Madd_FFF},
	{"maddvvv", "vvv", sl_Madd_VVV},
	{"maddvvf", "vvf", sl_Madd_VVF},
	// msub (multiply & subtract)
	{"msubfff", "fff", sl_Msub_FFF},
	{"msubvvv", "vvv", sl_Msub_VVV},
	{"msubvvf", "vvf", sl_Msub_VVF},
	// negate
	{"negff", "ff", sl_Neg_FF},
	{"negvv", "vv", sl_Neg_VV},
	// math funcs
	{"abs", "ff", sl_Abs},
	{"acos", "ff", sl_Acos},
	{"asin", "ff", sl_Asin},
	{"atan1", "ff", sl_Atan1},
	{"atan2", "fff", sl_Atan2},
	{"ceil", "ff", sl_Ceil},
	{"clampf", "ffff", sl_Clampf},
	{"clampv", "vvvv", sl_Clampv},
	{"degrees", "ff", sl_Degrees},
	{"sin", "ff", sl_Sin},
	{"cos", "ff", sl_Cos},
	{"tan", "ff", sl_Tan},
	{"sign", "ff", sl_Sign},
	{"sqrt", "ff", sl_Sqrt},
	{"invsqrt", "ff", sl_Invsqrt},
	{"exp", "ff", sl_Exp},
	{"floor", "ff", sl_Floor},
	{"log", "ff", sl_Log},
	{"logb", "fff", sl_Logb},
	{"minf", "fff", sl_Minf},
	//{"minv", "vvv", sl_Minv},
	{"maxf", "fff", sl_Maxf},
	//{"maxv", "vvv", sl_Maxv},
	{"mod", "fff", sl_Mod},
	{"pow", "fff", sl_Pow},
	{"radians", "ff", sl_Radians},
	// vector cross- & dot-product
	{"vcross", "vvv", sl_Vcross_VVV},
	{"vdot", "fvv", sl_Vdot_FVV},
	// copy/assignment
	{"movff", "ff", sl_Mov_FF},
	{"movvv", "vv", sl_Mov_VV},
	{"movvf", "vf", sl_Mov_VF},
	{"movmm", "mm", sl_Mov_MM},
	{"movmf", "mf", sl_Mov_MF},
	{"movvf3", "vfff", sl_Mov_VF3},
	{"movmf16", "mffffffffffffffff", sl_Mov_MF16},
	// copy/assigment *FROM* an array
	{"movfaff", "fff", sl_Movfa_FF},
	{"movfavv", "vvf", sl_Movfa_VV},
	// copy/assigment *TO* an array
	{"movtaff", "fff", sl_Movta_FF},
	{"movtavv", "vfv", sl_Movta_VV},
	{"movtavf3", "vffff", sl_Movta_VF3},
	// conditional copy/assignment
	{"cmovf", "fbff", sl_Cmov_F},
	{"cmovv", "vbvv", sl_Cmov_V},
	// unconditional branching
	{"jmp", "L", sl_Jmp},
	// conditional branching
	{"jeqss", "ssL", sl_Jeq_SS},
	{"jness", "ssL", sl_Jne_SS},
	{"jltff", "ffL", sl_Jlt_FF},
	{"jleff", "ffL", sl_Jle_FF},
	{"jeqff", "ffL", sl_Jeq_FF},
	{"jneff", "ffL", sl_Jne_FF},
	{"jgeff", "ffL", sl_Jge_FF},
	{"jgtff", "ffL", sl_Jgt_FF},
	// conditional branching on boolean value
	{"cjmp", "bL", sl_Cjmp},
	{"cjmpnot", "bL", sl_Cjmpnot},
	// simd flag state
	{"cond_reset", 0, sl_Cond_reset},
	{"cond_push", "b", sl_Cond_push},
	{"cond_pop", 0, sl_Cond_pop},
	{"cond_else", 0, sl_Cond_else},
	// boolean logic
	{"or", "bbb", sl_Or},
	{"and", "bbb", sl_And},
	{"not", "bb", sl_Not},
	// condition evaluation
	{"ifltff", "bff", sl_Iflt_FF},
	{"ifleff", "bff", sl_Ifle_FF},
	{"ifeqff", "bff", sl_Ifeq_FF},
	{"ifneff", "bff", sl_Ifne_FF},
	{"ifgeff", "bff", sl_Ifge_FF},
	{"ifgtff", "bff", sl_Ifgt_FF},
	{"ifeqss", "bss", sl_Ifeq_SS},
	{"ifness", "bss", sl_Ifne_SS},
	// condition eval. + bool.logic
	{"orltff", "bff", sl_Orlt_FF},
	{"orleff", "bff", sl_Orle_FF},
	{"oreqff", "bff", sl_Oreq_FF},
	{"orneff", "bff", sl_Orne_FF},
	{"orgeff", "bff", sl_Orge_FF},
	{"orgtff", "bff", sl_Orgt_FF},
	{"oreqss", "bss", sl_Oreq_SS},
	{"orness", "bss", sl_Orne_SS},
	{"andltff", "bff", sl_Andlt_FF},
	{"andleff", "bff", sl_Andle_FF},
	{"andeqff", "bff", sl_Andeq_FF},
	{"andneff", "bff", sl_Andne_FF},
	{"andgeff", "bff", sl_Andge_FF},
	{"andgtff", "bff", sl_Andgt_FF},
	{"andeqss", "bss", sl_Andeq_SS},
	{"andness", "bss", sl_Andne_SS},
	// shader functions
	{"length", "fv", sl_Length},
	{"setxcomp", "vf", sl_Setxcomp},
	{"setycomp", "vf", sl_Setycomp},
	{"setzcomp", "vf", sl_Setzcomp},
	{"xcomp", "fv", sl_Xcomp},
	{"ycomp", "fv", sl_Ycomp},
	{"zcomp", "fv", sl_Zcomp},
	{"compv", "fvf", sl_Compv},
	{"solar1", 0, sl_Solar1},
	{"solar2", "vf", sl_Solar2},
	{"end_solar", 0, sl_End_solar},
	{"illuminate1", "v", sl_Illuminate1},
	{"illuminate2", "vvf", sl_Illuminate1},
	{"end_illuminate", 0, sl_End_illuminate},
	{"illuminance1", "vL", sl_Illuminance1},
	{"illuminance2", "vvfL", sl_Illuminance2},
	{"shadow", "fsvu", sl_Shadow},
	{"texturev", "vs", sl_Texture_V},
	{"texturef", "fsff", sl_Texture_F},
	{"ambient", "v", sl_Ambient},
	{"diffuse", "vv", sl_Diffuse},
	{"specular", "vvvf", sl_Specular},
	{"specularbrdf", "vvvvf", sl_SpecularBrdf},
	{"normalize", "vv", sl_Normalize},
	{"depth", "fv", sl_Depth},
	{"distance", "fvv", sl_Distance},
	{"calcnormal", "vv", sl_Calcnormal},
	{"Duf", "ff", sl_Du_F},
	{"Dvf", "ff", sl_Dv_F},
	{"Duv", "vv", sl_Du_V},
	{"Dvv", "vv", sl_Dv_V},
	{"Derivf", "fff", sl_Deriv_F},
	{"Derivv", "vvf", sl_Deriv_V},
	{"area", "fv", sl_Area},
	{"faceforward1", "vvv", sl_Faceforward1},
	{"faceforward2", "vvvv", sl_Faceforward2},
	{"step", "fff", sl_Step},
	{"filterstep1", "fff", sl_Filterstep1},
	{"filterstep2", "ffff", sl_Filterstep2},
	{"smoothstep", "ffff", sl_Smoothstep},
	{"spline1f", "ffu", sl_Spline1_F},
	{"spline2f", "fsfu", sl_Spline2_F},
	{"spline1v", "vfu", sl_Spline1_V},
	{"spline2v", "vsfu", sl_Spline2_V},
	{"reflect", "vvv", sl_Reflect},
	{"refract", "vvvf", sl_Refract},
	// NOTE: fresnel functions return values are the first two or four args!
	{"fresnel1", "ffvvf", sl_Fresnel1},
	{"fresnel2", "ffvvvvf", sl_Fresnel2},
	{"mixf", "ffff", sl_Mixf},
	{"mixv", "vvvf", sl_Mixv},
	{"printf", "su", sl_Printf},
	{"noise1f", "ff", sl_Noise1_F},
	//{"noise2f", "fff", sl_Noise2_F},
	{"noise3f", "fv", sl_Noise3_F},
	//{"noise4f", "fvf", sl_Noise4_F},
	{"noise1v", "vf", sl_Noise1_V},
	//{"noise2v", "vff", sl_Noise2_V},
	{"noise3v", "vv", sl_Noise3_V},
	//{"noise4v", "vvf", sl_Noise4_V},
	{"pnoise1f", "fff", sl_Pnoise1_F},
	/*
	{"pnoise2f", "fffff", sl_Pnoise2_F},
	{"pnoise3f", "fvv", sl_Pnoise3_F},
	{"pnoise4f", "fvfvf", sl_Pnoise4_F},
	{"pnoise1v", "vff", sl_Pnoise1_V},
	{"pnoise2v", "vffff", sl_Pnoise2_V},
	{"pnoise3v", "vvv", sl_Pnoise3_V},
	{"pnoise4v", "vvfvf", sl_Pnoise4_V},
	*/
	{"cellnoise1f", "ff", sl_Cellnoise1_F},
	{"cellnoise2f", "fff", sl_Cellnoise2_F},
	//{"cellnoise3f", "fv", sl_Cellnoise3_F},
	//{"cellnoise4f", "fvf", sl_Cellnoise4_F},
	{"cellnoise1v", "vf", sl_Cellnoise1_V},
	{"cellnoise2v", "vff", sl_Cellnoise2_V},
	//{"cellnoise3v", "vv", sl_Cellnoise3_V},
	//{"cellnoise4v", "vvf", sl_Cellnoise4_V},
	{"fn_ridged", "fvfffff", sl_Fn_ridged},
	{"fn_hetero", "fvffff", sl_Fn_hetero},
	{"fn_fbm", "fvfff", sl_Fn_fbm},
	{"ptocurr", "psp", sl_Ptocurr},
	{"vtocurr", "vsv", sl_Vtocurr},
	{"ptransform1", "psp", sl_Ptransform1},
	{"ptransform2", "pssp", sl_Ptransform2},
	{"ptransform3", "pmp", sl_Ptransform3},
	/* TODO
	{"ptransform4", "psmp", sl_Ptransform4},
	*/
	{"vtransform1", "vsv", sl_Vtransform1},
	{"vtransform2", "vssv", sl_Vtransform2},
	/* TODO
	{"vtransform3", "vmv", sl_Vtransform3},
	{"vtransform4", "vsmv", sl_Vtransform4},
	{"ntransform1", "nsn", sl_Ntransform1},
	*/
	{"ntransform2", "nssn", sl_Ntransform2},
	/*
	{"ntransform3", "nmn", sl_Ntransform3},
	{"ntransform4", "nsmn", sl_Ntransform4},
	*/
	{"mtransform3", "msm", sl_Mtransform3},
	{"ctransform1", "csc", sl_Ctransform1},
	{"ctransform2", "cssc", sl_Ctransform2},
	{"randomf", "f", sl_RandomF},
	{"randomv", "v", sl_RandomV},
	{NULL, NULL, NULL}
};

__END_QDRENDER

