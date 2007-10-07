#ifndef DECPARAM_H
#define DECPARAM_H

#include "QDRender.h"
__BEGIN_QDRENDER

// ct_flags -> storage class & data type bits
// arlen -> data array length of variable (1 if no array)
// numfloats -> only used with primitive variables, total number of floats in data array (string type not used)
struct decParam_t { int ct_flags, arlen, numfloats; };

//----------------------------------------------------------------------------------------------
// update: added facevarying class, though can only guess as to its exact implementation...
enum dpStorageClass {SC_CONSTANT=1, SC_UNIFORM=2, SC_VARYING=4, SC_FACEVARYING=8, SC_VERTEX=16};
enum dpDataType {DT_FLOAT=32, DT_POINT=64, DT_VECTOR=128, DT_NORMAL=256, DT_COLOR=512,
               DT_STRING=1024, DT_MATRIX=2048, DT_HPOINT=4096, DT_INT=8192};
// SC_MASK -> storage class bits
// DT_MASK -> data type bits
// DT_FLOAT3MASK - > all types that are float[3] arrays, point/vector/normal/color
// DT_FLOATMASK -> all general float array based types, so DT_FLOAT3MASK + float/matrix/hpoint types
enum dpMaskBits {SC_MASK = (SC_CONSTANT | SC_UNIFORM | SC_VARYING | SC_FACEVARYING | SC_VERTEX),
DT_MASK = ~SC_MASK,
DT_FLOAT3MASK = (DT_POINT | DT_VECTOR | DT_NORMAL | DT_COLOR),
DT_FLOATMASK = (DT_FLOAT3MASK | DT_FLOAT | DT_MATRIX | DT_HPOINT) };

// helper function
// parses a declared parameter string, either inline or from RiDeclare()
// 'inline_name' if not NULL, indicates that 'declaration' is inline,
// and the extracted name will be returned in it.
bool parseDeclaration(const char* declaration, decParam_t* dp, char inline_name[256]);

__END_QDRENDER

#endif // DECPARAM_H
