//------------------------------------------------------------------------------
// The C++ rib... to Ri... interface, used by RIB_parser.cpp
//------------------------------------------------------------------------------

#include <iostream>
#include "ribInterface.h"

#include "State.h"
#include "Options.h"
#include "Attributes.h"
#include "Transform.h"

using namespace std;

__BEGIN_QDRENDER

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

//------------------------------------------------------------------------------
// Helper function to convert basic_string to a c type string
// (strdup() easier, but want to track all mem)
inline char* makeCString(const string& str)
{
	char* cs = new char[str.length() + 1];
	strcpy(cs, str.c_str());
	return cs;
}

//------------------------------------------------------------------------------
// parameter_t

parameter_t::parameter_t(const vector<string>& strvec)
{
	ct_flags = DT_STRING;
	string_array = strvec;
}

parameter_t::parameter_t(const vector<float>& fltvec)
{
	ct_flags = DT_FLOAT;
	float_array = fltvec;
}

parameter_t::parameter_t(const vector<int>& intvec)
{
	ct_flags = DT_INT;
	int_array = intvec;
}

string parameter_t::valueAsString() const
{
	string st = "";
	char tmp[1024];
	if (ct_flags & DT_FLOAT) {
		for (unsigned int i=0; i<float_array.size(); ++i) {
			snprintf(tmp, 1024, "%g", float_array[i]);
			st += tmp;
			if (i!=(float_array.size()-1)) st += ",";
		}
		if (float_array.size() > 1) return "[" + st + "]";
		return st;
	}
	else if (ct_flags & DT_STRING) {
		for (unsigned int i=0; i<string_array.size(); ++i) {
			st += "\"" + string_array[i] + "\"";
			if (i!=(string_array.size()-1)) st += ",";
		}
		if (string_array.size() > 1) return "[" + st + "]";
		return st;
	}
	else if (ct_flags & DT_INT) {
		for (unsigned int i=0; i<int_array.size(); ++i) {
			snprintf(tmp, 1024, "%d", int_array[i]);
			st += tmp;
			if (i!=(int_array.size()-1)) st += ",";
		}
		if (int_array.size() > 1) return "[" + st + "]";
		return st;
	}
	// for everything else, for now return empty string
	return "";
}

float parameter_t::getFloat() const
{
	if (ct_flags & DT_INT) return (float)int_array[0];
	if (ct_flags & DT_FLOAT) return float_array[0];
	// throw RIBparseException_t("Parameter not of requested type");
	return 0.f;
}

int parameter_t::getInt() const
{
	if (ct_flags & DT_INT) return int_array[0];
	if (ct_flags & DT_FLOAT) return (int)float_array[0];
	//throw RIBparseException_t("Parameter not of requested type");
	return 0;
}

string parameter_t::getString() const
{
	if (ct_flags & DT_STRING) return string_array[0];
	//throw RIBparseException_t("Parameter not of requested type");
	return "";
}

//------------------------------------------------------------------------------
// parameterList_t
// a list of parameters, for default functions, just the values,
// for any functions requiring tokens[]/parms[], list contains string followed by value

// search for parameter by name
const parameter_t* parameterList_t::getParameter(const string &name) const
{
	for (ParamIterator pi=params.begin(); pi!=params.end(); ++pi) {
		if (pi->ct_flags & DT_STRING) {
			if (pi->string_array[0]==name)
				if (pi!=(params.end()-1)) return &(*(pi+1)); else return NULL;
		}
	}
	return NULL;
}

