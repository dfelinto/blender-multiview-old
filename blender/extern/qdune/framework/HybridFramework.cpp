//------------------------------------------------------------------------
// Not implemented yet, although it does render, but without raytracing.
//------------------------------------------------------------------------

#include "HybridFramework.h"
#include "ReyesFramework.h"
#include "RayTraceFramework.h"

#include "Color.h"

__BEGIN_QDRENDER

HybridFramework::HybridFramework(const Attributes& attr, const Options &opt)
{
	reyes = new ReyesFramework(attr, opt);
	raytrace = new RayTraceFramework(attr, opt);
}

HybridFramework::~HybridFramework()
{
	if (reyes) { delete reyes;  reyes = NULL; }
	if (raytrace) { delete raytrace;  raytrace = NULL; }
}

void HybridFramework::worldBegin() const
{
	reyes->worldBegin();
	raytrace->worldBegin();
}

void HybridFramework::insert(Primitive* prim) const
{
	reyes->insert(prim);
	raytrace->insert(prim);
}

void HybridFramework::remove(const Primitive* prim) const
{
	reyes->remove(prim);
	raytrace->remove(prim);
}

void HybridFramework::worldEnd() const
{
	reyes->worldEnd();
}

Color HybridFramework::trace(const Point3 &p, const Vector &r) const
{
	return raytrace->trace(p, r);
}

__END_QDRENDER
