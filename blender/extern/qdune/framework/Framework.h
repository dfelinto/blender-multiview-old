#ifndef _FRAMEWORK_H
#define _FRAMEWORK_H

#include "QDRender.h"
__BEGIN_QDRENDER

class Color;
class Point3;
class Vector;
class Hider;
class Primitive;

class Framework
{
public:
	Framework():hider(NULL) {}
	virtual ~Framework() {}
	virtual void worldBegin() const=0;
	virtual void insert(Primitive*) const=0;
	virtual void remove(const Primitive*) const=0;
	virtual void worldEnd() const=0;
	virtual Color trace(const Point3 &p, const Vector &r) const=0;
protected:
	Hider* hider;
};

__END_QDRENDER

#endif //_FRAMEWORK_H
