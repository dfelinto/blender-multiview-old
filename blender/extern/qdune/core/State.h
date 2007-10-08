#ifndef STATE_H
#define STATE_H

#include "ri.h"
#include "Options.h"
#include "Attributes.h"
//#include "Primitive.h"
#include "Transform.h"
//#include "Framework.h"
//#include "FrameBuffer.h"
#include "Camera.h"
#include "decparam.h"
#include "ribInterface.h"
#include "qdtl.h"
//#include "TexCache.h"
#include "slshader.h"
#include <stack>
#include <vector>
#include <map>
#include <set>

#include "QDRender.h"
__BEGIN_QDRENDER

// Mode & ModeSet defined as bitsets
// ModeSet == 0 -> Mode undefined
enum Mode {MODE_UNDEFINED=0, MODE_BASE=1, MODE_FRAME=2, MODE_WORLD=4, MODE_ATTRIBUTE=8,
					MODE_TRANSFORM=16, MODE_SOLID=32, MODE_OBJECT=64, MODE_MOTION=128};
typedef int ModeSet;


class Transform;
class FrameBuffer;
class Framework;
class Primitive;
class BlurredPrimitive;
class TextureCache;


// CopyStack class for 'copy on push', see prbook
template<typename T>
class CopyStack : public std::vector<T>
{
public:
	void push()
	{
		if (std::vector<T>::empty())
			std::vector<T>::push_back( T() );
		else
			std::vector<T>::push_back( T( top() ) );
	}
	void pop()
	{
		if (std::vector<T>::empty()) throw RiException_t("pop() -> CopyStack EMPTY!");
		std::vector<T>::pop_back();
	}
	T& top()
	{
		if (std::vector<T>::empty()) throw RiException_t("top() -> CopyStack EMPTY!");
		return std::vector<T>::back();
	}
};



// Singleton State class, but unlike implementation example from prbook,
// this one *is* properly destructed at end of program
class State {
public:
	static State *Instance();
	~State();
protected:
	State();

public:
	// parameter declarations
	RtVoid addDeclared(const std::string &name, const decParam_t &dp);
	const decParam_t* findDeclaredParam(const char* name) const;
	// return parameter data defined by name, either predeclared or inline from name.
	// for inline, 'last' when set will delete the parameter since it is the last used instance
	bool parameterFromName(const char* name, decParam_t& dp, char varname[256], bool last=false);

	// Changing states
	RtVoid begin(RtToken name);
	RtVoid end();
	RtVoid frameBegin(RtInt frame);
	RtVoid frameEnd();
	RtVoid worldBegin();
	RtVoid worldEnd();
	RtVoid attributeBegin();
	RtVoid attributeEnd();
	RtVoid transformBegin();
	RtVoid transformEnd();
	RtVoid solidBegin(RtToken operation);
	RtVoid solidEnd();
	RtVoid motionBegin(RtInt n, RtFloat times[]);
	RtVoid motionEnd();
	// extra function, to free all data build inside motion block in case errors occurred
	void cancelMotion();

	RtObjectHandle objectBegin();
	RtVoid objectEnd();
	// disable/enable light in current lightsource list
	RtVoid setLight(RtLightHandle light, bool state);
	// convert rib integer light ID's to lighthandles
	void setLightID(RtLightHandle light, int ID);
	RtLightHandle getLightHandle(int ID) const;

	// verify if mode is valid
	bool verifyMode(ModeSet allowableModes, RtInt errnumIfDifferent, const std::string &from);

	// top of stack access
	Options &topOptions() { return optionStack.top(); }
	Attributes &topAttributes() { return attributeStack.top(); }
	Transform &topTransform() { return transformStack.top(); }

	// copy of top-of-stack attribs, xform & lightsource arrays
	Attributes* cloneAttributes();
	Transform* cloneTransform();
	SlShaderInstance** cloneLightArray(unsigned int &numlights);

	// new attribute cubic bases
	void newCubicBasis(RtBasis ubasis, RtBasis vbasis);