// starting from parameter at index n
// (which corresponds to the start of the variable token/parms list),
// if type of param != declared type, convert if possible
void parameterList_t::convertTypes(int n)
{
	// after n, what follows should be token/value pairs,
	// so size must be even, throw error if total remaining length is odd
	if ((params.size() - n) & 1)
		throw RiException_t("[ERROR] -> convertTypes(): odd token/value pair parameterlist size");

	for (unsigned j=n; j<params.size(); j+=2) {
		// first of pair must be string
		if ((params[j].ct_flags & DT_STRING)==0)
			throw RiException_t("[ERROR] -> convertTypes(): unexpected type, not a token");

		// get parameter, predeclared or inline
		State* state = State::Instance();
		decParam_t pdv = {0, 0, 0};
		char inline_name[256] = {0};
		// skip if not found/parse error
		if (!state->parameterFromName(params[j].getString().c_str(), pdv, inline_name)) continue;

		// ok, but skip if flag undefined, it is just a name, not an actual variable
		if (pdv.ct_flags == 0) continue;

		int ct_flags = pdv.ct_flags, arlen = pdv.arlen;
		// if current param value type != declared type, try conversion to declared type.
		// (at this point only float/int or strings are the possible types,
		// Ri points/vectors/normals/colors/matrices/hpoints are also just float arrays)
		// throw error if array length of declared type < array length of current type
		// (can be more, but not less)
		parameter_t* pm = &params[j+1];
		int cur_type = pm->ct_flags & DT_MASK;
		int dec_type = ct_flags & DT_MASK;

		// adjust needed arraylen based on declared type
		if (dec_type & DT_FLOAT3MASK) // point/vector/normal/color
			arlen *= 3;
		else if (dec_type & DT_HPOINT)
			arlen <<= 2;
		else if (dec_type & DT_MATRIX)
			arlen <<= 4;
		// others don't change

		if ((cur_type == dec_type) or ((cur_type & DT_FLOATMASK) and (dec_type & DT_FLOAT3MASK))) {
			// no conversion needed, but still check that declared array length is valid ( <= current num.elems)
			bool arlen_ok = false;
			if (cur_type & DT_FLOATMASK)
				arlen_ok = (arlen <= (int)pm->float_array.size());
			else if (cur_type & DT_INT)
				arlen_ok = (arlen <= (int)pm->int_array.size());
			else if (cur_type & DT_STRING)
				arlen_ok = (arlen <= (int)pm->string_array.size());
			if (!arlen_ok)
				throw RiException_t("[ERROR] -> convertTypes(): not enough data elements for conversion");
		}
		else {
			// do conversion
			//  ^= (old_flag | new_flag) -> clears old, sets new
			if (cur_type & DT_FLOATMASK) {
				if (dec_type & DT_INT) {
					// float -> int
					if (arlen > (int)pm->float_array.size())
						throw RiException_t("[ERROR] -> convertTypes(): not enough data elements for conversion");
					pm->ct_flags ^= (DT_FLOAT | DT_INT);
					for (unsigned k=0; k<pm->float_array.size(); ++k)
						pm->int_array.push_back((RtInt)pm->float_array[k]);
					pm->float_array.clear();
				}
				else if (dec_type & DT_STRING) {
					// float -> string
					if (arlen > (int)pm->float_array.size())
						throw RiException_t("[ERROR] -> convertTypes(): not enough data elements for conversion");
					pm->ct_flags ^= (DT_FLOAT | DT_STRING);
					char tmp[32];
					for (unsigned k=0; k<pm->float_array.size(); ++k) {
						snprintf(tmp, 32, "%g", pm->float_array[k]);
						pm->string_array.push_back(tmp);
					}
					pm->float_array.clear();
				}
				// 'should' never happen, unless maybe decParam_t data corrupt...
				else if ((dec_type & DT_FLOATMASK)==0) {
					throw RiException_t("[ERROR] -> convertTypes(): unexpected declared data type");
				}
			}
			else if (cur_type & DT_INT) {
				if (dec_type & DT_FLOATMASK) {
					// int -> float
					if (arlen > (int)pm->int_array.size())
						throw RiException_t("[ERROR] -> convertTypes(): not enough data elements for conversion");
					pm->ct_flags ^= (DT_INT | DT_FLOAT);
					for (unsigned k=0; k<pm->int_array.size(); ++k)
						pm->float_array.push_back((RtFloat)pm->int_array[k]);
					pm->int_array.clear();
				}
				else if (dec_type & DT_STRING) {
					// int -> string
					if (arlen > (int)pm->int_array.size())
						throw RiException_t("[ERROR] -> convertTypes(): not enough data elements for conversion");
					pm->ct_flags ^= (DT_INT | DT_STRING);
					char tmp[32];
					for (unsigned k=0; k<pm->int_array.size(); ++k) {
						snprintf(tmp, 32, "%d", pm->int_array[k]);
						pm->string_array.push_back(tmp);
					}
					pm->int_array.clear();
				}
				else // 'should' never happen...
					throw RiException_t("[ERROR] -> convertTypes(): unexpected declared data type");
			}
			else if (cur_type & DT_STRING) {
				if (dec_type & DT_FLOATMASK) {
					// string -> float
					if (arlen > (int)pm->string_array.size())
						throw RiException_t("[ERROR] -> convertTypes(): not enough data elements for conversion");
					pm->ct_flags ^= (DT_STRING | DT_FLOAT);
					for (unsigned k=0; k<pm->string_array.size(); ++k)
						pm->float_array.push_back((float)atof(pm->string_array[k].c_str()));
					pm->string_array.clear();
				}
				else if (dec_type & DT_INT) {
					// string -> int
					if (arlen > (int)pm->string_array.size())
						throw RiException_t("[ERROR] -> convertTypes(): not enough data elements for conversion");
					pm->ct_flags ^= (DT_STRING | DT_INT);
					for (unsigned k=0; k<pm->string_array.size(); ++k)
						pm->int_array.push_back(atoi(pm->string_array[k].c_str()));
					pm->string_array.clear();
				}
				else // 'should' never happen...
					throw RiException_t("[ERROR] -> convertTypes(): unexpected declared data type");
			}
			else // should 'never' happen...
				throw RiException_t("[ERROR] -> convertTypes(): unexpected internal data type (current param)");
		}

	}
}


// Starting from parameter n (which indicates start of optional parameters in C++ list),
// convert the list to the Ri compatible C type format tokens/parms arrays.
// Will try conversion if declared type != actual type.
// Returns length of either array (both the same)
int parameterList_t::makeRiParamList(int n)
{
	// do any type conversion if needed
	convertTypes(n);
	// the number of token/value pairs in parameterlist
	int numpairs = (int)params.size() - n;
	// must be even
	if (numpairs & 1) {
		char tmp[256];
		snprintf(tmp, 256, "[ERROR] -> makeRiParamList(): odd length parameterlist (%d)", numpairs);
		throw RiException_t(tmp);
	}
	// set to actual returned array length
	int arlen = numpairs >> 1;
	// alloc tokens/parms lists
	freeArrays();
	tokens = new RtToken[arlen];
	parms = new RtPointer[arlen];
	array_sizes = new unsigned int[arlen];
	num_riparams = 0;
	// assign everything, both are just pointers to the actual values in C++ parameterlist
	for (int i=0; i<arlen; ++i) {
		// index into C++ parameterlist
		int idx = (i << 1) + n;
		// make sure token of pair in p.list actually is a string
		if ((params[idx].ct_flags & DT_STRING)==0)
			throw RiException_t("[ERROR] -> makeRiParamList(): token/value pair invalid, expected string");
		tokens[i] = makeCString(params[idx].getString().c_str());
		num_riparams++;
		// assign pointer to value
		int ct = params[idx+1].ct_flags & DT_MASK;
		if (ct & DT_FLOAT) {
			parms[i] = static_cast<RtPointer>(&params[idx+1].float_array[0]);
			array_sizes[i] = (unsigned int)params[idx+1].float_array.size();
		}
		else if (ct & DT_INT) {
			parms[i] = static_cast<RtPointer>(&params[idx+1].int_array[0]);
			array_sizes[i] = (unsigned int)params[idx+1].int_array.size();
		}
		else if (ct & DT_STRING) {
			const int sts = (int)params[idx+1].string_array.size();
			// for some reason in some cases got a 'Bus Error'/segfault when using a vector of vectors, another gcc bug, or me being stupid again?
			// in any case, is's now a regular array in a vector...
			RtString* car = new RtString[sts];
			cstrings.push_back(make_pair(sts, car));
			for (int j=0; j<sts; ++j)
				car[j] = makeCString(params[idx+1].string_array[j].c_str());
			parms[i] = car;
			array_sizes[i] = sts;
		}
		else // 'should' never happen...
			throw RiException_t("[ERROR] -> makeRiParamList(): unexpected data type");
	}

	return arlen;
}

