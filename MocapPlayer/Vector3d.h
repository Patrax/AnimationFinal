#ifndef _Vector3d_h_
#define _Vector3d_h_

#include "Point3d.h"
#include <math.h>

//class Matrix4x4;

/*================================================================================================================================================================*
 | This class implements a Vector3d in a three dimensional space. Note that the w-coordinate of a vector expressed in homogenous coordinates is 0.                  |
 *================================================================================================================================================================*/
class Vector3d : public ThreeTuple {
public:
	/**
		A bunch of useful constructors.
	*/
	Vector3d();
	Vector3d(double x, double y, double z);
	Vector3d(double x, double y);
	Vector3d(const Vector3d &other);
	/**
		This vector points from the origin to the point p
	*/
	Vector3d(const Point3d &p);
	/**
		This vector points from p1 to p2.
	*/
	Vector3d(const Point3d &p1, const Point3d &p2);

	/**
		Destructor
	*/
	~Vector3d();


	/**
		a copy operator
	*/
	inline Vector3d& operator = (const Vector3d &other){
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;
		return *this;
	}

	/**
		a fast add function: *this = a+b
	*/
	inline void setToSum(const Vector3d& a, const Vector3d& b){
		this->x = a.x + b.x;
		this->y = a.y + b.y;
		this->z = a.z + b.z;
	}

	/**
		a fast add function: *this = a x b
	*/
	inline void setToCrossProduct(const Vector3d& a, const Vector3d& b){
		this->x = a.y * b.z - a.z * b.y;
		this->y = a.z * b.x - a.x * b.z;
		this->z = a.x * b.y - a.y * b.x;
	}

	/**
		addition of two vectors - results in a new vector.
	*/
	inline Vector3d operator + (const Vector3d &v) const{
		return Vector3d(this->x + v.x, this->y + v.y,this->z + v.z);
	}

	/**
		a fast subtraction function: *this = a - b
	*/
	inline void setToDifference(const Vector3d& a, const Vector3d& b){
		this->x = a.x - b.x;
		this->y = a.y - b.y;
		this->z = a.z - b.z;
	}

	/**
		subtraction of two vectors - results in a new vector.
	*/
	inline Vector3d operator - (const Vector3d &v) const{
		return Vector3d(this->x - v.x, this->y - v.y, this->z - v.z);
	}

	/**
		a fast multiplication by a constant function *this *= n
	*/
	inline void multiplyBy(double n){
		this->x *= n;
		this->y *= n;
		this->z *= n;
	}

	/**
		another fast multiplication by a constant: *this = a * n
	*/
	inline void setToProduct(const Vector3d& a, double n){
		this->x = a.x * n;
		this->y = a.y * n;
		this->z = a.z * n;
	}

	/**
		a fast addition of a vector multiplied by scalar
	*/
	inline void addScaledVector(const Vector3d& a, double s){
		this->x += a.x * s;
		this->y += a.y * s;
		this->z += a.z * s;
	}

	/**
		a fast addition of a vector
	*/
	inline void addVector(const Vector3d& a){
		this->x += a.x;
		this->y += a.y;
		this->z += a.z;
	}

	/**
		computes the vector between the two points
	*/
	inline void setToVectorBetween(const Point3d& a, const Point3d& b){
		this->x = b.x - a.x;
		this->y = b.y - a.y;
		this->z = b.z - a.z;
	}

	/**
		multiplication by a constant - results in a new vector.
	*/
	inline Vector3d operator * (double n) const{
		return Vector3d(n*this->x, n*this->y, n*this->z);
	}
	/**
		dividing by a constant - results in a new vector
	*/
	inline Vector3d operator / (double n) const{
		double m = 1.0/n;
		return Vector3d(this->x*m, this->y*m, this->z*m);
	}
	/**
		add a new vector to the current one.
	*/
	inline Vector3d& operator += (const Vector3d &v){
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return (*this);
	}
	/**
		subtract a new vector to the current one.
	*/
	inline Vector3d& operator -= (const Vector3d &v){
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return (*this);
	}
	/**
		multiply the current vector by a constant
	*/
	inline Vector3d& operator *= (double n){
		this->x *= n;
		this->y *= n;
		this->z *= n;

		return (*this);
	}
	/**
		divide the current vector by a constant.
	*/
	inline Vector3d& operator /= (double n){
		double m = 1.0/n;
		this->x *= m;
		this->y *= m;
		this->z *= m;
		return (*this);
	}

	/**
		Returns a vector that has all its components multiplied by -1.
	*/
	inline Vector3d operator - (){
		return Vector3d(-this->x, -this->y, -this->z);
	}

	/**
		computes the dot product of this vector with v.
	*/
	inline double dotProductWith(const Vector3d &v) const{
		return (this->x * v.x + this->y*v.y + this->z * v.z);
	}

	/**
		computes the cross product of this vector with v (*this x v).
	*/
	inline Vector3d crossProductWith(const Vector3d &v) const{
		/*
			Easiest way to figure it out is to set it up like this:
								___________________
							Ux | Uy   Uz   Ux   Uy | Uz
							Vx | Vy   Vz   Vx   Vy | Vz
								-------------------
			Cross product is given by cross multiplying the items in the box, and subing the other
			diagonal
		*/	
		Vector3d result, u = *this;
		result.x = u.y * v.z - u.z * v.y;
		result.y = u.z * v.x - u.x * v.z;
		result.z = u.x * v.y - u.y * v.x;

		return result;
	}

	/**
		computes the two norm (length) of the current vector
	*/
	inline double length() const{
		return sqrt(x*x+y*y+z*z);
	}

	/**
		This method changes the current vector to a unit vector.
	*/
	Vector3d& toUnit(){
		double d = this->length();
		if (d < 0.00001)
			return *this;
		*this /= d;
		return (*this);
	}

};

#endif

