//
//  main.cpp
//  Boids
//
//  Created by Patricio Jeri on 2014-04-02.
//  Copyright (c) 2014 Patricio Jeri. All rights reserved.
//

#include "defs.h"
#include "TriangleMesh.h"
#include "FreeFormDeformation.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>

#include <GLUT/glut.h>
#include <vector>
#include <time.h>
#include <stdlib.h>     /* srand, rand */
#include "Bird.h"

#define XRES 800
#define YRES 800

#define NUM_BIRDS 100

int lastFrameTime = 0;

int perspective = 0;

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

int option = 1;
// Rendering parameters
FFDControlPoints::DrawStyle latticeDrawStyle = FFDControlPoints::Lattice;
TriangleMesh::MeshDrawStyle meshDrawStyle = TriangleMesh::SOLID;

// Animation parameters
bool paused = false;   // is the animation paused?
float currentTime = 0; // current time of the animation (seconds)
float maxTime = 2.0;   // maximum time (seconds). 'time' loops from 0 to maxTime
float deltaTime = 0.0; // elapsed time since last update (seconds)
int elapsedTime = 0;   // elapsed time since program started (milliseconds)
float dtScale = 1.0;   // scaling factor for timesteps in physical simulation

// Default scene file  and   scenario
string sceneFile("dragon.txt");
int mode = 0;

//
TriangleMesh meshCopy;
TriangleMesh mesh;
Affine3 bindFrame;
FFDControlPoints bindControlPoints;
FFDControlPoints tempControlPoints;
FFDControlPoints offsetControlPoints;
FFDControlPoints deformedControlPoints;
vector<FFDControlPoints> keyframeControlPoints;

extern void initScene();
extern void updateScene();
extern void loadScene();
extern void computeFFDCoords(TriangleMesh &mesh, Affine3 &frame);
extern void computeDeformedMesh();

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
//@               TO IMPLEMENT               @//
//        Add your global variables here
FFDCoords controlPoints[100000];
//@                                          @//
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//

///////////////////////////////////////////////////////////////////
// FUNC:  init()
// DOES:  initializes default light source and surface material
///////////////////////////////////////////////////////////////////