//---------------------------------------------------------------------------------------
// checks expected array sizes (geom only), primvars of type 'constant' are always size 1
// update: added 'facevarying' class, can't find any docs
// on what the correct size should be, so its just a guess...
bool parameterList_t::checkPrimvarList(int numparam, int uniformMax, int varyingMax, int vertexMax, int faceVaryingMax)
{
	if (numparam == 0) return true; // no params
	State* state = State::Instance();
	char inline_name[256] = {0};
	for (int i=0; i<numparam; i++) {
		decParam_t dp = {0, 0, 0};
		if (!state->parameterFromName(tokens[i], dp, inline_name))
			return false; // possibly inline parse error
		// string variables are actually possible, but lets just ignore that here...
		if (dp.ct_flags & DT_STRING) {
			cout << "[ERROR]: primitive variables of type 'string' are not supported (yet)...\n";
			return false;
		}
		unsigned int lenmult = 1;
		if (dp.ct_flags & (DT_POINT | DT_VECTOR | DT_NORMAL | DT_COLOR))
			lenmult = 3;
		else if (dp.ct_flags & DT_HPOINT)
			lenmult = 4;
		else if (dp.ct_flags & DT_MATRIX)
			lenmult = 16;
		else if ((dp.ct_flags & DT_FLOAT)==0) {
			cout << "[ERROR]: Unexpected primitive variable data type of " << dp.ct_flags <<  " ( '" << tokens[i] << "' )\n";
			return false;
		}
		// if given array size is larger than the expected size, emit warning
		// (can simply ignore extraneaous part),
		// if it is less than the expected size, return error, primvar list not valid
		string vt = "constant";
		unsigned int arlen = dp.arlen * lenmult;
		if (dp.ct_flags & SC_UNIFORM)
			{ vt = "uniform";  arlen *= uniformMax; }
		else if (dp.ct_flags & SC_VARYING)
			{ vt = "varying";  arlen *= varyingMax; }
		else if (dp.ct_flags & SC_FACEVARYING)
			{ vt = "facevarying";  arlen *= faceVaryingMax; }
		else if (dp.ct_flags & SC_VERTEX)
			{ vt = "vertex";  arlen *= vertexMax; }
		if (array_sizes[i] != arlen) {
			cout << ((array_sizes[i] < arlen) ? "[ERROR]: " : "[WARNING]: ")
			     << "expected '" << vt << "' variable array '" << tokens[i] << "' of length " << arlen
			     << ", got " << array_sizes[i] << endl;
			if (array_sizes[i] < arlen) return false;
		}
	}
	return true;
}

//---------------------------------------------------------------------------------------
// The rib interface

#define exc_numargs RiException_t("[WARNING]: incorrect number of required arguments")
void ribDeclare(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 2) throw exc_numargs;
	RiDeclare(plist.params[0].getString().c_str(), plist.params[1].getString().c_str());
}

//-----------
// begin/end
//-----------
void ribFrameBegin(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiFrameBegin(plist.params[0].getInt());
}

void ribFrameEnd(unsigned int numargs, parameterList_t& plist)
{
	RiFrameEnd();
}

