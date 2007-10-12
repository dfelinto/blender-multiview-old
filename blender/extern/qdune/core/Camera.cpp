#include "Camera.h"
#include "Transform.h"
#include "Bound.h"
#include "Mathutil.h"
#include "Options.h"

#include "QDRender.h"
__BEGIN_QDRENDER

void Camera::init(const Options& opt)
{
	// default eye, aim & up
	eye.set(0, 0, 0);
	aim.set(0, 0, 1);
	up.set(0, 1, 0);
	
	fov = opt.fov;
	fdist = opt.focalDistance;
	inv_fdist = 1.f/fdist;
	lrad = 0.5f * opt.focalLength / opt.fStop;	// aperture
	width = opt.xRes;
	height = opt.yRes;

	wld2cam = LookAt(eye, aim, up);
	cam2wld = wld2cam;
	cam2wld.invert();
	ortho = (opt.projection == Options::PROJ_ORTHOGRAPHIC);
	nearclip = opt.nearClip;
	farclip = opt.farClip;
	if (ortho)
		cam2scr = Scale(1.f, 1.f, 1.f/(farclip - nearclip)) * Translate(0.f, 0.f, -nearclip);
	else
		cam2scr = Perspective(fov, nearclip, farclip);

	wld2scr = cam2scr * wld2cam;
	aspect = opt.pixelAspectRatio;
	float scr[4] = {opt.left, opt.right, opt.bottom, opt.top};
	scr2ras = Scale((float)width, (float)height, 1.f) *
	          Scale(1.f/(scr[1] - scr[0]), 1.f/(scr[2] - scr[3]), 1.f) *
	          Translate(-scr[0], -scr[3], 0.f);
	ras2scr = scr2ras;
	ras2scr.invert();
	ras2cam = cam2scr;
	ras2cam.invert();
	ras2cam = ras2cam * ras2scr;
	
	// camera to raster space mtx for projection
	cam2ras = ras2cam;
	cam2ras.invert();

	scr2cam = cam2scr;
	scr2cam.invert();
}

Point3 Camera::project(const Point3 &w) const
{
	// only need x/y, so don't need to do full matrix mult.
	// assumes eye always origin
	if (ortho) {
		return Point3(cam2ras[0][0]*w.x + cam2ras[0][1]*w.y + cam2ras[0][2]*w.z + cam2ras[0][3],
		              cam2ras[1][0]*w.x + cam2ras[1][1]*w.y + cam2ras[1][2]*w.z + cam2ras[1][3],
		              w.z);
	}
	// perspective, cam2ras[0/1][3] always zero
	Point3 p(cam2ras[0][0]*w.x + cam2ras[0][1]*w.y + cam2ras[0][2]*w.z,
	         cam2ras[1][0]*w.x + cam2ras[1][1]*w.y + cam2ras[1][2]*w.z,
	         w.z);
	if (w.z != 0.f) { const float t = 1.f/w.z;  p.x *= t;  p.y *= t; }
	return p;
}

ray_t Camera::eyeRay(float x, float y)
{
	
	return ray_t(eye, (cam2wld*toVector(ras2cam*Point3(x, y, 0))).normalize(),
	                  (cam2wld*toVector(ras2cam*Point3(x+1, y, 0))).normalize(),
	                  (cam2wld*toVector(ras2cam*Point3(x, y+1, 0))).normalize());
}


ray_t Camera::eyeRay_FB(float x, float y)
{
	/*
	const Point3 Pcam(ras2cam * point3d_t(x, y, 0));
	Vector direction(toVector(Pcam));
	if (lrad > 0.f) {
		float u, v;
		shirleyDisc(H2.getNext(), H3.getNext(), u, v);
		u *= lrad, v *= lrad;
		point3d_t Pfocus = origin + (fdist / direction.z)*direction;
		origin.x += u, origin.y += v;
		direction = Pfocus - origin;
	}
	direction.normalize();
	return ray_t(cam2wld*pcam, cam2wld*direction);
	*/
	return ray_t();
}

__END_QDRENDER
