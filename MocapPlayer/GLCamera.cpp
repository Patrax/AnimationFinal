#include "GLCamera.h"
#include "GLHeaders.h"

GLCamera::GLCamera(void){
	//initialize to the identity quaternion
	orientation = Quaternion(1, 0, 0, 0);
	camDistance = -4;
	target = Point3d(0,0,0);
	rotations = Vector3d(0,0,0);
}

GLCamera::~GLCamera(void){
}


//this method is used to apply the transofmations 
void GLCamera::applyCameraTransformations(){
	//build the orientation. We will construct it by rotating about the z axis, y axis first, and the x

   Vector3d x(1,0,0);
   Vector3d y(0,1,0);
   Vector3d z(0,0,1);
   
   
   
   orientation = Quaternion::getRotationQuaternion(rotations.z, z)
				* Quaternion::getRotationQuaternion(rotations.y, y)
				* Quaternion::getRotationQuaternion(rotations.x, x);
	//we know we are looking down the z-axis of the camera coordinate frame. So we need to compute what that axis is in world coordinates.
	Vector3d zW = orientation.rotate(Vector3d(0,0,-1));
	//this is the camera's up vector, expressed in world coordinates 
	Vector3d yW = orientation.rotate(Vector3d(0,1,0));
	//we know where we are looking, so we can compute the position of the camera in world coordinates.
	Point3d camPos = target + zW * camDistance;
	//now we have all the information we need: the camera coordinate frame expressed in world coordinates. Need only invert it...
	gluLookAt(camPos.x, camPos.y, camPos.z, target.x, target.y, target.z, yW.x, yW.y, yW.z);
}