void init(void)
{
    glClearColor (0.8f, 0.8f, 0.8f, 1.0);
    glShadeModel (GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
    
    initScene();
    updateScene();
}

///////////////////////////////////////////////////////////////////
// FUNC: initScene()
// DOES: initialize the scene
///////////////////////////////////////////////////////////////////

void initScene()
{
    currentTime = 0;     // reset time
    
    loadScene();         // read scene description:  mesh, binding frame, target keyframe
    computeFFDCoords(mesh, bindFrame);  // compute s,t,u coords for each vertex
    
    meshCopy = mesh;
    
    switch(mode) {   // mode-specific initialization
        case 0:
            bindControlPoints = FFDControlPoints(bindFrame);  // initial control point locations
            break;
    }
}

///////////////////////////////////////////////////////////////////
// FUNC: computeFFDCoords()
// DOES: update the location of all objects/vertices in the scene, as a function of Time
///////////////////////////////////////////////////////////////////

void computeFFDCoords(TriangleMesh &mesh, Affine3 &bindFrame)
{
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
    //@               TO IMPLEMENT               @//
    //     Compute the FFD vertex coordinates
    
    Vector3 P0 = bindFrame.getP0();
    Vector3 S = bindFrame.getS();
    Vector3 U = bindFrame.getU();
    Vector3 T = bindFrame.getT();
    
    for (int i=0; i<mesh.vertices.size(); i++) {
        Vector3 P = mesh.vertices[i];
        float s = (P-P0).dot(S)/(S.length()*S.length());
        float t = (P-P0).dot(T)/(T.length()*T.length());
        float u = (P-P0).dot(U)/(U.length()*U.length());
        controlPoints[i].s = s;
        controlPoints[i].t = t;
        controlPoints[i].u = u;
    }
    
    
    //@                                          @//
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}

///////////////////////////////////////////////////////////////////
// FUNC: computeDeformedMesh()
// DOES: compute new location of all vertices based on the current deformed lattice
///////////////////////////////////////////////////////////////////

void computeDeformedMesh()
{
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
    //@               TO IMPLEMENT               @//
    //    Deform the mesh according using the
    //    pre-computed FFD coordinates and the
    //    control points 'deformedControlPoints'
    
    for (int q=0; q<mesh.vertices.size(); q++) {
        Vector3 Pstu = Vector3(0, 0, 0);
        float s = controlPoints[q].s;
        float t = controlPoints[q].t;
        float u = controlPoints[q].u;
        

            for (int i=0; i<3; i++)
                for (int j=0; j<3; j++)
                    for (int k=0; k<3; k++){
                        Pstu += ( nChoosek(2, i) * pow( (1-s), (2-i) ) * pow(s,i) ) *
                        ( nChoosek(2, j) * pow( (1-t), (2-j) ) * pow(t,j) ) *
                        ( nChoosek(2, k) * pow( (1-u), (2-k) ) * pow(u,k) ) *
                        deformedControlPoints(i,j,k);
                    }
            mesh.vertices[q] = Pstu;
        
    }
    
    //@                                          @//
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@//
}

float localTime(float t){
    if (t < 1) {
        return t;
    }else{
        return fmodf(t, (int)t);
    }
}

///////////////////////////////////////////////////////////////////
// FUNC: updateScene()
// DOES: update the location of all objects/vertices in the scene, as a function of Time
///////////////////////////////////////////////////////////////////

void updateScene()
{
    double u;
    Affine3 currentBindFrame;
    Vector3 orig;
    

    u = currentTime < 0.5*maxTime ? 2*currentTime/maxTime : 2*(maxTime-currentTime)/maxTime;
    deformedControlPoints = linearInterpolation(u, keyframeControlPoints[0], keyframeControlPoints[1]);
    //computeDeformedMesh();     // now compute the deformed mesh
}

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
    
    // Draw a floor
    glColor4f(1, 1, 1, 0.5);
    glNormal3f(0, 1, 0);
    glBegin(GL_POLYGON);
    glVertex3d(-100, -MAX_Y-10, -100);
    glVertex3d(-100, -MAX_Y-10, 100);
    glVertex3d(100, -MAX_Y-10, 100);
    glVertex3d(100, -MAX_Y-10, -100);
    glEnd();

    
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
		predator->drawPredator(mesh);
        
        // Draw mesh
        glColor3f(0.7,0.5,0.1);   // brownish color
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
	int newElapsedTime = glutGet(GLUT_ELAPSED_TIME);  // Compute new time and update elapsed time
    deltaTime = 0.0002*dtScale*(newElapsedTime - elapsedTime);
    elapsedTime = newElapsedTime;
    currentTime += deltaTime;
    if (currentTime>=maxTime)   currentTime = 0.0;
    updateScene();         // update scene
    glutPostRedisplay();   // draw scene

}

/**
 * handleKey() - respond to keyboard input
 * w = move camera along positive z axis
 * s = move camera along negative z axis
 * p = spawn predator or register predator to be deleted
 **/
