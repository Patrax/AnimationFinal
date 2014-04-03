//
//  main.cpp
//  Boids
//
//  Created by Patricio Jeri on 2014-04-02.
//  Copyright (c) 2014 Patricio Jeri. All rights reserved.
//

#ifndef __VEC_3__
#define __VEC_3__

#include <math.h>
/**
 * Vec3:
 * Helper class for 3D Vector arithmetic.
 **/
class Vec3 {
public:
	Vec3();
	Vec3(double x, double y, double z);
	void multiply(double scalar);
	void divide(double scalar);
	void truncate(double scalar);
	void add(Vec3 other);
	void add(double x, double y, double z);
	void subtract(Vec3 other);
	double magnitude();
	void normalize();
	double x, y, z;
};

#endif
