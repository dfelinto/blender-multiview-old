#ifndef _REYESFRAMEWORK_H
#define _REYESFRAMEWORK_H

#include "Framework.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class Attributes;
class Options;

class ReyesFramework : public Framework
{
public:
	ReyesFramework(const Attributes& attr, const Options &opt);
	virtual ~ReyesFramework();
	virtual void worldBegin() const;
	virtual void insert(Primitive*) const;
	virtual void remove(const Primitive*) const;
	virtual void worldEnd() const;
	virtual Color trace(const Point3 &p, const Vector &r) const;
};

__END_QDRENDER

#endif //_REYESFRAMEWORK_H
