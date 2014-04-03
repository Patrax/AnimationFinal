#ifndef _GLCamera_h_
#define _GLCamera_h_

#include "Vector3d.h"
#include "Quaternion.h"
#include "Point3d.h"
#include "Matrix4x4.h"

// simple camera model to follow a target and rotate around it

class GLCamera{
private:
	Quaternion orientation;    // current orientation of camera
public:
	GLCamera(void);
	~GLCamera(void);

	void applyCameraTransformations();
	Vector3d rotations;
	double camDistance;     // distance, assuming looking down -z axis of camera frame
	Point3d target;         // look-at point (in world coords)
};

#endif