void handleKey(unsigned char key, int x, int y) {
	
    if( (key >= '0') && (key <= '9') ) {
        mode = (int) key - (int) '0';
        initScene();
        updateScene();
    }
    
    if (key == 'w') {
		movingForward = true;
		movingBackward = false;
	} else if (key == 's') {
		movingForward = false;
		movingBackward = true;
	}
    
    if (key == 'o') {
        perspective == 0 ? perspective = 1 : perspective = 0;
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
    init();
    
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

///////////////////////////////////////////////////////////////////
// FUNC: getParsedLine()
// DOES: reads in a line and parses it into words;  also discards empty lines and comment lines
///////////////////////////////////////////////////////////////////

int getParsedLine(ifstream &file, vector<string>  &words)
{
    using namespace std;
    string line;
    
    words.clear();
    while(getline(file,line)) {               // for each line
        // Split the line into words
        string buf;
        stringstream ss(line);
        while (ss >> buf)
            words.push_back(buf);
        if(words.empty())
            continue;
        if (words[0][0] == '#') {        // comment lines:  discard
            words.clear();
            continue;
        }
        break;
    }
    return words.size();
    //  cout << words.size() << " words read." << endl;
}

bool absFrame = false;
bool keyframeStart = false;
extern void kfEnd();

///////////////////////////////////////////////////////////////////
// FUNC: kfStart(words)
// DOES: start of a keyframe
///////////////////////////////////////////////////////////////////

void kfStart(vector<string> &words) {
    Affine3 tempFrame;
    
    if (keyframeStart)     // end the previous keyframe specification if needed
        kfEnd();
    absFrame = false;
    keyframeStart = true;
    offsetControlPoints.zero();
    if (words.size()==1) {           // no affine frame? Assume that absolute x,y,z control points will follow
        absFrame = true;
        cout << "abs frame" << endl;
        return;
    }
    double val[12];                  //  read in frame info
    for (int n=0; n<12; n++)
        val[n] = atof(words[n+1].c_str());   // read in 12 numbers
    tempFrame = Affine3(val);              // remember the new frame
    tempControlPoints = FFDControlPoints(tempFrame);
}

///////////////////////////////////////////////////////////////////
// FUNC: kfControlPoint()
// DOES: keyframe control point
///////////////////////////////////////////////////////////////////

void kfControlPoint(vector<string> &words) {
    int i,j,k;
    sscanf(words[0].c_str(),"(%d,%d,%d):",&i,&j,&k);
    offsetControlPoints(i,j,k)[0] = atof(words[1].c_str());
    offsetControlPoints(i,j,k)[1] = atof(words[2].c_str());
    offsetControlPoints(i,j,k)[2] = atof(words[3].c_str());
}

///////////////////////////////////////////////////////////////////
// FUNC: kfEnd()
// DOES: end of a keyframe
///////////////////////////////////////////////////////////////////

void kfEnd()  {
    FFDControlPoints finalControlPoints;
    
    if (keyframeStart==false) return;         // no keyframe to end?
    if (absFrame) {                     // absolute x,y,z values given?
        finalControlPoints = offsetControlPoints;
    } else {                            // frame + relative offsets
        finalControlPoints = FFDadd(tempControlPoints, offsetControlPoints);
    }
    keyframeControlPoints.push_back(finalControlPoints);
    //  finalControlPoints.print("keyframe control points");
    keyframeStart = false;
}

///////////////////////////////////////////////////////////////////
// FUNC: loadScene()
// DOES: reads in scene as specifed by a text file
///////////////////////////////////////////////////////////////////

void loadScene()
{
    using namespace std;
    string line;
    int kfCount = 0;
    vector<string> words;
    
    absFrame = false;
    keyframeStart = false;
    ifstream myfile(sceneFile.c_str());          // open scene file
    if (!myfile.is_open()) {
        cerr << "Unable open scene: " << sceneFile << endl;
        return;
    }
    keyframeControlPoints.clear();            // reset all keyframes
    while(getParsedLine(myfile,words)) {      // for each line (non-blank, non-comment)
        string lineType = words[0];               // look at first word to determine line type
        if (lineType == "mesh") {                 // mesh to load
            mesh.readFromOBJ(words[1].c_str());
            if (words.size()==3) {             // optional size paramater for mesh
                float newsize = atof(words[2].c_str());
                mesh.normalize(newsize);
            }
            cout << "Read in mesh: " << words[1] << endl;
        }
        if (lineType == "bindframe") {    // bindframe
            double val[12];
            for (int n=0; n<12; n++)
                val[n] = atof(words[n+1].c_str());   // read in 12 numbers
            bindFrame = Affine3(val);     // remember the bindframe
        }
        if (lineType == "keyframe")    kfStart(words);         // start of a keyframe
        if (lineType[0] == '(')        kfControlPoint(words);  // optional keyframe individual control points
        if (lineType == "endkeyframe") kfEnd();                // end of a keyframe
        if (lineType == "dtscale") dtScale = atof(words[1].c_str());                // end of a keyframe
    }
    kfEnd();         // end any ongoing keyframe specifications
    cout << "Read " << keyframeControlPoints.size() << " keyframes" << endl;
}