void ribWorldBegin(unsigned int numargs, parameterList_t& plist)
{
	RiWorldBegin();
}
void ribWorldEnd(unsigned int numargs, parameterList_t& plist)
{
	RiWorldEnd();
}
void ribAttributeBegin(unsigned int numargs, parameterList_t& plist)
{
	RiAttributeBegin();
}
void ribAttributeEnd(unsigned int numargs, parameterList_t& plist)
{
	RiAttributeEnd();
}
void ribTransformBegin(unsigned int numargs, parameterList_t& plist)
{
	RiTransformBegin();
}
void ribTransformEnd(unsigned int numargs, parameterList_t& plist)
{
	RiTransformEnd();
}
void ribSolidBegin(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiSolidBegin(plist.params[0].getString().c_str());
}
void ribSolidEnd(unsigned int numargs, parameterList_t& plist)
{
	RiSolidEnd();
}
void ribMotionBegin(unsigned int numargs, parameterList_t& plist)
{
	const int n = (int)plist.params[0].float_array.size();
	float* tma = new float[n];
	for (int i=0; i<n; ++i)
		tma[i] = plist.params[i].float_array[i];
	RiMotionBeginV(n, tma);
	delete[] tma;
}
void ribMotionEnd(unsigned int numargs, parameterList_t& plist)
{
	RiMotionEnd();
}
void ribObjectBegin(unsigned int numargs, parameterList_t& plist)
{
	RiObjectBegin();
}
void ribObjectEnd(unsigned int numargs, parameterList_t& plist)
{
	RiObjectEnd();
}
//-----------
// Transforms
//-----------
void ribIdentity(unsigned int numargs, parameterList_t& plist)
{
	RiIdentity();
}
void ribTransform(unsigned int numargs, parameterList_t& plist)
{
	RtMatrix m;
	const vector<float>& fa = plist.params[0].float_array;
	if (fa.size() != 16) throw exc_numargs;
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			m[i][j] = fa[i + 4*j];
	RiTransform(m);
}
void ribConcatTransform(unsigned int numargs, parameterList_t& plist)
{
	const vector<float>& fa = plist.params[0].float_array;
	if (fa.size() != 16) throw exc_numargs;
	RtMatrix m;
	for (int i=0; i<4; ++i)
		for (int j=0; j<4; ++j)
			m[i][j] = fa[i + 4*j];
	RiConcatTransform(m);
}
void ribPerspective(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiPerspective(plist.params[0].getFloat()); // fov
}
void ribTranslate(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 3) throw exc_numargs;
	// dx, dy, dz
	RiTranslate(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat());
}
void ribRotate(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 4) throw exc_numargs;
	// angle, dx, dy, dz
	RiRotate(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(), plist.params[3].getFloat());
}
void ribScale(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 3) throw exc_numargs;
	// sx, sy, sz
	RiScale(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat());
}
void ribSkew(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 7) throw exc_numargs;
	// angle, dx1, dy1, dz1, dx2, dy2, dz2
	RiSkew(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(), plist.params[3].getFloat(),
				plist.params[4].getFloat(), plist.params[5].getFloat(), plist.params[6].getFloat());
}
void ribCoordinateSystem(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiCoordinateSystem(plist.params[0].getString().c_str());
}
void ribCoordSysTransform(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiCoordSysTransform(plist.params[0].getString().c_str());
}
//-----------
// Options
//-----------
// camera
void ribFormat(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 3) throw exc_numargs;
	RiFormat(plist.params[0].getInt(), plist.params[1].getInt(), plist.params[2].getFloat());
}
void ribFrameAspectRatio(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiFrameAspectRatio(plist.params[0].getFloat());
}
void ribScreenWindow(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 4) throw exc_numargs;
	RiScreenWindow(plist.params[0].getFloat(), plist.params[1].getFloat(),
								plist.params[2].getFloat(), plist.params[3].getFloat());
}
void ribCropWindow(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 4) throw exc_numargs;
	RiCropWindow(plist.params[0].getFloat(), plist.params[1].getFloat(),
							plist.params[2].getFloat(), plist.params[3].getFloat());
}
// !!! Remember, when preparing for Ri...V() function call, use < for numargs test,
// not !=,  remaining params after the required args are optional
void ribProjection(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiProjectionV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
void ribClipping(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 2) throw exc_numargs;
	RiClipping(plist.params[0].getFloat(), plist.params[1].getFloat());
}
void ribClippingPlane(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 6) throw exc_numargs;
	RiClippingPlane(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(),
									plist.params[3].getFloat(), plist.params[4].getFloat(), plist.params[5].getFloat());
}
void ribDepthOfField(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 3) throw exc_numargs;
	RiDepthOfField(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat());
}
void ribShutter(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 2) throw exc_numargs;
	RiShutter(plist.params[0].getFloat(), plist.params[1].getFloat());
}
// display
void ribPixelVariance(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiPixelVariance(plist.params[0].getFloat());
}
void ribPixelSamples(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 2) throw exc_numargs;
	RiPixelSamples(plist.params[0].getFloat(), plist.params[1].getFloat());
}
void ribPixelFilter(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 3) throw exc_numargs;
	RtFilterFunc ft;
	string fname = plist.params[0].getString();
	if (fname == "box")
		ft = RiBoxFilter;
	else if (fname == "triangle")
		ft = RiTriangleFilter;
	else if (fname == "gaussian")
		ft = RiGaussianFilter;
	else if (fname == "catmull-rom")
		ft = RiCatmullRomFilter;
	else if (fname == "sinc")
		ft = RiSincFilter;
	else {
		// unknown, use gaussian default
		ft = RiGaussianFilter;
		cout << "[WARNING]: unknown filter type '" << fname << "'\n";
	}
	RiPixelFilter(ft, plist.params[1].getFloat(), plist.params[2].getFloat());
}
void ribExposure(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 2) throw exc_numargs;
	RiExposure(plist.params[0].getFloat(), plist.params[1].getFloat());
}
void ribImager(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiImagerV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
void ribQuantize(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 5) throw exc_numargs;
	RiQuantize(plist.params[0].getString().c_str(),
	           plist.params[1].getInt(), plist.params[2].getInt(), plist.params[3].getInt(),
	           plist.params[4].getFloat());
}
void ribDisplay(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 3) throw exc_numargs;
	const int n = plist.makeRiParamList(3);
	RiDisplayV(plist.params[0].getString().c_str(), plist.params[1].getString().c_str(),
	           plist.params[2].getString().c_str(), n, plist.tokens, plist.parms);
}
// additional
void ribHider(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiHiderV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
void ribColorSamples(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 3) throw exc_numargs;
	// not used, not implemented
	RiColorSamples(0, NULL, NULL);
}
void ribRelativeDetail(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiRelativeDetail(plist.params[0].getFloat());
}
// optional
void ribOption(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiOptionV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
//-----------
// Attributes
//-----------
// shading
void ribColor(unsigned int numargs, parameterList_t& plist)
{
	// can be either 3-float array, or 3 single float values,
	// so have to do extra test here
	RtColor c;
	if (numargs == 1) {
		if (plist.params[0].float_array.size() == 3) {
			c[0] = plist.params[0].float_array[0];
			c[1] = plist.params[0].float_array[1];
			c[2] = plist.params[0].float_array[2];
		}
		else throw exc_numargs;
	}
	else if (numargs == 3) {
		c[0] = plist.params[0].getFloat();
		c[1] = plist.params[1].getFloat();
		c[2] = plist.params[2].getFloat();
	}
	else throw exc_numargs;
	RiColor(c);
}
void ribOpacity(unsigned int numargs, parameterList_t& plist)
{
	// can be either 3-float array, or 3 single float values,
	// so have to do extra test here
	RtColor c;
	if (numargs == 1) {
		if (plist.params[0].float_array.size() == 3) {
			c[0] = plist.params[0].float_array[0];
			c[1] = plist.params[0].float_array[1];
			c[2] = plist.params[0].float_array[2];
		}
		else throw exc_numargs;
	}
	else if (numargs == 3) {
		c[0] = plist.params[0].getFloat();
		c[1] = plist.params[1].getFloat();
		c[2] = plist.params[2].getFloat();
	}
	else throw exc_numargs;
	RiOpacity(c);
}
void ribTextureCoordinates(unsigned int numargs, parameterList_t& plist)
{
	// can be array as well
	if (numargs != 8) {
		if (plist.params[0].float_array.size() == 8) {
			vector<float>& fa = plist.params[0].float_array;
			RiTextureCoordinates(fa[0], fa[1], fa[2], fa[3], fa[4], fa[5], fa[6], fa[7]);
		}
		else throw exc_numargs;
	}
	else {
		RiTextureCoordinates(plist.params[0].getFloat(), plist.params[1].getFloat(),
		                     plist.params[2].getFloat(), plist.params[3].getFloat(),
		                     plist.params[4].getFloat(), plist.params[5].getFloat(),
		                     plist.params[6].getFloat(), plist.params[7].getFloat());
	}
}
// lights
void ribLightSource(unsigned int numargs, parameterList_t& plist)
{
	// RIB has extra ID parameter following name, so skip first 2 params instead of 1
	if (numargs < 2) throw exc_numargs;
	const int n = plist.makeRiParamList(2);
	RtLightHandle LH = RiLightSourceV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
	// save ID for Illuminate statements
	State::Instance()->setLightID(LH, plist.params[1].getInt());
}
void ribAreaLightSource(unsigned int numargs, parameterList_t& plist)
{
	/*
	// RIB has extra ID parameter following name, so skip 2 params instead of 1
	if (numargs < 2) throw exc_numargs;
	const int n = plist.makeRiParamList(2);
	RiAreaLightSourceV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
	*/
	// arealights not supported yet, for now treat as regular lightsource
	ribLightSource(numargs, plist);
}
void ribIlluminate(unsigned int numargs, parameterList_t& plist)
{
	// convert integer to pointer
	RtLightHandle LH = State::Instance()->getLightHandle(plist.params[0].getInt());
	if (LH != NULL) RiIlluminate(LH, plist.params[1].getInt());
}
// shaders
void ribSurface(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiSurfaceV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
void ribDisplacement(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiDisplacementV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
void ribAtmosphere(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiAtmosphereV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
void ribInterior(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiInteriorV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
void ribExterior(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiExteriorV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
// shading
void ribShadingRate(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiShadingRate(plist.params[0].getFloat());
}
void ribShadingInterpolation(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiShadingInterpolation(plist.params[0].getString().c_str());
}
void ribMatte(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiMatte((RtBoolean)plist.params[0].getInt());
}
// geometry
void ribBound(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 6) throw exc_numargs;
	RtBound b = {plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(),
							plist.params[3].getFloat(), plist.params[4].getFloat(), plist.params[5].getFloat()};
	RiBound(b);
}
void ribDetail(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 6) throw exc_numargs;
	RtBound b = {plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(),
							plist.params[3].getFloat(), plist.params[4].getFloat(), plist.params[5].getFloat()};
	RiDetail(b);
}
void ribDetailRange(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 4) throw exc_numargs;
	RiDetailRange(plist.params[0].getFloat(), plist.params[1].getFloat(),
								plist.params[2].getFloat(), plist.params[3].getFloat());
}
void ribGeometricApproximation(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 2) throw exc_numargs;
	RiGeometricApproximation(plist.params[0].getString().c_str(), plist.params[1].getFloat());
}
void ribOrientation(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiOrientation(plist.params[0].getString().c_str());
}
void ribReverseOrientation(unsigned int numargs, parameterList_t& plist)
{
	// no arguments
	RiReverseOrientation();
}
void ribSides(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 1) throw exc_numargs;
	RiSides(plist.params[0].getInt());
}
// optional
void ribAttribute(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	RiAttributeV(plist.params[0].getString().c_str(), n, plist.tokens, plist.parms);
}
//------------------------------------------------------------------------------------------------------------------------------
// Geometric Primitives, all checkPrimVarList() facevarying counts are guesses, can't find any info on this at all anywhere...
//------------------------------------------------------------------------------------------------------------------------------
void ribPolygon(unsigned int numargs, parameterList_t& plist)
{
	// no default arguments
	const int n = plist.makeRiParamList(0);
	RtInt nverts = 0;
	for (int i=0; i<n; i++) {
		if (!strcmp(plist.tokens[i], RI_P)) {
			nverts = plist.array_sizes[i];
			break;
		}
	}
	nverts /= 3; // assuming "P"
	if (plist.checkPrimvarList(n, 1, nverts, nverts, nverts))
		RiPolygonV(nverts, n, plist.tokens, plist.parms);
}
// GeneralPolygon TODO
void ribPointsPolygons(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 2) throw exc_numargs;
	const int n = plist.makeRiParamList(2);
	// npolys & nverts array
	RtInt npolys = (RtInt)plist.params[0].float_array.size();
	RtInt* nverts = new RtInt[npolys];
	unsigned int vsize = 0; // number of vertex indices
	int max_fvary = 0;	// max number of facevarying vars
	for (int i=0; i<npolys; ++i) {
		nverts[i] = (int)plist.params[0].float_array[i];
		max_fvary += nverts[i];
		vsize += (unsigned int)nverts[i];
	}
	// verts array
	if (vsize != plist.params[1].float_array.size()) {
		delete[] nverts;
		throw RiException_t("[ERROR]: ribPointsPolygons() -> index array size does not match face vertex sum");
	}
	RtInt* verts = new RtInt[vsize];
	int max_vidx = 0; // highest index + 1 -> number of expected vertices
	for (unsigned int i=0; i<vsize; ++i) {
		verts[i] = (int)plist.params[1].float_array[i];
		max_vidx = (verts[i] > max_vidx) ? verts[i] : max_vidx;
	}
	max_vidx++;
	if (plist.checkPrimvarList(n, npolys, max_vidx, max_vidx, max_fvary))
		RiPointsPolygonsV(npolys, nverts, verts, n, plist.tokens, plist.parms);
	delete[] verts;
	delete[] nverts;
}
// PointsGeneralPolygons TODO
void ribBasis(unsigned int numargs, parameterList_t& plist)
{
	if (numargs != 4) throw exc_numargs;
	// first and second argurments can be either name of predef. basis or a matrix
	RtBasis bs[2];
	for (int b=0; b<2; ++b) {
		const int b2 = b << 1;
		if (plist.params[b2].ct_flags & DT_STRING) {
			string bname = plist.params[b2].getString();
			if (bname=="bezier")
				memcpy(bs[b], RiBezierBasis, sizeof(RtBasis));
			else if (bname=="b-spline")
				memcpy(bs[b], RiBSplineBasis, sizeof(RtBasis));
			else if (bname=="catmull-rom")
				memcpy(bs[b], RiCatmullRomBasis, sizeof(RtBasis));
			else if (bname=="hermite")
				memcpy(bs[b], RiHermiteBasis, sizeof(RtBasis));
			else if (bname=="power")
				memcpy(bs[b], RiPowerBasis, sizeof(RtBasis));
			else { // unknown, use bezier as default
				cout << "[WARNING]: unknown basis '" << bname << "'\n";
				memcpy(bs[b], RiBezierBasis, sizeof(RtBasis));
			}
		}
		else {
			// assume matrix, float array
			if (plist.params[b2].float_array.size() != 16)
				throw exc_numargs;
			for (int j=0; j<4; j++)
				for (int i=0; i<4; i++)
					bs[b][j][i] = plist.params[b2].float_array[i + j*4];
		}
	}
	RiBasis(bs[0], plist.params[1].getInt(), bs[1], plist.params[3].getInt());
}
void ribPatch(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	const string ptype = plist.params[0].getString();
	if (plist.checkPrimvarList(n, 1, 4, ((ptype == "bilinear") ? 4 : 16), 4))
		RiPatchV(ptype.c_str(), n, plist.tokens, plist.parms);
}
void ribPatchMesh(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 5) throw exc_numargs;
	const int n = plist.makeRiParamList(5);
	const int nu = plist.params[1].getInt(), nv = plist.params[3].getInt();
	const string ptype = plist.params[0].getString(),
							uwrap = plist.params[2].getString(),
							vwrap = plist.params[4].getString();
	const bool linp = (ptype == "bilinear");
	const bool uper = (uwrap == "periodic");
	const bool vper = (vwrap == "periodic");
	const Attributes& attr = State::Instance()->topAttributes();
	const int ustep = attr.cubicBasisStep[0], vstep = attr.cubicBasisStep[1];
	const int nupat = (linp ? (uper ? nu : (nu - 1))
													: (uper ? (nu / ustep) : (((nu - 4) / ustep) + 1)));
	const int nvpat = (linp ? (vper ? nv : (nv - 1))
													: (vper ? (nv / vstep) : (((nv - 4) / vstep) + 1)));
	const int nuvar = (uper ? nupat : (nupat + 1));
	const int nvvar = (vper ? nvpat : (nvpat + 1));
	if (plist.checkPrimvarList(n, nupat*nvpat, nuvar*nvvar, nu*nv, nupat*nvpat*4))
		RiPatchMeshV(ptype.c_str(), nu, uwrap.c_str(), nv, vwrap.c_str(), n, plist.tokens, plist.parms);
}
void ribNuPatch(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 10) throw exc_numargs;
	const int n = plist.makeRiParamList(10);
	const unsigned int nu = plist.params[0].getInt(), uorder = plist.params[1].getInt();
	const unsigned int nv = plist.params[5].getInt(), vorder = plist.params[6].getInt();
	const unsigned int num_uknot = nu + uorder, num_vknot = nv + vorder;
	if (num_uknot != plist.params[2].float_array.size()) {
		cout << "[ERROR] ribNuPatch() -> expected " << num_uknot
		     << " uknot values, got " << plist.params[2].float_array.size() << endl;
		return;
	}
	if (num_vknot != plist.params[7].float_array.size()) {
		cout << "[ERROR] ribNuPatch() -> expected " << num_vknot
		     << " vknot values, got " << plist.params[7].float_array.size() << endl;
		return;
	}
	RtFloat *uknot = new RtFloat[num_uknot], *vknot = new RtFloat[num_vknot];
	for (unsigned int i=0; i<num_uknot; ++i)
		uknot[i] = plist.params[2].float_array[i];
	for (unsigned int i=0; i<num_vknot; ++i)
		vknot[i] = plist.params[7].float_array[i];
	const int nuseg =  nu - uorder + 1, nvseg = nv - vorder + 1;
	if (plist.checkPrimvarList(n, nuseg*nvseg, (nuseg+1)*(nvseg+1), nu*nv, nuseg*nvseg*4))
		RiNuPatchV(nu, uorder, uknot, plist.params[3].getFloat(), plist.params[4].getFloat(),
		           nv, vorder, vknot, plist.params[8].getFloat(), plist.params[9].getFloat(),
		           n, plist.tokens, plist.parms);
	delete[] vknot;
	delete[] uknot;
}
void ribTrimCurve(unsigned int numargs, parameterList_t& plist)
{
	// TODO
}
void ribSubdivisionMesh(unsigned int numargs, parameterList_t& plist)
{
	// at least 3 args required, scheme, nvertices, and vertices
	if (numargs < 3) throw exc_numargs;
	RtInt ntags = 0, numint = 0, numflt = 0;
	RtToken* tags = NULL;
	RtInt* nargs = NULL;
	RtInt* intargs = NULL;
	RtFloat* floatargs = NULL;
	int optvar_start = 3; // if no tags, makeRiParamList should start at var 3
	// tags
	// either more subdiv params follow, or the token/value list starts...
	// In any case, if more subd params follow, the next var should be the 'tags' string array
	// this also means that makeRiParamList() should start after these params
	if (numargs > 3) {
		if (plist.params[3].getString() != "P") {
			ntags = (int)plist.params[3].string_array.size();
			tags = new RtToken[ntags];
			for (int i=0; i<ntags; i++) {
				tags[i] = makeCString(plist.params[3].string_array[i].c_str());
				numint += (int)plist.params[4].float_array[i*2];
				numflt += (int)plist.params[4].float_array[i*2 + 1];
			}
			if (numint) {
				// intargs
				intargs = new RtInt[numint];
				for (int i=0; i<numint; i++)
					intargs[i] = (int)plist.params[5].float_array[i];
			}
			if (numflt) {
				// floatargs
				floatargs = new RtFloat[numint];
				for (int i=0; i<numflt; i++)
					floatargs[i] = plist.params[6].float_array[i];
			}
			// makeRiParamList starts at param 7 if complete arrays given,
			// but since parser skips empty arrays, start could be earlier
			optvar_start = 7 - (numint==0 ? 1 : 0) - (numflt==0 ? 1 : 0);
		}
	}
	const int n = plist.makeRiParamList(optvar_start);
	// nvertices array (list of num. of vertices per face)
	RtInt num_nverts = (RtInt)plist.params[1].float_array.size();
	RtInt* nverts = new RtInt[num_nverts];
	int sum_facevert = 0;	// also used for facevarying count
	for (int i=0; i<num_nverts; i++) {
		nverts[i] = (int)plist.params[1].float_array[i];
		sum_facevert += nverts[i];
	}
	// vertices array (list of vertex indices), size must be equal to sum_facevert
	RtInt num_verts = (RtInt)plist.params[2].float_array.size();
	if (num_verts != sum_facevert) {
		delete[] nverts;
		cout << "[ERROR] ribSubdivisionMesh() -> expected " << sum_facevert
		     << " vertex indices, got " << num_verts << endl;
		return;
	}
	RtInt* verts = new RtInt[num_verts];
	int max_vert = 0; // highest vertex index + 1 -> number of vertices
	for (int i=0; i<num_verts; i++) {
		verts[i] = (int)plist.params[2].float_array[i];
		max_vert = (verts[i] > max_vert) ? verts[i] : max_vert;
	}
	max_vert++;
	if (plist.checkPrimvarList(n, num_nverts, max_vert, max_vert, sum_facevert)) {
		RiSubdivisionMeshV(plist.params[0].getString().c_str(), num_nverts, nverts, verts,
		                   ntags, tags, nargs, intargs, floatargs, n, plist.tokens, plist.parms);
	}
	delete[] nverts;
	delete[] verts;
	if (tags) { // c string copies
		for (int i=0; i<ntags; ++i)
			delete[] tags[i];
		delete[] tags;
	}
	if (intargs) delete[] intargs;
	if (floatargs) delete[] floatargs;
}
// NOTE: SPHERE/CONE/CYLINDER/ETC DEFAULT ARGS CAN ALSO BE ARRAY !!! TODO !!!
void ribSphere(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 4) throw exc_numargs;
	const int n = plist.makeRiParamList(4);
	if (plist.checkPrimvarList(n, 1, 4, 4, 4))
		RiSphereV(plist.params[0].getFloat(), plist.params[1].getFloat(),
		          plist.params[2].getFloat(), plist.params[3].getFloat(),
		          n, plist.tokens, plist.parms);
}
void ribCone(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 3) throw exc_numargs;
	const int n = plist.makeRiParamList(3);
	if (plist.checkPrimvarList(n, 1, 4, 4, 4))
		RiConeV(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(),
		        n, plist.tokens, plist.parms);
}
void ribCylinder(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 4) throw exc_numargs;
	const int n = plist.makeRiParamList(4);
	if (plist.checkPrimvarList(n, 1, 4, 4, 4))
		RiCylinderV(plist.params[0].getFloat(), plist.params[1].getFloat(),
		            plist.params[2].getFloat(), plist.params[3].getFloat(),
		            n, plist.tokens, plist.parms);
}
void ribHyperboloid(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 7) throw exc_numargs;
	const int n = plist.makeRiParamList(7);
	RtPoint p1 = {plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat()};
	RtPoint p2 = {plist.params[3].getFloat(), plist.params[4].getFloat(), plist.params[5].getFloat()};
	if (plist.checkPrimvarList(n, 1, 4, 4, 4))
		RiHyperboloidV(p1, p2, plist.params[6].getFloat(), n, plist.tokens, plist.parms);
}
void ribParaboloid(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 4) throw exc_numargs;
	const int n = plist.makeRiParamList(4);
	if (plist.checkPrimvarList(n, 1, 4, 4, 4))
		RiParaboloidV(plist.params[0].getFloat(), plist.params[1].getFloat(),
		              plist.params[2].getFloat(), plist.params[3].getFloat(),
		              n, plist.tokens, plist.parms);
}
void ribDisk(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 3) throw exc_numargs;
	const int n = plist.makeRiParamList(3);
	if (plist.checkPrimvarList(n, 1, 4, 4, 4))
		RiDiskV(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(),
		        n, plist.tokens, plist.parms);
}
void ribTorus(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 5) throw exc_numargs;
	const int n = plist.makeRiParamList(5);
	if (plist.checkPrimvarList(n, 1, 4, 4, 4))
		RiTorusV(plist.params[0].getFloat(), plist.params[1].getFloat(), plist.params[2].getFloat(),
		         plist.params[3].getFloat(), plist.params[4].getFloat(),
		         n, plist.tokens, plist.parms);
}
void ribPoints(unsigned int numargs, parameterList_t& plist)
{
	// no arguments
	const int n = plist.makeRiParamList(0);
	int nverts = 0;
	for (int i=0; i<n; i++) {
		if (!strcmp(plist.tokens[i], RI_P)) {
			nverts = plist.array_sizes[i];
			break;
		}
	}
	nverts /= 3;
	if (plist.checkPrimvarList(n, 1, nverts, nverts, nverts))
		RiPointsV(nverts, n, plist.tokens, plist.parms);
}
void ribCurves(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 3) throw exc_numargs;
	const int n = plist.makeRiParamList(3);
	bool linear;
	if (plist.params[0].getString() == "linear")
		linear = true;
	else if (plist.params[0].getString() == "cubic")
		linear = false;
	else {
		cout << "[ERROR] ribCurves() -> unknown type \"" << plist.params[0].getString() << "\"\n";
		return;
	}
	bool periodic;
	if (plist.params[2].getString() == "periodic")
		periodic = true;
	else if (plist.params[2].getString() == "nonperiodic")
		periodic = false;
	else {
		cout << "[ERROR] ribCurves() -> unknown wrap type \"" << plist.params[2].getString() << "\"\n";
		return;
	}
	const int ncurves = (int)plist.params[1].float_array.size();
	RtInt* nvertices = new RtInt[ncurves];
	int tot_nvary = 0, tot_verts = 0;
	const int vstep = State::Instance()->topAttributes().cubicBasisStep[1];
	for (int i=0; i<ncurves; ++i) {
		nvertices[i] = (int)plist.params[1].float_array[i];
		if (!linear && nvertices[i] < 4) {
			// could ignore this and force wrap around, aqsis & pixie seem to do this
			// but it would seem to make sense to consider it an error...
			cout << "[ERROR] ribCurves() -> not enough vertices for cubic curve " << i
			     << "\n(has " << nvertices[i] << ", need at least 4)\n";
			delete[] nvertices;
			return;
		}
		tot_verts += nvertices[i];
		const int nseg = linear ? (periodic ? nvertices[i] : (nvertices[i] - 1)) :
															(periodic ? (nvertices[i]/vstep) : ((nvertices[i] - 4)/vstep + 1));
		tot_nvary += (periodic ? nseg : (nseg + 1));
	}
	if (plist.checkPrimvarList(n, ncurves, tot_nvary, tot_verts, tot_nvary*2))	// facevarying guess, no clue...
		RiCurvesV(plist.params[0].getString().c_str(), ncurves, nvertices, plist.params[2].getString().c_str(),
		          n, plist.tokens, plist.parms);
	delete[] nvertices;
}
// 'include' file
void ribReadArchive(unsigned int numargs, parameterList_t& plist)
{
	if (numargs < 1) throw exc_numargs;
	const int n = plist.makeRiParamList(1);
	// no callback yet
	RiReadArchiveV(plist.params[0].getString().c_str(), NULL, n, plist.tokens, plist.parms);
}

