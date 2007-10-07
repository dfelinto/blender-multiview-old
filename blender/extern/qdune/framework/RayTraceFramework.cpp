//--------------------------------------------------------
// Completely unimplemented skeleton code.
// The framework for pure raytracing. TODO
//--------------------------------------------------------

#include "RayTraceFramework.h"

#include "RayTraceHider.h"
#include "Color.h"

__BEGIN_QDRENDER

RayTraceFramework::RayTraceFramework(const Attributes& attr, const Options &opt)
{
	hider = new RayTraceHider(attr, opt);
}

RayTraceFramework::~RayTraceFramework()
{
	if (hider) { delete hider;  hider = NULL; }
}

void RayTraceFramework::worldBegin() const
{
	hider->worldBegin();
}

void RayTraceFramework::insert(Primitive* p) const
{
	hider->insert(p);
}

void RayTraceFramework::remove(const Primitive* p) const
{
	hider->remove(p);
}

Color RayTraceFramework::trace(const Point3 &p, const Vector &r) const
{
	return hider->trace(p, r);
}

void RayTraceFramework::worldEnd() const
{
	while (hider->bucketBegin())
		hider->bucketEnd();
	hider->worldEnd();
}

__END_QDRENDER