	// Add constructed primitive to object, light, blur, or framework.
	void insert(Primitive *prim, int motion_pos);

	// named coordinate systems
	void addNamedCoordSys(const std::string &name);
	Transform* getNamedCoordSys(const std::string &name);
	void activateNamedCoordSys(const std::string &name);
	// sets current transform to start of block transform
	Transform transformIdentity();
	// returns current transform, usually top of transformStack, but can be different when in motion block,
	// in which case 'motion_pos' should be current position in motion block, default -1 is outside of motion blocks
	Transform& currentTransform(int motion_pos=-1);

	// load a shader from file if not already in database
	SlShaderInstance* loadShader(const char* name, SlShaderType expected_type);

	// used for timing render
	void renderStart() const;
	void renderEnd() const;	

	// get a reference to the texture cache
	const TextureCache* getTextureCache() const { return texcache; }

	// only used in subdivision code, prevents culled patches from deleting subd.data,
	// see getSubdivData() in CCSubdivision.cpp
	bool rendering() const { return renderstate; }

	// some public data
	
	// flags set by RiScreenWindow() / RiFrameAspectRatio(),
	// frameAspectRatio and/or display area may need to be adjusted
	bool resetScrWin, resetFrameAspect;

	// temporary
	Camera cam, projcam;	// projection camera init. by hider

	// this only used by ribInterface, lookup of Rib callbacks
	hashmap_t<RIBFUNC> _riblu;

	// current framebuffer, used by the hiders
	FrameBuffer* framebuffer;

protected:
	// cubic basis 'database'
	std::vector<Transform*> ref_cubs;

	// shader 'database'
	std::map<std::string, SlShader*> ref_shd; // shaders
	std::vector<SlShaderInstance*> ref_shdInst;  // instances
	// keep track of all shaders that failed to load
	std::set<std::string> failed_shaders;

	// reference to all cloned attributes
	std::vector<Attributes*> ref_attr;
	hashtable_t<unsigned int, Attributes*, 10> hash_attr;

	// reference to all cloned transforms
	std::vector<Transform*> ref_xform;
	hashtable_t<unsigned int, Transform*, 10> hash_xfrm;

	// reference to all cloned lightsource arrays
	std::vector<SlShaderInstance**> ref_lights;
	hashtable_t<unsigned int, SlShaderInstance**, 10> hash_lights;

	// list of named coordinate systems
	std::map<std::string, Transform> namedCoordSys;

	// for motion blur, transforms at specified times
	std::vector<Transform> motion_xform;

	// list of declared parameters
	std::map<std::string, decParam_t> declaredParams;
	// list of inline declared parameters, all temporary, lasting only for the current Ri func
	std::map<std::string, decParam_t> inlineParams;

	std::stack<Mode> modeStack;
	CopyStack<Options> optionStack;
	CopyStack<Attributes> attributeStack;
	CopyStack<Transform> transformStack;
	// prbook has these two combined as one stack
	std::stack<std::set<RtLightHandle> > lightStack;
	std::stack<std::set<RtObjectHandle> > objectStack;
	// rib integer light ID's to lighthandle mapper
	std::map<int, RtLightHandle> lightID2LH;

	// internal methods called by begin and end methods
	void pushMode(Mode m);        void popMode();
	void pushOption();            void popOption();
	void pushAttrib();            void popAttrib();
	void pushTransform();         void popTransform();
	void pushObjectLight();       void popObjectLight();

	// in object mode, motion mode, or when defining an area light,
	// gather primitives here, Otherwise, these pointers will all
	// be NULL and the primitives will pass through to the Framework.
	//RtObjectHandle openObject;
	//RtLightHandle openAreaLight;
	BlurredPrimitive* openBlurPrim;
	// number of commands expected in motion block
	int motion_expected;

	// current framework
	Framework* framework;

	// texture cache
	TextureCache* texcache;

	// only used in subdivision code, prevents culled patches to delete subd.data,
	// see getSubdivData() in CCSubdivision.cpp
	bool renderstate;
};


__END_QDRENDER

#endif // STATE_H