_ribFunc ribFuncTable[] = {
	{"Declare", ribDeclare},
	{"FrameBegin", ribFrameBegin},
	{"FrameEnd", ribFrameEnd},
	{"WorldBegin", ribWorldBegin},
	{"WorldEnd", ribWorldEnd},
	{"AttributeBegin", ribAttributeBegin},
	{"AttributeEnd", ribAttributeEnd},
	{"TransformBegin", ribTransformBegin},
	{"TransformEnd", ribTransformEnd},
	{"SolidBegin", ribSolidBegin},
	{"SolidEnd", ribSolidEnd},
	{"MotionBegin", ribMotionBegin},
	{"MotionEnd", ribMotionEnd},
	{"ObjectBegin", ribObjectBegin},
	{"ObjectEnd", ribObjectEnd},
	{"Identity", ribIdentity},
	{"Transform", ribTransform},
	{"ConcatTransform", ribConcatTransform},
	{"Perspective", ribPerspective},
	{"Translate", ribTranslate},
	{"Rotate", ribRotate},
	{"Scale", ribScale},
	{"Skew", ribSkew},
	{"CoordinateSystem", ribCoordinateSystem},
	{"CoordSysTransform", ribCoordSysTransform},
	{"Format", ribFormat},
	{"FrameAspectRatio", ribFrameAspectRatio},
	{"ScreenWindow", ribScreenWindow},
	{"CropWindow", ribCropWindow},
	{"Projection", ribProjection},
	{"Clipping", ribClipping},
	{"ClippingPlane", ribClippingPlane},
	{"DepthOfField", ribDepthOfField},
	{"Shutter", ribShutter},
	{"PixelVariance", ribPixelVariance},
	{"PixelSamples", ribPixelSamples},
	{"PixelFilter", ribPixelFilter},
	{"Exposure", ribExposure},
	{"Imager", ribImager},
	{"Quantize", ribQuantize},
	{"Display", ribDisplay},
	{"Hider", ribHider},
	{"ColorSamples", ribColorSamples},
	{"RelativeDetail", ribRelativeDetail},
	{"Option", ribOption},
	{"Color", ribColor},
	{"Opacity", ribOpacity},
	{"TextureCoordinates", ribTextureCoordinates},
	{"LightSource", ribLightSource},
	{"AreaLightSource", ribAreaLightSource},
	{"Illuminate", ribIlluminate},
	{"Surface", ribSurface},
	{"Displacement", ribDisplacement},
	{"Atmosphere", ribAtmosphere},
	{"Interior", ribInterior},
	{"Exterior", ribExterior},
	{"ShadingRate", ribShadingRate},
	{"ShadingInterpolation", ribShadingInterpolation},
	{"Matte", ribMatte},
	{"Bound", ribBound},
	{"Detail", ribDetail},
	{"DetailRange", ribDetailRange},
	{"GeometricApproximation", ribGeometricApproximation},
	{"Orientation", ribOrientation},
	{"ReverseOrientation", ribReverseOrientation},
	{"Sides", ribSides},
	{"Attribute", ribAttribute},
	{"Polygon", ribPolygon},
	//{"GeneralPolygon", ribGeneralPolygon},
	{"PointsPolygons", ribPointsPolygons},
	//{"PointsGeneralPolygons", ribPointsGeneralPolygons},
	{"Basis", ribBasis},
	{"Patch", ribPatch},
	{"PatchMesh", ribPatchMesh},
	{"NuPatch", ribNuPatch},
	{"TrimCurve", ribTrimCurve},
	{"SubdivisionMesh", ribSubdivisionMesh},
	{"Sphere", ribSphere},
	{"Cone", ribCone},
	{"Cylinder", ribCylinder},
	{"Hyperboloid", ribHyperboloid},
	{"Paraboloid", ribParaboloid},
	{"Disk", ribDisk},
	{"Torus", ribTorus},
	{"Points", ribPoints},
	{"Curves", ribCurves},
	{"ReadArchive", ribReadArchive},
	{NULL, NULL}
};

