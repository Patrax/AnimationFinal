//
//  main.cpp
//  Boids
//
//  Created by Patricio Jeri on 2014-04-02.
//  Copyright (c) 2014 Patricio Jeri. All rights reserved.
//


#ifndef __BIRD_H__
#define __BIRD_H__

#include <vector>
#include <math.h>

#include <GLUT/glut.h>

#include "Vec3.h"

#include "TriangleMesh.h"

//the rough bounding box for the sim
//not exact, as birds will only start turning at these points
#define MAX_X 40
#define MIN_X -40
#define	MAX_Y 40
#define MIN_Y -40
#define MAX_Z 40
#define MIN_Z -40

#define RADIANS2DEGREES 57.2957795

/**
 * Class Bird:
 * Cone that flies around in space according to the Reynolds SIGGRAPH paper.
 * Has an X, Y, and Z position.  Has a yaw, pitch, and roll orientation.
 * The orientation depends on the boid's velocity vector.
 */
class Bird {
public:
	Bird(float mX, float mY, float mZ, bool isPredator);
	~Bird();
	void draw();
    void drawPredator(TriangleMesh mesh);
	void update(int elapsed, std::vector<Bird*> &flock, Bird* predator);
	float getDistance(Bird* other);
private:
	float maxSpeed;
	float maxForce;
	float oldX, oldY, oldZ;
	Vec3 position;
	Vec3 velocity;
	Vec3 accel;
	bool isPredator;
	float base;
	float height;
	float aDetect;
	float mDetect;
	float cDetect;
};
#endif