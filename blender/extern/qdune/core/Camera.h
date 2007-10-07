#ifndef _CAMERA_H
#define _CAMERA_H

#include "qdVector.h"
#include "Transform.h"

#include "QDRender.h"
__BEGIN_QDRENDER

class Options;
class Camera
{
private:
	Camera(const Camera&);
	//Camera& operator=(const Camera&);
public:
	Camera():width(0), height(0), fov(0), lrad(0), fdist(0), inv_fdist(0), aspect(0), ortho(false) {}
	Camera(const Options& opt) { init(opt); }
	~Camera() {}
	void init(const Options& opt);
	ray_t eyeRay(float x, float y);
	ray_t eyeRay_FB(float x, float y);
	Point3 project(const Point3 &w) const;
	const Vector& getUp() const { return up; }
	const Point3& getEye() const { return eye; }
	const Point3& getAim() const { return aim; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	float getFOV() const { return fov; }
	float getAperture() const { return lrad; }
	void setFDist(float fd) { fdist = fd;  inv_fdist = (fd==0.f) ? 1.f : (1.f/fd); }
	float getFDist() const { return fdist; }
	float getInvFDist() const { return inv_fdist; }
	bool isOrthographic() const { return ortho; }
	Transform getWld2Cam() const { return wld2cam; }
	Transform getCam2Ras() const { return cam2ras; }
	Transform getRas2Cam() const { return ras2cam; }
	Transform getScr2Cam() const { return scr2cam; }
protected:
	Transform wld2cam, wld2scr;
	Transform cam2wld, cam2scr;
	Transform cam2ras, ras2cam;
	Transform ras2scr, scr2ras;
	Transform scr2cam;
	Point3 eye, aim;
	Vector up;
	int width, height;
	float fov, lrad, fdist, inv_fdist, aspect, nearclip, farclip;
	bool ortho;
};

__END_QDRENDER

#endif  // _CAMERA_H