// main function
// processes the parameterList and calls the corresponding Ri_ functions
// return string is only meant for debug output
string parameterList_t::process(int state_hint, const char* basepath)
{
	State* state = State::Instance();

	// used for debug output
	string out = "";

	// handle state_hint first
	if (state_hint!=0) {
		if (state_hint==-1) {
			state->begin(NULL);
			// set the basepath in options directly
			if (basepath)
				strncpy(state->topOptions().basepath, basepath, 256);
		}
		else if (state_hint==1)
			state->end();
		else
			out += "unexpected state hint, not -1,0 or 1";    // should never happen
		return out;
	}

	if (listname=="NONE")
		return "PARAMLIST UNFINISHED ( \"" + listname + "\" )";

	// total number of arguments in parameterlist
	unsigned int numargs = (unsigned int)params.size();

	// catch errors
	try
	{
		RIBFUNC* ribfunc = state->_riblu.find(listname.c_str());
		if (ribfunc)
			(*ribfunc)(numargs, *this);
		else // for anything not handled yet
			cout << "TODO! -> " << listname << endl;
	}
	catch (RiException_t &ri_exc)
	{
		// exception already prints message
		out += " !!! " + string(ri_exc.what()) + " !!! ";
	}

	out += "PARAMLIST " + listname + ": ";
	for (ParamIterator pi=params.begin(); pi!=params.end(); ++pi)
		out += pi->valueAsString() + ", ";
	out += " RI_NULL";

	return out;
}

#undef exc_numargs

__END_QDRENDER
