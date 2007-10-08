#include "State.h"
#include "Primitive.h"
#include "Framework.h"
#include "FrameBuffer.h"
#include "TexCache.h"
#include "RayTraceFramework.h"
#include "HybridFramework.h"
#include "ReyesFramework.h"
#include <iostream>
#include <cstdio>

__BEGIN_QDRENDER

using namespace std;

// all known tokens as table for declared parameter lookup
// some are just id's, not really declared params
static struct // predeclared variable, token & declared param
{
	RtToken tok;
	decParam_t dec;
} tokenTable[] = {
	{ RI_FRAMEBUFFER, {0, 0, 0}},
	{ RI_FILE, {0, 0, 0}},
	{ RI_RGB, {0, 0, 0}},
	{ RI_RGBA, {0, 0, 0}},
	{ RI_RGBZ, {0, 0, 0}},
	{ RI_RGBAZ, {0, 0, 0}},
	{ RI_A, {0, 0, 0}},
	{ RI_Z, {0, 0, 0}},
	{ RI_AZ, {0, 0, 0}},
	{ RI_PERSPECTIVE, {0, 0, 0}},
	{ RI_ORTHOGRAPHIC, {0, 0, 0}},
	{ RI_HIDDEN, {0, 0, 0}},
	{ RI_PAINT, {0, 0, 0}},
	{ RI_CONSTANT, {0, 0, 0}},
	{ RI_SMOOTH, {0, 0, 0}},
	{ RI_FLATNESS, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_FOV, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_AMBIENTLIGHT, {0, 0, 0}},
	{ RI_POINTLIGHT, {0, 0, 0}},
	{ RI_DISTANTLIGHT, {0, 0, 0}},
	{ RI_SPOTLIGHT, {0, 0, 0}},
	{ RI_INTENSITY, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_LIGHTCOLOR, {SC_CONSTANT|DT_COLOR, 1, 3}},
	{ RI_FROM, {SC_CONSTANT|DT_POINT, 1, 3}},
	{ RI_TO, {SC_CONSTANT|DT_POINT, 1, 3}},
	{ RI_CONEANGLE, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_CONEDELTAANGLE, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_BEAMDISTRIBUTION, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_MATTE, {0, 0, 0}},
	{ RI_METAL, {0, 0, 0}},
	{ RI_SHINYMETAL, {0, 0, 0}},
	{ RI_PLASTIC, {0, 0, 0}},
	{ RI_PAINTEDPLASTIC, {0, 0, 0}},
	{ RI_KA, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_KD, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_KS, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_ROUGHNESS, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_KR, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_TEXTURENAME, {SC_CONSTANT|DT_STRING, 1, 0}},
	{ RI_SPECULARCOLOR, {SC_CONSTANT|DT_COLOR, 1, 3}},
	{ RI_DEPTHCUE, {0, 0, 0}},
	{ RI_FOG, {0, 0, 0}},
	{ RI_BUMPY, {0, 0, 0}},
	{ RI_MINDISTANCE, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_MAXDISTANCE, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_BACKGROUND, {SC_CONSTANT|DT_COLOR, 1, 3}},
	{ RI_DISTANCE, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_AMPLITUDE, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_RASTER, {0, 0, 0}},
	{ RI_SCREEN, {0, 0, 0}},
	{ RI_CAMERA, {0, 0, 0}},
	{ RI_WORLD, {0, 0, 0}},
	{ RI_OBJECT, {0, 0, 0}},
	{ RI_INSIDE, {0, 0, 0}},
	{ RI_OUTSIDE, {0, 0, 0}},
	{ RI_LH, {0, 0, 0}},
	{ RI_RH, {0, 0, 0}},
	// ----- THE VARS
	{ RI_P, {SC_VERTEX|DT_POINT, 1, 3}},
	{ RI_PZ, {SC_VERTEX|DT_FLOAT, 1, 1}},
	{ RI_PW, {SC_VERTEX|DT_HPOINT, 1, 4}},
	{ RI_N, {SC_VARYING|DT_NORMAL, 1, 3}},
	{ RI_NP, {SC_UNIFORM|DT_POINT, 1, 3}}, //??? can't find anything on what this is in RiSpec...
	{ RI_CS, {SC_VARYING|DT_COLOR, 1, 3}},
	{ RI_OS, {SC_VARYING|DT_COLOR, 1, 3}},
	{ RI_S, {SC_VARYING|DT_FLOAT, 1, 1}},
	{ RI_T, {SC_VARYING|DT_FLOAT, 1, 1}},
	{ RI_ST, {SC_VARYING|DT_FLOAT, 2, 2}}, // the only array sofar...
	// -----
	{ RI_BILINEAR, {0, 0, 0}},
	{ RI_BICUBIC, {0, 0, 0}},
	{ RI_LINEAR, {0, 0, 0}},
	{ RI_CUBIC, {0, 0, 0}},
	{ RI_PRIMITIVE, {0, 0, 0}},
	{ RI_INTERSECTION, {0, 0, 0}},
	{ RI_UNION, {0, 0, 0}},
	{ RI_DIFFERENCE, {0, 0, 0}},
	{ RI_PERIODIC, {0, 0, 0}},
	{ RI_NONPERIODIC, {0, 0, 0}},
	{ RI_CLAMP, {0, 0, 0}},
	{ RI_BLACK, {0, 0, 0}},
	{ RI_IGNORE, {0, 0, 0}},
	{ RI_PRINT, {0, 0, 0}},
	{ RI_ABORT, {0, 0, 0}},
	{ RI_HANDLER, {0, 0, 0}},
	{ RI_COMMENT, {0, 0, 0}},
	{ RI_STRUCTURE, {0, 0, 0}},
	{ RI_VERBATIM, {0, 0, 0}},
	{ RI_IDENTIFIER, {0, 0, 0}},
	{ RI_NAME, {0, 0, 0}},
	{ RI_SHADINGGROUP, {0, 0, 0}},
	{ RI_WIDTH, {SC_VARYING|DT_FLOAT, 1, 1}},
	{ RI_CONSTANTWIDTH, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_JITTER, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	{ RI_ZFILE, {0, 0, 0}},
	{ RI_SPHERE, {SC_CONSTANT|DT_FLOAT, 1, 1}},
	// extra's
	{ RI_RAYTRACE, {0, 0, 0}},
	{ RI_HYBRID, {0, 0, 0}},
	{ RI_BUCKETSIZE, {SC_CONSTANT|DT_INT, 1, 1}},
	{ RI_TEXTUREMEMORY, {SC_CONSTANT|DT_INT, 1, 1}},
	{ RI_GRIDSIZE, {SC_CONSTANT|DT_INT, 1, 1}},
	{ RI_BINARY, {SC_CONSTANT|DT_INT, 1, 1}}
};

// state create
State::State()
{
	cout << "State created\n";

	// add the predeclared known Ri variables
	int numtok = sizeof(tokenTable) / sizeof(tokenTable[0]);
	for (int n=0; n<numtok; n++)
		addDeclared(tokenTable[n].tok, tokenTable[n].dec);

	// init the hashtable for the ribInterface
	_ribFunc* ribf = ribFuncTable;
	while (ribf->name) {
		_riblu.insert(ribf->name, ribf->func);
		ribf++;
	}

	// defaults
	pushOption();
	pushAttrib();
	newCubicBasis(RiBezierBasis, RiBezierBasis);
	pushTransform();
	// have to push empty objectlight set too, lights can be defined before worldBegin
	pushObjectLight();
	pushMode(MODE_UNDEFINED); // undefined

}

// destructor, alternative singleton design, so this *IS* called at program end
State::~State()
{
	// pop defaults
	popMode();
	popObjectLight();
	popTransform();
	popAttrib();
	popOption();

	cout << "State destructor\n";
	cout << "modeStack size     : " << modeStack.size() << endl;
	cout << "optionStack size   : " << optionStack.size() << endl;
	cout << "attributeStack size: " << attributeStack.size() << endl;
	cout << "transformStack size: " << transformStack.size() << endl;
	cout << "lightStack size    : " << lightStack.size() << endl;
	cout << "objectStack size   : " << objectStack.size() << endl;
}

// create instance of state, only done once
State* State::Instance()
{
	static State state_inst;
	return &state_inst;
}

// rendertimes, another quick hack...
// shouldn't probably really be here
#include <sys/timeb.h>
static struct timeb TYD1, TYD2;
void State::renderStart() const
{
	ftime(&TYD1);
}

void State::renderEnd() const
{
	ftime(&TYD2);
	cout << "rendertime: " << ((TYD2.time - TYD1.time)*1000 + (TYD2.millitm - TYD1.millitm))*0.001f << endl;
	framebuffer->finalize();
}

// parameter declarations
RtVoid State::addDeclared(const string &name, const decParam_t &dp)
{
	if (declaredParams.find(name) != declaredParams.end())
		cout << "[WARNING]: parameter \"" << name << "\" redefined\n";
	declaredParams[name] = dp;
}

const decParam_t* State::findDeclaredParam(const char* name) const
{
	map<string, decParam_t>::const_iterator p = declaredParams.find(name);
	if (p != declaredParams.end()) return &p->second;
	return NULL;
}

// return parameter data defined by name, either predeclared or inline from name.
// for inline, 'last' when set will delete the parameter since it is the last used instance
bool State::parameterFromName(const char* name, decParam_t& dp, char inline_name[256], bool last)
{
	// predeclared parameter?
	const decParam_t* pdp = findDeclaredParam(name);
	if (pdp) {
		dp.ct_flags = pdp->ct_flags;
		dp.arlen = pdp->arlen;
	}
	else {
		// not found (or, since only name is checked, the name might be known,
		// but the string might contain more than just that name,
		// in which case it might be a local redefinition, so this is the correct behaviour)
		// assume inline declaration.
		// First check if in current inline declation list
		map<string, decParam_t>::iterator cid = inlineParams.find(name);
		if (cid != inlineParams.end()) {
			const size_t slen = min(cid->first.length(), (size_t)254);
			strncpy(inline_name, cid->first.c_str(), slen);
			inline_name[slen] = 0;
			dp = cid->second;
			if (last) inlineParams.erase(cid);
			return true;
		}
		// not found, try parsing string
		if (!parseDeclaration(name, &dp, inline_name)) {
			// parse error (and so implicitly also means it is undeclared)
			cout << "[WARNING]: inline declaration error/not declared: \"" << name << "\"\n";
			return false;
		}
		inlineParams[inline_name] = dp;
	}
	return true;
}

// insert a primitive
void State::insert(Primitive* prim, int motion_pos)
{
	verifyMode(MODE_WORLD|MODE_ATTRIBUTE|MODE_TRANSFORM|MODE_SOLID|MODE_OBJECT|MODE_MOTION,
				RIE_NOTPRIMS, "State::insert()");
	/*
	if (OpenObject!=NULL)
		TODO
	else if (openAreaLight)
		TODO
	else */
	if (motion_pos >= 0) {
		if (openBlurPrim == NULL) openBlurPrim = new BlurredPrimitive();
		// set lightarray and std_dice flags first before calling cloneAttributes(),
		Attributes& attr = topAttributes();
		attr.lightsources = cloneLightArray(attr.numlights);
		prim->dice_flags() = 0;
		if (attr.surface_shader) prim->dice_flags() |= attr.surface_shader->shader->globals_used;
		if (attr.displacement_shader) prim->dice_flags() |= attr.displacement_shader->shader->globals_used;
		if (attr.atmosphere_shader) prim->dice_flags() |= attr.atmosphere_shader->shader->globals_used;
		if (attr.interior_shader) prim->dice_flags() |= attr.interior_shader->shader->globals_used;
		if (attr.exterior_shader) prim->dice_flags() |= attr.exterior_shader->shader->globals_used;
		prim->setAttributeReference(cloneAttributes());
		prim->setTransformReference(cloneTransform());
		openBlurPrim->append(prim);
	}
	else if (framework) {
		// set lightarray and std_dice flags first before calling cloneAttributes(),
		Attributes& attr = topAttributes();
		attr.lightsources = cloneLightArray(attr.numlights);
		prim->dice_flags() = 0;
		if (attr.surface_shader) prim->dice_flags() |= attr.surface_shader->shader->globals_used;
		if (attr.displacement_shader) prim->dice_flags() |= attr.displacement_shader->shader->globals_used;
		if (attr.atmosphere_shader) prim->dice_flags() |= attr.atmosphere_shader->shader->globals_used;
		if (attr.interior_shader) prim->dice_flags() |= attr.interior_shader->shader->globals_used;
		if (attr.exterior_shader) prim->dice_flags() |= attr.exterior_shader->shader->globals_used;
		if ((openBlurPrim == NULL) && !motion_xform.empty()) {
			// moving, with single instance base primitive
			BlurredPrimitive* bprim = new BlurredPrimitive();
			// copy the motion transforms to blurred prim, if any
			for(vector<Transform>::const_iterator ti=motion_xform.begin(); ti!=motion_xform.end(); ++ti)
				bprim->addXform(*ti);
			// attributes & transform must be set in base prim
			prim->setAttributeReference(cloneAttributes());
			prim->setTransformReference(cloneTransform());
			// add to bprim
			bprim->append(prim);
			// set the attribute reference for blurred primitive itself, transform never accessed, so can be skipped in this case
			bprim->setAttributeReference(cloneAttributes());
			bprim->post_init(); // do any post initialization if needed
			framework->insert(bprim);
		}
		else {
			if (openBlurPrim && (prim == openBlurPrim)) {
				// inserted primitive is BlurredPrimitive,
				// copy the motion transforms to blurred prim, if any
				for(vector<Transform>::const_iterator ti=motion_xform.begin(); ti!=motion_xform.end(); ++ti)
					openBlurPrim->addXform(*ti);
			}
			// set the attribute and transform references
			prim->setAttributeReference(cloneAttributes());
			prim->setTransformReference(cloneTransform());
			prim->post_init(); // do any post initialization if needed
			framework->insert(prim);
			if (openBlurPrim && (prim == openBlurPrim)) {
				// inserted primitive was BlurredPrimitive, reset for possible next ones.
				// And no, delete is *NOT* missing, inserted primitive is deleted by framework, not here
				openBlurPrim = NULL;
			}
		}
	}
}

//------------------------------------------------------------------------------

RtVoid State::begin(RtToken name)
{
	verifyMode(MODE_UNDEFINED, RIE_ILLSTATE, "State::begin()");
	//openObject = NULL;
	//openAreaLight = NULL;
	openBlurPrim = NULL;
	renderstate = false;
	framework = NULL;
	framebuffer = NULL;
	texcache = NULL;	// created at worldbegin
	resetScrWin = resetFrameAspect = false;
	motion_expected = 0;
	pushMode(MODE_BASE);
}

RtVoid State::end()
{
	verifyMode(MODE_BASE, RIE_NESTING, "State::end()");
	popMode();

	cout << "State::end() -> num. transforms      : " << ref_xform.size() << endl;
	cout << "State::end() -> num. cubic bases     : " << ref_cubs.size() << endl;
	cout << "State::end() -> num. attributes      : " << ref_attr.size() << endl;
	cout << "State::end() -> num. shaders         : " << ref_shd.size() << endl;
	cout << "State::end() -> num. shader instances: " << ref_shdInst.size() << endl;
	cout << "In terms of memory, above 5 as total in bytes: " <<
		(ref_xform.size()*sizeof(Transform) +
		ref_cubs.size()*sizeof(Transform) +
		ref_attr.size()*sizeof(Attributes) +
		ref_shd.size()*sizeof(SlShader) +
		ref_shdInst.size()*sizeof(SlShaderInstance)) << endl;

	if (framebuffer) delete framebuffer;
	framebuffer = NULL;

	for (vector<SlShaderInstance*>::iterator i=ref_shdInst.begin(); i!=ref_shdInst.end(); ++i)
	{
		if (*i) delete *i;
		*i = NULL;
	}
	ref_shdInst.clear();

	for (map<string, SlShader*>::iterator i=ref_shd.begin(); i!=ref_shd.end(); ++i)
	{
		if (i->second) delete i->second;
		i->second = NULL;
	}
	ref_shd.clear();

	for (vector<Transform*>::iterator i=ref_xform.begin(); i!=ref_xform.end(); ++i)
	{
		if (*i) delete *i;
		*i = NULL;
	}
	ref_xform.clear();

	for (vector<Transform*>::iterator i=ref_cubs.begin(); i!=ref_cubs.end(); ++i)
	{
		if (*i) delete *i;
		*i = NULL;
	}
	ref_cubs.clear();

	// delete cloned attributes
	for (vector<Attributes*>::iterator i=ref_attr.begin(); i!=ref_attr.end(); ++i)
	{
		if (*i) delete *i;
		*i = NULL;
	}
	ref_attr.clear();

	// delete cloned lightsource arrays
	cout << "Total lightsource arrays : " << ref_lights.size() << endl;
	for (vector<SlShaderInstance**>::iterator i=ref_lights.begin(); i!=ref_lights.end(); ++i) {
		if (*i) delete[] *i;
		*i = NULL;
	}
	ref_lights.clear();

	renderstate = false;
}

RtVoid State::frameBegin(RtInt frame)
{
	verifyMode(MODE_BASE, RIE_ILLSTATE, "RiBegin() not called?");
	pushMode(MODE_FRAME);
	pushOption();
	pushAttrib();
	pushTransform();
	pushObjectLight();
}

RtVoid State::frameEnd()
{
	verifyMode(MODE_FRAME, RIE_NESTING, "frameEnd()");
	popObjectLight();
	popTransform();
	popAttrib();
	popOption();
	popMode();
}

RtVoid State::worldBegin()
{
	verifyMode(MODE_FRAME|MODE_BASE, RIE_NESTING, "State::worldBegin()");
	// set the standard named transforms
	// set world matrix
	namedCoordSys["world"] = topTransform();
	// camera matrix
	namedCoordSys["camera"] = Transform();
	// camera == current
	namedCoordSys["current"] = Transform();

	// initialization from options
	Options& opt = topOptions();

	// adjust frameAspectRatio and/or display area if needed
	if (resetFrameAspect) { // && (opt.projection != Options::PROJ_ORTHOGRAPHIC)) {
		if (opt.frameAspectRatio > (opt.xRes*opt.pixelAspectRatio/(float)opt.yRes))
			opt.yRes = (int)((float)opt.xRes*opt.pixelAspectRatio/opt.frameAspectRatio);
		else
			opt.xRes = (int)(opt.frameAspectRatio*(float)opt.yRes/opt.pixelAspectRatio);
	}
	if (!resetScrWin) { // init screen win (only when *not* already set)
		if (opt.frameAspectRatio >= 1.f) {
			opt.left = -opt.frameAspectRatio;
			opt.right = opt.frameAspectRatio;
			opt.bottom = -1.f;
			opt.top = 1.f;
		}
		else {
			opt.left = -1.f;
			opt.right = 1.f;
			opt.bottom = -1.f/opt.frameAspectRatio;
			opt.top = 1.f/opt.frameAspectRatio;
		}
	}

	// camera
	cam.init(opt);

	// raster matrix
	namedCoordSys["raster"] = cam.getRas2Cam();
	// NDC matrix
	Transform NDC = Scale(1.f/cam.getWidth(), 1.f/cam.getHeight(), 1.f) * cam.getCam2Ras();
	NDC.invert();
	namedCoordSys["NDC"] = NDC;
	// screen matrix
	namedCoordSys["screen"] = cam.getScr2Cam();

	// initialize framebuffer
	framebuffer = new FrameBuffer(topOptions());

	if (opt.hider == Options::HD_HIDDEN)
		framework = new ReyesFramework(topAttributes(), opt);
	else if (opt.hider == Options::HD_RAYTRACE)
		framework = new RayTraceFramework(topAttributes(), opt);
	else if (opt.hider == Options::HD_HYBRID)
		framework = new HybridFramework(topAttributes(), opt);
	else	// 'hidden' default
		framework = new ReyesFramework(topAttributes(), opt);

	// Initialize texture cache
	texcache = new TextureCache(topOptions().texturememory);

	//renderStart();
	framework->worldBegin();

	pushMode(MODE_WORLD);
	pushTransform();
	pushAttrib();
	pushObjectLight();
}

RtVoid State::worldEnd()
{
	verifyMode(MODE_WORLD, RIE_NESTING, "State::worldEnd()");
	popObjectLight();
	popAttrib();
	popTransform();
	popMode();

	// THE main call, where all the real work starts...
	renderStart();
	if (framework) {
		renderstate = true;
		framework->worldEnd();
	}
	else
		throw RiException_t("State::worldEnd() no framework!? worldBegin() not called?");

	delete texcache;
	texcache = NULL;

	delete framework;
	framework = NULL;

}

RtVoid State::attributeBegin()
{
	// any mode but object
	verifyMode(~MODE_OBJECT, RIE_NOTATTRIBS, "State::attributeBegin()");
	pushMode(MODE_ATTRIBUTE);
	pushTransform();
	pushObjectLight();	// also needed here
	pushAttrib();
}

RtVoid State::attributeEnd()
{
	verifyMode(MODE_ATTRIBUTE, RIE_NESTING, "attributeEnd()");
	popAttrib();
	popObjectLight();
	popTransform();
	popMode();
}

RtVoid State::transformBegin()
{
	pushMode(Mode(modeStack.top() | MODE_TRANSFORM));
	pushTransform();
}

RtVoid State::transformEnd()
{
	verifyMode(MODE_TRANSFORM, RIE_NESTING, "transformEnd()");
	popTransform();
	popMode();
}

RtVoid State::solidBegin(RtToken operation)
{
	pushMode(MODE_SOLID);
	pushAttrib();
	pushTransform();
}

RtVoid State::solidEnd()
{
	popMode();
	popTransform();
	popAttrib();
}

RtVoid State::motionBegin(RtInt n, RtFloat times[])
{
	pushMode(MODE_MOTION);
	// for now times array is ignored, it is assumed that it is equivalent to the shutter time values, so only two shutter times
	motion_expected = n;
}

RtVoid State::motionEnd()
{
	popMode();
	motion_expected = 0;
	// if geometry was defined in motion block, primitive set can now be inserted into framework
	if (openBlurPrim) insert(openBlurPrim, -1);
}

// function to free all data build inside motion block in case errors occurred
void State::cancelMotion()
{
	motion_xform.clear();
	motion_expected = 0;
	if (openBlurPrim) {
		delete openBlurPrim;
		openBlurPrim = NULL;
	}
}

RtObjectHandle State::objectBegin()
{
	pushAttrib();
	pushTransform();
	return &objectStack.top();
}

RtVoid State::objectEnd()
{
	popTransform();
	popAttrib();
}

// disable/enable light in current lightsource list
RtVoid State::setLight(RtLightHandle light, bool on)
{
	if (on)
		lightStack.top().insert(light);
	else
		lightStack.top().erase(light);
}

// convert rib integer light ID's to lighthandles
void State::setLightID(RtLightHandle light, int ID)
{
	lightID2LH[ID] = light;
}

RtLightHandle State::getLightHandle(int ID) const
{
	map<int, RtLightHandle>::const_iterator LH = lightID2LH.find(ID);
	if (LH == lightID2LH.end()) {
		cout << "[WARNING]: State::getLightHandle() -> unknown rib lightsource ID: " << ID << endl;
		return NULL;
	}
	return LH->second;
}

//------------------------------------------------------------------------------

// verify if mode is valid, if not, throw error
bool State::verifyMode(ModeSet allowableModes, RtInt errnumIfDifferent, const string &from)
{
	// MODE_UNDEFINED = 0, so have to test for that differently (0 == false, -> exception)
	if ((allowableModes == MODE_UNDEFINED) && (modeStack.top() == MODE_UNDEFINED)) return true;
	if (modeStack.top() & allowableModes) return true;
	try {
		cout << "ERROR: " << errnumIfDifferent << " allowableModes = " << (int)allowableModes
		     << " current Mode = " << (int)modeStack.top() << endl;
		switch (errnumIfDifferent) {
			case RIE_NESTING:
				throw RiException_t(string(from + " nesting error").c_str());
			case RIE_ILLSTATE:
				throw RiException_t(string(from + " invalid state").c_str());
			case RIE_NOTSTARTED:
				throw RiException_t(string(from + " RiBegin() not called").c_str());
			default: {
				char msg[256];
				#ifdef _MSC_VER
				_snprintf(msg, 256, "%s errornumber: %d", from.c_str(), errnumIfDifferent);
				#else
				snprintf(msg, 256, "%s errornumber: %d", from.c_str(), errnumIfDifferent);
				#endif
				throw RiException_t(msg);
			}
		}
	}
	catch (RiException_t&) {
		if (errnumIfDifferent != RIE_NESTING) exit(EXIT_FAILURE);
		return false;
	}
}

//------------------------------------------------------------------------------
// copy of top-of-stack attributes.
// will try to re-use any already stored if possible.
// Uses hash table indexed by hash of the actual attribute contents, so a hash of a hash...
//(Paul Hsieh's hash function works very well in this case,
// trying to do the same with crc64 gave almost 30000 misses in one case,
// while using the hash function gave only 1...)
Attributes* State::cloneAttributes()
{
	// current attributes
	Attributes& cur_attr = topAttributes();
	// hash of contents
	const unsigned int ha = hashfunc(reinterpret_cast<const unsigned char*>(&cur_attr), sizeof(Attributes));
	// a durable copy is always made of the very first attribute
	if (!ref_attr.empty()) {
		// references not empty, re-use of already allocated ones possible?
		Attributes** ru_attr = hash_attr.find(ha);
		if (ru_attr) {
			// do a memcmp() to make sure
			if (memcmp(*ru_attr, &cur_attr, sizeof(Attributes)) == 0) return *ru_attr;
		}
	}
	// first attribute, nothing found, or miss, so make a new durable copy
	Attributes* new_attr = new Attributes(cur_attr);
	ref_attr.push_back(new_attr);
	hash_attr.insert(ha, new_attr);
	return new_attr;
}

// as above, the same for all Transforms
Transform* State::cloneTransform()
{
	Transform& cur_xform = topTransform();
	const unsigned int hx = hashfunc(reinterpret_cast<const unsigned char*>(&cur_xform), sizeof(Transform));
	if (!ref_xform.empty()) {
		Transform** ru_xfrm = hash_xfrm.find(hx);
		if (ru_xfrm) {
			if (memcmp(*ru_xfrm, &cur_xform, sizeof(Transform)) == 0) return *ru_xfrm;
		}
	}
	Transform* xf = new Transform(cur_xform);
	ref_xform.push_back(xf);
	hash_xfrm.insert(hx, xf);
	return xf;
}

// and another, for lightsource arrays
SlShaderInstance** State::cloneLightArray(unsigned int &numlights)
{
	set<RtLightHandle>& cur_lights = lightStack.top();
	numlights = cur_lights.size();
	if (numlights == 0) return NULL;	// no lights
	// new light array, temporary if re-use of previous possible
	SlShaderInstance** nla = new SlShaderInstance*[numlights];
	unsigned int lidx = 0;
	for (set<RtLightHandle>::iterator si=cur_lights.begin(); si!=cur_lights.end(); ++si)
		nla[lidx++] = reinterpret_cast<SlShaderInstance*>(*si);
	// re-use possible?
	const size_t nla_bytes = sizeof(SlShaderInstance*)*numlights;
	const unsigned int hl = hashfunc(reinterpret_cast<const unsigned char*>(nla), nla_bytes);
	if (!ref_lights.empty()) {
		SlShaderInstance*** ru_light = hash_lights.find(hl);	// !!!
		if (ru_light) {
			if (memcmp(*ru_light, nla, nla_bytes) == 0) {
				delete[] nla;
				return *ru_light;
			}
		}
	}
	ref_lights.push_back(nla);
	hash_lights.insert(hl, nla);
	return nla;
}

// new attribute cubic bases are set with this function, so it is not necessary to
// have two actual matrices (32 floats!) per every attribute, only pointers.
// Since custom bases are rarely used, and even for the default types usually
// only a single set is used at a time, for most ribs, this will be only two pointers.
// So a hashtable is in this case not needed.
void State::newCubicBasis(RtBasis ubasis, RtBasis vbasis)
{
	Transform *uru = NULL, *vru = NULL;
	// U basis re-use possible?
	vector<Transform*>::iterator ci = ref_cubs.begin();
	for (; ci!=ref_cubs.end(); ++ci) {
		if (memcmp((*ci)->getRtMatrixPtr(), ubasis, sizeof(Transform)) == 0) {
			uru = *ci;
			break;
		}
	}
	// V basis re-use possible?
	ci = ref_cubs.begin();
	for (; ci!=ref_cubs.end(); ++ci) {
		if (memcmp((*ci)->getRtMatrixPtr(), vbasis, sizeof(Transform)) == 0) {
			vru = *ci;
			break;
		}
	}
	Attributes& attr = topAttributes();
	if (uru)
		attr.cubicBasisMatrix[0] = uru;
	else {
		attr.cubicBasisMatrix[0] = new Transform(ubasis);
		ref_cubs.push_back(attr.cubicBasisMatrix[0]);
	}
	if (vru)
		attr.cubicBasisMatrix[1] = vru;
	else {
		attr.cubicBasisMatrix[1] = new Transform(vbasis);
		ref_cubs.push_back(attr.cubicBasisMatrix[1]);
	}
}

//------------------------------------------------------------------------------
// internal methods called by begin and end methods
void State::pushMode(Mode m)
{
	modeStack.push(m);
}

void State::popMode()
{
	if (modeStack.empty())
		throw RiException_t("mode stack empty!");
	modeStack.pop();
}

// push option on stack
void State::pushOption()
{
	optionStack.push();
}

void State::popOption()
{
	if (optionStack.empty())
		throw RiException_t("option stack empty!");
	optionStack.pop();
}

void State::pushAttrib()
{
	attributeStack.push();
}

void State::popAttrib()
{
	if (attributeStack.empty())
		throw RiException_t("attribute stack empty!");
	attributeStack.pop();
}

void State::pushTransform()
{
	transformStack.push();
}

void State::popTransform()
{
	if (transformStack.empty())
		throw RiException_t("transform stack empty!");
	transformStack.pop();
}

void State::pushObjectLight()
{
	// either I'm doing something wrong or it is not explained correctly in pr.book,
	// but unlike pr.book suggestions, have to push a copy of top of stack
	// instead of always push an empty set, otherwise results are simply wrong...
	// so lightStack could be made a copystack instead
	if (lightStack.empty())
		lightStack.push(set<RtLightHandle>());
	else
		lightStack.push(set<RtLightHandle>(lightStack.top()));
	// empty object set
	objectStack.push(set<RtObjectHandle>());
}

void State::popObjectLight()
{
	// lights
	if (lightStack.empty())
		throw RiException_t("light stack empty!");
	lightStack.pop();
	// objects
	if (objectStack.empty())
		throw RiException_t("object stack empty!");
	objectStack.pop();
}

// not in pr book, for named coordinate systems
void State::addNamedCoordSys(const string &name)
{
	namedCoordSys[name] = Transform(topTransform());
}

void State::activateNamedCoordSys(const string &name)
{
	map<string, Transform>::iterator ncs = namedCoordSys.find(name);
	if (ncs != namedCoordSys.end())
		transformStack.top() = ncs->second;
	else
		cout << "Unknown coordinate system \"" << name << "\"\n";
}

Transform* State::getNamedCoordSys(const string &name)
{
	map<string, Transform>::iterator ncs = namedCoordSys.find(name);
	if (ncs != namedCoordSys.end()) return &ncs->second;
	return NULL;
}

Transform State::transformIdentity()
{
	if (modeStack.top() & MODE_WORLD) {
		const Transform* w = getNamedCoordSys("world");
		if (w) return Transform(*w);
	}
	return Transform();
}

Transform& State::currentTransform(int motion_pos)
{
	if (motion_pos >= 0) {
		verifyMode(MODE_MOTION, RIE_BADMOTION, "State::currentTransform()");
		if ((motion_pos == 0) && motion_xform.empty()) {
			// initialize motion transform vector
			motion_xform.resize(motion_expected);	// sets all to identity
		}
		return motion_xform.at(motion_pos);
	}
	return transformStack.top();
}

// add a shader reference
SlShaderInstance* State::loadShader(const char* name, SlShaderType expected_type)
{
	SlShader* sh;
	map<string, SlShader*>::iterator si = ref_shd.find(name);
	if (si != ref_shd.end()) {
		// already available
		sh = si->second;
	}
	else {
		// not found, load from file,
		// test first if already tried to load without success
		if (failed_shaders.find(name) != failed_shaders.end()) {
			// yes, already tried, no use doing it again...
			return NULL;
		}

		// fixed searchpath for now, make user option later
		string searchpath = "shaders/";
		string fname = searchpath + string(name) + ".sqd";
		// load from file
		FILE* fp = fopen(fname.c_str(), "r");
		if (fp==NULL) {
			cout << "[WARNING] State::loadShader() -> Could not load shader \"" << name << "\"\n";
			// save the name, so that next time same file is requested it can be skipped
			failed_shaders.insert(name);
			return NULL;
		}

		// NOTE: this should be removed and read file in parse function itself.
		// This is a remnant of the testing phase when sourcecode could be passed to the loader to test the VM directly with handwritten code

		// determine file size and read
		long curpos = ftell(fp);
		fseek(fp, 0L, SEEK_END);
		long length = ftell(fp);
		fseek(fp, curpos, SEEK_SET);
		char* scode = new char[length + 1];
		fread(scode, length, 1, fp);
		fclose(fp);
		scode[length] = 0;

		// parse the sourcecode
		sh = parseVMsource(scode, expected_type);
		delete[] scode;

		if (sh == NULL) {
			// error in parse
			cout << "[WARNING] State::loadShader() -> shader \"" << name << "\" parse error\n";
			return NULL;
		}

		// ok, add to shader references
		cout << "Shader \"" << name << "\" OK\n";
		ref_shd[name] = sh;
	}

	SlShaderInstance* SI = new SlShaderInstance();
	SI->shader = sh;
	// set the current transform
	SI->xform = cloneTransform();
	ref_shdInst.push_back(SI);

	// set shader in current attributes
	Attributes& attr = topAttributes();
	if (sh->type == SL_SURFACE)
		attr.surface_shader = SI;
	else if (sh->type == SL_DISPLACEMENT)
		attr.displacement_shader = SI;
	else if (sh->type == SL_VOLUME)
		attr.atmosphere_shader = SI;
	else if (sh->type == SL_LIGHT) {
		// add to current active lightset, lightsource list for attributes is created in State::insert()
		lightStack.top().insert(SI);
	}
	else if (sh->type == SL_IMAGER)
		cout << "Imager shaders not supported yet\n";
	else	// never gets here since shader already tested for correct expected type
		cout << "Unknown shader type? ( " << sh->type << " )\n";	// eh?

	return SI;
}

//------------------------------------------------------------------------------

__END_QDRENDER
