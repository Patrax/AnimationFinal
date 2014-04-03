#include "Matrix4x4.h"
#include <string.h>

/**
	default constructor
*/
Matrix4x4::Matrix4x4(){
	loadIdentity();
}

/**
	destructor
*/
Matrix4x4::~Matrix4x4(void){
	//nothing to do here...
}

/**
	copy constructor from another transformation matrix
*/
Matrix4x4::Matrix4x4(const Matrix4x4& other){
	memcpy(this->data, other.data, sizeof(double)*4*4);
}

/**
	copy operator from a transformation matrix
*/
Matrix4x4& Matrix4x4::operator = (const Matrix4x4& other){
	//we already know they have the same dimensions, so we won't check for that to save time
	memcpy(this->data, other.data, sizeof(double)*4*4);
	return *this;
}

/**
	This method populates the data in the matrix with the values that are obtained from openGL (stored in column major order)
*/
void Matrix4x4::setOGLValues(const double* oglValues){
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
			this->data[i*4+j] = oglValues[j*4+i];
}

/**
	This method populates the data in the matrix with the values that are passed in (stored in row major order)
*/
void Matrix4x4::setValues(const double* values){
	memcpy(this->data, values, sizeof(double)*4*4);
}


/**
	This method copies the data stored in the current matrix in the array of doubles provided as input.
*/
void Matrix4x4::getValues(double* values) const{
	memcpy(values, this->data, sizeof(double)*4*4);
}

/**
	This method copies the data stored in the current matrix in the array of doubles provided as input, in column major order.
*/
void Matrix4x4::getOGLValues(double* values)const{
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++)
			values[j*4+i] = data[i*4+j];
}

/**
	This method sets the current matrix to be equal to: a * b
*/
void Matrix4x4::setToProductOf(const Matrix4x4& a, const Matrix4x4& b){
	for (int i=0;i<4;i++)
		for (int j=0;j<4;j++){
			data[i*4+j] = 0;
			for (int k=0;k<4;k++)
				data[i*4+j] += a.data[i*4+k]*b.data[k*4+j];
		}
}

/**
	loads the identity matrix
*/
void Matrix4x4::loadIdentity(){
	memset(data, 0, sizeof(double)*4*4);
	data[0*4+0] = data[1*4+1] = data[2*4+2] = data[3*4+3] = 1;
}

/**
	This method sets the current matrix to a 4x4 transformation matrix that corresponds to a translation
*/
void Matrix4x4::setToTranslationMatrix(const ThreeTuple& coords){
	this->loadIdentity();
	this->setTranslation(Vector3d(coords.x, coords.y, coords.z));
}

/**
	This method sets the current matrix to a 4x4 transformation matrix that corresponds to a rotation of angle radians around the given axis.
	IT IS ASSUMED THAT THE VECTOR PASSED IN IS A UNIT VECTOR!!!
*/
void Matrix4x4::setToRotationMatrix(double angle, Vector3d& axis){
	double Ax = axis.x, Ay = axis.y, Az = axis.z;
	double c = cos(angle);
	double s = sin(angle);
	double rotData[16] = {c + (1-c)*Ax*Ax,		(1-c)*Ax*Ay - s*Az,		(1-c)*Ax*Az + s*Ay,		0,
						(1-c)*Ax*Ay+s*Az,		c + (1-c)*Ay*Ay,		(1-c)*Ay*Az - s*Ax,		0,
						(1-c)*Ax*Az-s*Ay,		(1-c)*Ay*Az+s*Ax,		c+(1-c)*Az*Az,			0,
						0,						0,						0,						1};
	memcpy(data, rotData, sizeof(double)*4*4);
}

/**
	This method returns a point that was transformed by the current matrix. Assume the point's w coordinate is 1!
*/
Point3d Matrix4x4::operator*(const Point3d &p) const{

	double w = data[3*4+3]*1;

	Point3d result(	data[0*4+0]*p.x + data[0*4+1]*p.y + data[0*4+2]*p.z + data[0*4+3]*1,
					data[1*4+0]*p.x + data[1*4+1]*p.y + data[1*4+2]*p.z + data[1*4+3]*1,
					data[2*4+0]*p.x + data[2*4+1]*p.y + data[2*4+2]*p.z + data[2*4+3]*1);

	result.x /= w;
	result.y /= w;
	result.z /= w;

	return result;
}

/**
	This method returns a vector that was transformed by the current matrix. Note that since the w-coordinate of the vector is 0, it will be unnafected
	by the translation part of the transformation matrix.
*/
Vector3d Matrix4x4::operator*(const Vector3d &v) const{
	return Vector3d(data[0*4+0]*v.x + data[0*4+1]*v.y + data[0*4+2]*v.z,
					data[1*4+0]*v.x + data[1*4+1]*v.y + data[1*4+2]*v.z,
					data[2*4+0]*v.x + data[2*4+1]*v.y + data[2*4+2]*v.z);
}


/**
	This method returns the vector that corresponds to the translation associated with the current matrix.
	This vector goes from the origin of the global frame to the origin of the local frame, where this matrix
	corresponds to the transfomation matrix from the local frame into the golbal frame. The vector returned
	is specified in global coordinates.
*/
Vector3d Matrix4x4::getTranslation() const{
	Vector3d result;
	result.x = data[0*4+3];
	result.y = data[1*4+3];
	result.z = data[2*4+3];
	return result;
}

/**
	This method sets the translation part of the transformation matrix to the one that is passed in as a parameter. The translation
	vector is assumed to be represented in global coordinates.
*/
void Matrix4x4::setTranslation(const Vector3d& t){
	data[0*4+3] = t.x;
	data[1*4+3] = t.y;
	data[2*4+3] = t.z;
}

/**
	This method sets the translation part of the transformation matrix to the one that is passed in as a parameter.
*/
void Matrix4x4::setTranslation(const Point3d& t){
	data[0*4+3] = t.x;
	data[1*4+3] = t.y;
	data[2*4+3] = t.z;
}


/**
	This method clears the translation portion of the matrix.
*/
void Matrix4x4::clearTranslation(){
	data[0*4+3] = 0;
	data[1*4+3] = 0;
	data[2*4+3] = 0;
}


/**
	This method sets the current matrix to be equal to its transpose.
*/
void Matrix4x4::setToTranspose(){
	for (int i=0;i<4;i++)
		for (int j=i+1;j<4;j++){
			double temp = data[i*4+j];
			data[i*4+j] = data[j*4+i];
			data[j*4+i] = temp;
		}
}

