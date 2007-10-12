#include "ReyesFramework.h"
#include "ZbufferHider.h"
#include "Primitive.h"
#include "Color.h"
#include "MicroPolygonGrid.h"
#include "Options.h"

#include <iostream>

#include "QDRender.h"
__BEGIN_QDRENDER

ReyesFramework::ReyesFramework(const Attributes& attr, const Options& opt)
{
	hider = new ZbufferHider(attr, opt);
	max_eyesplits = opt.eyesplits;
	total_eyesplits = 0;
}

ReyesFramework::~ReyesFramework()
{
	if (hider) { delete hider;  hider = NULL; }
}

void ReyesFramework::worldBegin() const
{
	hider->worldBegin();
}

void ReyesFramework::remove(const Primitive *prim) const
{
	hider->remove(prim);
}

// no raytracing here
Color ReyesFramework::trace(const Point3 &p, const Vector &r) const
{
	return Color(0);
}

void ReyesFramework::insert(Primitive* prim) const
{
	int v = hider->inFrustum(prim);
	if (v == IS_VISIBLE)
		hider->insert(prim);
	else if (v == EYE_SPLIT) {
		if (prim->splitable()) {
			if (prim->getEyeSplits() < max_eyesplits) {
				prim->incEyeSplits();
				prim->split(*this, true, true);
			}
			else	// max split exceeded, discard...
				++total_eyesplits;
			delete prim;
		}
	}
	else // not visible
		delete prim;
}


void ReyesFramework::worldEnd() const
{
	if (total_eyesplits)
		std::cout << "\rExceeded maximum eyesplitcount " << total_eyesplits << " times" << std::endl;
	while (hider->bucketBegin()) {
		Primitive* p;
		while ((p = hider->firstPrim()) != NULL) {
			MicroPolygonGrid g;
			bool usplit, vsplit;
			if (p->diceable(g, *hider, usplit, vsplit)) {
				p->dice(g);
				//if (!g.isBackfacing() && !g.trim()) {
					//if (!hider->gridOccluded(g)) {
						g.displace();
						if (!hider->noShading()) g.shade();	// no shading for depth renders
						hider->hide(g);
					//}
				//}
			}
			else {
				if (p->splitable()) p->split(*this, usplit, vsplit);
			}
			// prim can now be deleted
			delete p;
		}
		hider->bucketEnd();
	}
	hider->worldEnd();
}

__END_QDRENDER
