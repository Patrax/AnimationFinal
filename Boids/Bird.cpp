//
//  main.cpp
//  Boids
//
//  Created by Patricio Jeri on 2014-04-02.
//  Copyright (c) 2014 Patricio Jeri. All rights reserved.
//

#include "Bird.h"
#include <stdlib.h>     /* srand, rand */




/**
 * Constructor Bird() - make a new Boid object with a given position.
 * The Boid is initialized to have a random velocity.
 * Coloring and behavior changes if you are the predator (predator is bigger and red).
 */
Bird::Bird(float mX, float mY, float mZ, bool iPredator) {
	isPredator = iPredator;
	aDetect = 5.0;
	mDetect = 10.0;
	if (isPredator) {
		//predators are larger than regular birds
		base = 1.0f;
		height = 2.0f;
		maxSpeed = 0.3;
		maxForce = 0.005;
		//a predator has a larger centering range
		cDetect = 30.0;
	} else {
		base = 0.5f;
		height = 1.0f;
		maxSpeed = 0.3;
		maxForce = 0.005;
		cDetect = 15.0;
	}
	position.x = mX;
	position.y = mY;
	position.z = mZ;
	//initialize velocity of all boids to be random
	velocity.x = ((float)rand() / RAND_MAX) - 0.5;
	velocity.y = ((float)rand() / RAND_MAX) - 0.5;
	velocity.z = ((float)rand() / RAND_MAX) - 0.5;
	velocity.normalize();
	accel.multiply(0.0);
	//the orientation of the boid with no rotation is (0, 0, 1)
	oldX = 0.0;
	oldY = 0.0;
	oldZ = 1.0;
	
}

/**
 * Destructor Bird().
 */
Bird::~Bird() {
	
}

/**
 * draw() - draw a bird to the screen as a cone.
 * The orientation of the cone matches the velocity vector of the bird.
 */
void Bird::draw() {
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	//orient the boid based on velocity vector
	float magnitude = velocity.magnitude();
	if (magnitude > 0.0) {
		float xNorm = velocity.x / magnitude;
		float yNorm = velocity.y / magnitude;
		float zNorm = velocity.z / magnitude;
		glRotatef(acos(oldX * xNorm + oldY * yNorm + oldZ * zNorm) * RADIANS2DEGREES, oldY * zNorm - oldZ * yNorm, oldZ * xNorm - oldX * zNorm, oldX * yNorm - oldY * xNorm);
	}
	glutWireCone(base, height, 50, 50);
	glPopMatrix();
}

/**
 * getDistance() - return Euclidian distance to the other Bird.
 **/
float Bird::getDistance(Bird* other) {
	return sqrt(pow(other->position.x - this->position.x, 2) + pow(other->position.y - this->position.y, 2) + pow(other->position.z - this->position.z, 2));
}

/**
 * update() - update the internal logic of the bird.
 * elapsed = number of ms since the last call to this function.
 */
void Bird::update(int elapsed, std::vector<Bird*> &flock, Bird* predator) {
	Vec3 avoidance(0.0, 0.0, 0.0);
	Vec3 matching(0.0, 0.0, 0.0);
	Vec3 centering(0.0, 0.0, 0.0);
	Vec3 obstacles(0.0, 0.0, 0.0);
	Vec3 avoidPredator(0.0, 0.0, 0.0);
	float aCount = 0.0;
	float mCount = 0.0;
	float cCount = 0.0;
	//check for interflock forces
	for(int i=0; i < flock.size(); i++) {
		Bird* b = flock.at(i);
		float distance = this->getDistance(b);
		if (distance < aDetect && distance > 0.0001) {
			//COLLISION AVOIDANCE
			//avoid collisions with nearby flockmates
			aCount += 1.0;
			avoidance.add(position.x - b->position.x, position.y - b->position.y, position.z - b->position.z);
		}
		if (distance < mDetect && distance > 0.0001) {
			mCount += 1.0;
			//VELOCITY MATCHING
			//attempt to match velocity with nearby flockmates
			matching.add(b->velocity);
		}
		if (distance < cDetect && distance > 0.0001) {
			cCount += 1.0;
			//FLOCK CENTERING
			//attempt to stay close to nearby flockmates
			centering.add(b->position);
		}
		
	}
	//avoid the predator if it is around
	if (isPredator == false && predator != NULL) {
		float distance = this->getDistance(predator);
		if (distance < 30.0) {
			avoidPredator.x = position.x - predator->position.x;
			avoidPredator.y = position.y - predator->position.y;
			avoidPredator.z = position.z - predator->position.z;
			avoidPredator.truncate(maxForce);
			//scale for predator avoidance is 3.0
			avoidPredator.multiply(3.0);
			//add force
			accel.add(avoidPredator);
		}
	}
	
	//accelerate based on avoidance with flockmates
	if (aCount > 0 && isPredator == false) {
		avoidance.divide(aCount);
		avoidance.truncate(maxForce);
		//scale for avoidance is 2.0
		avoidance.multiply(2.0);
		//add force
		accel.add(avoidance);
	}
	
	//accelerate based on matching velocity with flockmates
	if (mCount > 0 && isPredator == false) {
		matching.divide(mCount);
		matching.subtract(velocity);
		matching.truncate(maxForce);
		//scale for matching is 1.0
		matching.multiply(1.0);
		//add force
		accel.add(matching);
	}
	
	//accelerate based on centering towards flockmates
	if (cCount > 0) {
		centering.divide(cCount);
		centering.subtract(position);
		centering.truncate(maxForce);
		//scale for centering is 1.0
		centering.multiply(1.0);
		//add force
		accel.add(centering);
	}

	//change velocity based on acceleration vector
	velocity.add(accel);
	
	//don't go past maximum speed
	velocity.truncate(maxSpeed);
	
	//don't go too far beyond the box
	if (position.x > MAX_X) {
		velocity.x -= 0.01;
	}
	if (position.x < MIN_X) {
		velocity.x += 0.01;
	}
	if (position.y > MAX_Y) {
		velocity.y -= 0.01;
	}
	if (position.y < MIN_Y) {
		velocity.y += 0.01;
	}
	if (position.z > MAX_Z) {
		velocity.z -= 0.01;
	}
	if (position.z < MIN_Z) {
		velocity.z += 0.01;
	}
	
	//change position based on velocity vector.
	position.add(velocity);
	
	//reset accelerations to 0 after this loop
	accel.multiply(0.0);
}