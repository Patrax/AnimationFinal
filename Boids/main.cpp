//
//  main.cpp
//  Boids
//
//  Created by Patricio Jeri on 2014-04-02.
//  Copyright (c) 2014 Patricio Jeri. All rights reserved.
//

#include <GLUT/glut.h>
#include <vector>
#include <time.h>
#include <stdlib.h>     /* srand, rand */
#include "Bird.h"

#define XRES 800
#define YRES 800

#define NUM_BIRDS 100

int lastFrameTime = 0;

float cameraX = 0.0;
float cameraY = 0.0;
float cameraZ = -100.0;

int mouseX = 0;
int mouseY = 0;

float xAngle = 0.0;
float yAngle = 0.0;

std::vector<Bird*> birds;

Bird* predator = NULL;
bool killPredator = false;

bool movingForward = false;
bool movingBackward = false;

/**
 * display() - update the internal logic for the scene and draw it.
 */
void display() {
	
	if (lastFrameTime == 0) {
		lastFrameTime = glutGet(GLUT_ELAPSED_TIME);
	}
	int now = glutGet(GLUT_ELAPSED_TIME);
	int elapsedMS = now - lastFrameTime;
	lastFrameTime = now;
	//elapsedMS = delta of MS since last frame
	
	//handle zoom in and out
	if (movingForward) {
		cameraZ += 1;
	} else if (movingBackward) {
		cameraZ -= 1;
	}
	
	//clear
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//draw here
    
	//move camera back 100 units
	glLoadIdentity();
	glTranslatef(cameraX, cameraY, cameraZ);
	glRotatef(yAngle, 1.0, 0, 0);
	glRotatef(xAngle, 0, 1.0, 0.0);
    
//    //draw ground plane
//    glColor3f(0.5,0.5,0.9);
//    glNormal3f(0,1,0);
//    glBegin(GL_POLYGON);
//    {
//        glVertex3d(-1000, -MAX_Y-2, -1000);
//        glVertex3d(-1000, -MAX_Y-2, 1000);
//        glVertex3d(1000, -MAX_Y-2, 1000);
//        glVertex3d(1000, -MAX_Y-2, -1000);
//    }
//    glEnd();
    
    // Draw a floor. Since it is transparent, we need to do it AFTER all of
    // the opaque objects.
        for (int x = -100; x < 100; x+=10) {
            for (int z = -100; z < 100; z+=10) {
                glColor4f(1, 1, 1, (x + z) % 20 ? 0.75 : 0.5);
                glNormal3f(0, 1, 0);
                glBegin(GL_POLYGON);
                glVertex3d(x, -MAX_Y-10, z);
                glVertex3d(x+10, -MAX_Y-10, z);
                glVertex3d(x+10, -MAX_Y-10, z+10);
                glVertex3d(x, -MAX_Y-10, z+10);
                glEnd();
            }
        }

    
	glColor3f(1.0f, 1.0f, 1.0f);
    
	//run the logic and draw routines for each bird
	//regular birds
	for(unsigned int i=0; i < birds.size(); i++) {
		birds.at(i)->update(elapsedMS, birds, predator);
		birds.at(i)->draw();
	}
	glColor3f(1.0f, 0.0f, 0.0f);
	//predator
	if (predator != NULL) {
		predator->update(elapsedMS, birds, predator);
		predator->draw();
	}
	//end draw
	glutSwapBuffers();
	//only delete predator at the end of the update loop
	if (killPredator) {
		delete predator;
		predator = NULL;
		killPredator = false;
	}
}

/**
 * reshape() - adjust the window to a new width and height
 */
void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, width / height, 0.1, 300);
	glMatrixMode(GL_MODELVIEW);
}

/**
 * idle() - when not doing anything, just do a redraw.
 */
void idle() {
	glutPostRedisplay();
}

/**
 * handleKey() - respond to keyboard input
 * w = move camera along positive z axis
 * s = move camera along negative z axis
 * p = spawn predator or register predator to be deleted
 **/
void handleKey(unsigned char key, int x, int y) {
	if (key == 'w') {
		movingForward = true;
		movingBackward = false;
	} else if (key == 's') {
		movingForward = false;
		movingBackward = true;
	}
	if (key == 'p') {
		if (predator == NULL) {
			predator = new Bird((rand() % (MAX_X * 2)) - MAX_X, (rand() % (MAX_Y * 2)) - MAX_Y, (rand() % (MAX_Z * 2)) - MAX_Z, true);
		} else {
			killPredator = true;
		}
	}
}

/**
 * handleKeyReleased() - respond to when the user stops holding a key.
 * w released = stop moving foward along z axis
 * s released = stop moving backward along z axis
 **/
void handleKeyReleased(unsigned char key, int x, int y) {
    if (key == 'w') {
		movingForward = false;
	} else if (key == 's') {
		movingBackward = false;
	}
}

/**
 * handleMouseMove() - rotate camera based on change in mouse coords
 **/
void handleMouseMove(int x, int y) {
	int diffX = x - mouseX;
	int diffY = y - mouseY;
	mouseX = x;
	mouseY = y;
	xAngle += diffX;
	yAngle += diffY;
}

/**
 * handleMouse() - respond to mouse button presses.
 * Currently does nothing.
 **/
void handleMouse(int button, int state, int x, int y) {
	
}

/**
 * main() - start the program.  Setup the display variables.  Setup the simulation.  Kick off the main loop.
 */
int main (int argc, char* argv[]) {
	//initialize opengl
    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(XRES, YRES);
	glutInitWindowPosition(100, 0);
	glutCreateWindow("Boids Demo");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutKeyboardFunc(handleKey);
	glutKeyboardUpFunc(handleKeyReleased);
	glutMouseFunc(handleMouse);
	glutPassiveMotionFunc(handleMouseMove);
	glEnable(GL_DEPTH_TEST);
    
    // Set clearing values.
    glClearColor(0, 0, 0, 1);
    glClearDepth(1);
    
    // Turn on depth testing.
    glEnable(GL_DEPTH_TEST);
    
    // Turn on blending (for floor).
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Turn on normal normalization (for non-uniform scaling of primitives).
    glEnable(GL_NORMALIZE);

    
	//seed the rand function
	srand(time(NULL));
    
	//add the birds randomly to box
	for( int i=0; i < NUM_BIRDS; i++) {
		float x = rand() % (MAX_X * 2);
		x -= MAX_X;
		float y = rand() % (MAX_Y * 2);
		y += MAX_Y;
		float z = rand() % (MAX_Z * 2);
		z -= MAX_Z;
		birds.push_back(new Bird(x, y, z, false));
	}
	
	
	//kick off the main loop
	glutMainLoop();
	
	//clean up after we are done
	birds.clear();
	if (predator != NULL) {
		delete predator;
	}
	return EXIT_SUCCESS;
}
