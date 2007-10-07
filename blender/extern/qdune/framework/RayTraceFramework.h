#ifndef _RAYTRACEFRAMEWORK_H
#define _RAYTRACEFRAMEWORK_H

#include "Framework.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class Attributes;
class Options;

class RayTraceFramework : public Framework
{
public:
	RayTraceFramework(const Attributes& attr, const Options &opt);
	virtual ~RayTraceFramework();
	virtual void worldBegin() const;
	virtual void insert(Primitive*) const;
	virtual void remove(const Primitive*) const;
	virtual void worldEnd() const;
	virtual Color trace(const Point3 &p, const Vector &r) const;
};

__END_QDRENDER

#endif //_RAYTRACEFRAMEWORK_H
