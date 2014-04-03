#include "Quaternion.h"


/**
	Returns the complex conjugate of the current quaternion.
*/
Quaternion Quaternion::getComplexConjugate() const{
	return Quaternion(s, v * (-1));
}

/**
	Returns the inverse of the current quaternion: q * q^-1 = identity quaternion: s = 1, v = (0,0,0)
*/
Quaternion Quaternion::getInverse() const {
	double length = this->getLength();
	return (this->getComplexConjugate() * (1/(length*length)));
}

/**
	Returns the length of a quaternion.
*/
double Quaternion::getLength() const{
	return sqrt(s*s + v.dotProductWith(v));
}

/**
	Computes the dot product between the current quaternion and the one given as parameter.
*/
double Quaternion::dotProductWith(const Quaternion &other) const{
	return (this->s * other.s + this->v.dotProductWith(other.v));
}

/**
	This method returns a quaternion that is the result of linearly interpolating between the current quaternion and the one provided as a parameter.
	The value of the parameter t indicates the progress: if t = 0, the result will be *this. If it is 1, it will be other. If it is inbetween, then
	the result will be a combination of the two initial quaternions.
	Both quaternions that are used for the interpolation are assumed to have unit length!!!
*/
Quaternion Quaternion::linearlyInterpolateWith(const Quaternion &other, double t) const{
	if (t<0) t = 0;
	if (t>1) t = 1;
	Quaternion result = (*this)*(1-t) + other*t;
	return result * (1/result.getLength());
}


/**
	This method is used to rotate the vector that is passed in as a parameter by the current quaternion (which is assumed to be a 
	unit quaternion).
*/
Vector3d Quaternion::rotate(const Vector3d& u) const{
	//uRot = q * (0, u) * q' = (s, v) * (0, u) * (s, -v)
	//working it out manually, we get:
	Vector3d t = u * s + v.crossProductWith(u);
	return v*u.dotProductWith(v) + t * s + v.crossProductWith(t);
}


/**
	This method is used to rotate the vector that is passed in as a parameter by the current quaternion (which is assumed to be a 
	unit quaternion).
*/
Vector3d Quaternion::inverseRotate(const Vector3d& u) const{
	//uRot = q * (0, u) * q' = (s, -v) * (0, u) * (s, v)
	//working it out manually, we get:
	Vector3d t = u * s + u.crossProductWith(v);
	return v*u.dotProductWith(v) + t * s + t.crossProductWith(v);
}

/**
	This method returns a quaternion that is the result of spherically interpolating between the current quaternion and the one provided as a parameter.
	The value of the parameter t indicates the progress: if t = 0, the result will be *this. If it is 1, it will be other. If it is inbetween, then
	the result will be a combination of the two initial quaternions.
	Both quaternions that are used for the interpolation are assumed to have unit length!!!
*/
Quaternion Quaternion::sphericallyInterpolateWith(const Quaternion &other, double t) const{
	//make sure that we return the same value if either of the quaternions involved is q or -q 
	if (this->dotProductWith(other) < 0){
		Quaternion temp;
		temp.s = -other.s;
		temp.v = other.v * (-1);
		return this->sphericallyInterpolateWith(temp, t);
	}

	if (t<0) t = 0;
	if (t>1) t = 1;
	double dotProduct = this->dotProductWith(other);
	double tmp = 1-dotProduct*dotProduct;
	if (tmp < 0) tmp = 0;
	double sinTheta = sqrt(tmp);
	double theta = acos(dotProduct);
	if (sinTheta == 0)
		return (*this);
	return ((*this) * sin(theta * (1-t)) + other * sin(theta * t)) * (1/sin(theta));
}


/**
	This method will return a quaternion that represents a rotation of angle radians around the axis provided as a parameter.
	IT IS ASSUMED THAT THE VECTOR PASSED IN IS A UNIT VECTOR!!!
*/
Quaternion Quaternion::getRotationQuaternion(double angle, Vector3d &axis){
	Quaternion result(cos(angle/2), axis * sin(angle/2));
	result.toUnit();
	return result;
}

/**
	This method will return a 4x4 matrix that represents an equivalent rotation as the given quaternion.
*/
void Quaternion::getRotationMatrix(Matrix4x4* m) const{
	double w = s, x = v.getX(), y = v.getY(), z = v.getZ();
	double values[16] = {1-2*y*y-2*z*z,	2*x*y - 2*w*z,	2*x*z + 2*w*y,	0, 
						 2*x*y + 2*w*z,	1-2*x*x-2*z*z,	2*y*z - 2*w*x,	0,
						 2*x*z - 2*w*y,	2*y*z + 2*w*x,	1-2*x*x-2*y*y,	0,
								0,			0,				0,			1};
	m->setValues(values);
}

/**
	Returns the result of multiplying the current quaternion by rhs. NOTE: the product of two quaternions represents a rotation as well: q1*q2 represents
	a rotation by q2 followed by a rotation by q1!!!!
*/
Quaternion Quaternion::operator * (const Quaternion &other) const{
	return Quaternion(this->s * other.s - this->v.dotProductWith(other.v), other.v * this->s + this->v * other.s + this->v.crossProductWith(other.v));
}

/**
	this method is used to set the current quaternion to the product of the two quaternions passed in as parameters.
	the bool parameters invA and invB indicate wether or not, the quaternion a or b should be inverted (well, complex conjugate really)
	for the multiplication
*/
void Quaternion::setToProductOf(const Quaternion& a, const Quaternion& b, bool invA, bool invB){
	double multA = (invA==false)?(1):(-1);
	double multB = (invB==false)?(1):(-1);
	this->s = a.s*b.s - a.v.dotProductWith(b.v) * multA * multB;
	this->v.setToCrossProduct(a.v, b.v);
	this->v.multiplyBy(multA * multB);

	this->v.addScaledVector(a.v, b.s*multA);
	this->v.addScaledVector(b.v, a.s*multB);
}


/**
	This operator multiplies the current quaternion by the rhs one. Keep in mind the note RE quaternion multiplication.
*/
Quaternion& Quaternion::operator *= (const Quaternion &other){
	double newS = this->s * other.s - this->v.dotProductWith(other.v);
	Vector3d newV = other.v * this->s + this->v * other.s + this->v.crossProductWith(other.v);
	this->s = newS;
	this->v = newV;
	return *this;
}

/**
	This method multiplies the current quaternion by a scalar.
*/
Quaternion& Quaternion::operator *= (double scalar){
	this->s *= scalar;
	this->v *= scalar;
	return *this;
}


/**
	This method returns a copy of the current quaternion multiplied by a scalar.
*/
Quaternion Quaternion::operator * (double scalar) const{
	return Quaternion(s * scalar, v * scalar);
}


/**
	This method returns a quaternion that was the result of adding the quaternion rhs to the current quaternion.
*/
Quaternion Quaternion::operator + (const Quaternion &other) const{
	return Quaternion(s + other.s, v + other.v);
}


/**
	This method adds the rhs quaternion to the current one.
*/
Quaternion& Quaternion::operator += (const Quaternion &other){
	this->s += other.s;
	this->v += other.v;
	return *this;
}


/**
	This method transforms the current quaternion to a unit quaternion.
*/
Quaternion& Quaternion::toUnit(){
	*this *= (1/this->getLength());
	return *this;
}


