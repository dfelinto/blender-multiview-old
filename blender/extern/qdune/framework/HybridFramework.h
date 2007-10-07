#ifndef _HYBRIDFRAMEWORK_H
#define _HYBRIDFRAMEWORK_H

#include "Framework.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class Attributes;
class Options;
class ReyesFramework;
class RayTraceFramework;
class HybridFramework : public Framework
{
public:
	HybridFramework(const Attributes& attr, const Options &opt);
	virtual ~HybridFramework();
	virtual void worldBegin() const;
	virtual void insert(Primitive*) const;
	virtual void remove(const Primitive*) const;
	virtual void worldEnd() const;
	virtual Color trace(const Point3 &p, const Vector &r) const;
protected:
	ReyesFramework* reyes;
	RayTraceFramework* raytrace;
};

__END_QDRENDER

#endif //_HYBRIDFRAMEWORK_H
