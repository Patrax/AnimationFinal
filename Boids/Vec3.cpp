//
//  main.cpp
//  Boids
//
//  Created by Patricio Jeri on 2014-04-02.
//  Copyright (c) 2014 Patricio Jeri. All rights reserved.
//

#include "Vec3.h"

/**
 * Constructor - x,y,z initialized to 0.
 **/
Vec3::Vec3() {
	x = 0.0;
	y = 0.0;
	z = 0.0;
}

/**
 * Construcotr - x, y, z initialized to passed values.
 **/
Vec3::Vec3(double mX, double mY, double mZ) {
	x = mX;
	y = mY;
	z = mZ;
}


/**
 * multiply() - scale by the given factor.
 **/
void Vec3::multiply(double scalar) {
    x *= scalar;
	y *= scalar;
	z *= scalar;
}

/**
 * divide() - divide each component by scalar.
 **/
void Vec3::divide(double scalar) {
	x /= scalar;
	y /= scalar;
	z /= scalar;
}

/**
 * magnitude() - return the norm length of vector.
 **/
double Vec3::magnitude() {
	return sqrt(x * x + y * y + z * z);
}

/**
 * truncate() - limit the magnitude of this vector to be scalar
 **/
void Vec3::truncate(double scalar) {
	float myMag = this->magnitude();
	if (myMag > scalar) {
	    x *= scalar / myMag;
		y *= scalar / myMag;
		z *= scalar / myMag;
	}
}


/**
 * normalize() - set the magnitude to be 1.0.
 **/
void Vec3::normalize() {
	x /= this->magnitude();
	y /= this->magnitude();
	z /= this->magnitude();
}

/**
 * add() - add x,y,z componentwise.
 **/
void Vec3::add(double oX, double oY, double oZ) {
    x += oX;
	y += oY;
	z += oZ;
}

/**
 * add() - add another vector.
 **/
void Vec3::add(Vec3 other) {
	x += other.x;
	y += other.y;
	z += other.z;
}

/**
 * subtract() - subtract another vector from this one.
 **/
void Vec3::subtract(Vec3 other) {
    x -= other.x;
	y -= other.y;
	z -= other.z;
}



