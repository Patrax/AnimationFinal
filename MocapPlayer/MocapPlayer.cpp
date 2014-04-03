#include "MocapPlayer.h"
#include <stdlib.h>
#include <stdio.h>
#include "GLHeaders.h"
#include "ArticulatedFigure.h"
#include "GLCamera.h"
#include "GLTexture.h"

  //  global variables
ArticulatedFigure* af;
int mouseButtonPressed;
int oldMouseX = 0;
int oldMouseY = 0;
bool Animate = true;
GLCamera camera;                             // camera parameters
GLTexture* groundTexture;                    // ground texture
char PlayString[100];                        // define a playlist
char DisplayText[100];                       // text to display for the current frame
int CurrPlayClip=0;                          // current PlayClip entry being played
int CurrFrame=0;                             // current frame being played

//////////////////////////////////////////////////////////////////////////////
// clip constructor
//////////////////////////////////////////////////////////////////////////////

clip::clip(int f1p, int f2p, char cnamep, char *namep) {
	f1 = f1p;
	f2 = f2p;
	if (f2 > af->frameCount) 
		f2 = af->frameCount-1;
	cname = cnamep;
	strcpy(name,namep);
}

//////////////////////////////////////////////////////////////////////////////
// playclip constructor
//////////////////////////////////////////////////////////////////////////////

playclip::playclip(int f1ap, int f2ap, int f1rp, int f2rp, int fsp, clip *mclipp) {
	f1a = f1ap;
	f2a = f2ap;
	f1r = f1rp;
	f2r = f2rp;
	fs = fsp;
	mclip = mclipp;
	printf("creating playclip:  %c %4d %4d\n",mclipp->cname,f1a,f2a);
}

//////////////////////////////////////////////////////////////////////////////
// mouse button press callback
//////////////////////////////////////////////////////////////////////////////

void processMouse(int button, int state, int x, int y){
	mouseButtonPressed = button;
	oldMouseX = x;
	oldMouseY = y;
}

//////////////////////////////////////////////////////////////////////////////
// mouse motion callback
//////////////////////////////////////////////////////////////////////////////

void processMouseActiveMotion(int x, int y){
	Vector3d v;
	switch (mouseButtonPressed){
		case GLUT_RIGHT_BUTTON:
					v.x = (oldMouseX - x)/200.0;
					v.y = -(oldMouseY - y)/200.0;
					v.z = 0;
					camera.target.x += v.x;
					camera.target.y += v.y;
					camera.target.z += v.z;
					break;
		case GLUT_LEFT_BUTTON:
					camera.rotations -= Vector3d((-oldMouseY+y)/100.0, (-oldMouseX+x)/100.0, 0);
					break;
		case GLUT_MIDDLE_BUTTON:
					camera.camDistance *= 1-((oldMouseY - y)/200.0);
					if (camera.camDistance > -0.1)
						camera.camDistance = -0.1;
					break;
	}
	oldMouseX = x;
	oldMouseY = y;
}

void processKeyboard(unsigned char key, int x, int y){
	extern void playlist();
	int lastFrame = PlayList[PlayList.size()-1]->f2r - 1;

		//process keyboard events here...
	switch(key) {
		case ' ':
			Animate = !Animate;
			break;
		case '<':
			CurrFrame -= 100;
			if (CurrFrame<0)
				CurrFrame = 0;
			playlist();
			glutPostRedisplay();
			break;
		case '>':
			CurrFrame += 100;
			if (CurrFrame>lastFrame)
				CurrFrame = lastFrame;
			playlist();
			glutPostRedisplay();
			break;
		default:
			break;
	}
}

void drawScene(){
	//draw the articulated figure...
	glPushMatrix();
//	af->draw(0);
	af->drawR(0);
	glPopMatrix();
}

void drawGround(){
	//draw the ground - texture mapped rect
	double x, z, size = 30;
	double a = 0, b = 1, c = 0, d = 0;
	glColor4d(1,1,1,1);

	glPushMatrix();
	glScaled(20,20,20);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	groundTexture->activate();
	glBegin(GL_QUADS);
		x = size; z = size;
		glTexCoord2d(x/2, z/2);
		glVertex3d(x, (-x*a-z*c-d)/b,z);
		z = -size;
		glTexCoord2d(x/2, z/2);
		glVertex3d(x, (-x*a-z*c-d)/b,z);
		x = -size; z = -size;
		glTexCoord2d(x/2, z/2);
		glVertex3d(x, (-x*a-z*c-d)/b,z);
		z = size;
		glTexCoord2d(x/2, z/2);
		glVertex3d(x, (-x*a-z*c-d)/b,z);
	glEnd();
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

//////////////////////////////////////////////////////////////////////////////
// display call back
//////////////////////////////////////////////////////////////////////////////

void appRender(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);   // clear screen
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glColor4f(0.0, 0.0, 0.0, 1.0);
	drawText(0.2,0.9,DisplayText);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	  // have camera track the character
	camera.target.x = af->Cx;
	camera.target.z = af->Cz;
	camera.applyCameraTransformations();
	drawScene();
	drawGround();

	  // draw shadows
	Vector3d n(0,1,0);    	  // assume y-up, and that (0,0,0) is on the ground plane
	Vector3d dir = Vector3d(-150,200,200);      // light direction

	  // mark where the ground is by drawing a 1 in the stencil buffer.
	/*
	glClear(GL_STENCIL_BUFFER_BIT);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	drawGround();
	*/
	  // now render using the shadow projection matrix, only drawing 
	  // where there is a '1' in the stencil buffer
	//    glStencilFunc(GL_LESS, 0, 0xffffffff);
	//    glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	//    glPolygonOffset(-2.0f, -2.0f);
	//    glEnable(GL_POLYGON_OFFSET_FILL);
	//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//    glColor4f(0.0, 0.0, 0.0, 0.5);
    glColor3f(0.0, 0.0, 0.0);
	glDisable(GL_LIGHTING);
    glPushMatrix();
	double dot = n.dotProductWith(dir);
	double mat[16] = {        // setup projection matrix
		dot - n.x*dir.x,		-n.x * dir.y,			-n.x * dir.z,			0,
		-n.y * dir.x,			dot - n.y * dir.y,	-n.y * dir.z,			0,
		-n.z * dir.x,			- n.z * dir.y,		dot - n.z * dir.z,	0,
			0,				     0,					0,			   dot
	};
    glMultMatrixd(mat);
	drawScene();
	glPopMatrix();
	glEnable(GL_LIGHTING);
	//    glDisable(GL_POLYGON_OFFSET_FILL);
	//    glDisable(GL_STENCIL_TEST);
	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////
// playlist playback
//////////////////////////////////////////////////////////////////////////////

void playlist() {
	int hbi = 5;       // half blend interval
	double w;

	int nblocks = int(PlayList.size());
	if (CurrPlayClip>=nblocks) return;
	while (CurrFrame>=PlayList[CurrPlayClip]->f2r) {    // advance into next block in playlist ?
		CurrPlayClip++;
		if (CurrPlayClip>=nblocks) {      // at end ?  reset play loop
			CurrPlayClip = 0;               
			CurrFrame = 0;
			break;
		}
	}
	int baseFrame = PlayList[CurrPlayClip]->f1a;
	int offset = CurrFrame - PlayList[CurrPlayClip]->f1r;
	int remaining = PlayList[CurrPlayClip]->f2r - CurrFrame;
	int frame = baseFrame + offset;
	int b2 = baseFrame + PlayList[CurrPlayClip]->fs - hbi;      // frame at which end blending starts
	int b1 = baseFrame + hbi - 1;               // frame at which beginning blending ends

	int frame2;
	w = -1.0;
	if (frame<=b1 && CurrPlayClip>0) {                 // blend at beginning?
		w = 1.0 - (b1-frame)/(2.0*hbi-1.0);
		frame2 = PlayList[CurrPlayClip-1]->f2a + offset + 1;
//			printf("play2 f=%4d CurrPlayClip=%2d frame=%4d frame2=%4d w=%5.2lf\n",f,CurrPlayClip,frame,frame2,w);
		af->playBack2(frame,frame2,w);
	} else if (frame>=b2 && CurrPlayClip<nblocks-1) {           // blend at end?
		w = 1.0 - (frame-b2)/(2.0*hbi-1.0);
		frame2 = PlayList[CurrPlayClip+1]->f1a - remaining;
//			printf("play2 f=%4d CurrPlayClip=%2d frame=%4d frame2=%4d w=%5.2lf\n",f,CurrPlayClip,frame,frame2,w);
		af->playBack2(frame,frame2,w);
	} else {
//			printf("play  f=%4d CurrPlayClip=%2d frame=%4d frame2=xxxx w=%5.2lf\n", f, CurrPlayClip, frame,w);
		af->playBack(frame);
	}
	clip *pc = PlayList[CurrPlayClip]->mclip;
	sprintf(DisplayText,"%c %20s %4d", pc->cname, pc->name, frame);
	CurrFrame+=2;
}

//////////////////////////////////////////////////////////////////////////////
// idle callback;   does the setup for the next frame to be drawn
//////////////////////////////////////////////////////////////////////////////

void appProcess() {
	static bool init = true;

	if (!Animate) return;
	if (init) {        // first time initialization
		af->Cx = af->Cz = af->Cth = 0;
		init = false;
	}
	
//	af->playBack(CurrFrame);   // setup for current pose
	playlist();
	glutPostRedisplay();           // force redraw
}

//////////////////////////////////////////////////////////////////////////////
// OpenGL initialization stuff
//////////////////////////////////////////////////////////////////////////////

void GLInit(double w, double h){
	//setup the viewport and the perspective transformation matrix
    glViewport(0, 0, (int)w, (int)h);
	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix
	gluPerspective(45.0, (double)w/h,0.1,1500.0);
	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix

	// set the colour of the background, as well as other shading options
	glClearColor(1.0, 1.0, 1.0, 0);
//	glClearColor(0.0, 0.0, 0.0, 0);
	glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_BLEND);
	//turn on antialiasing
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glBlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA);

	//now set up lights...
    GLfloat ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat diffuse0[] = {0.9f, 0.9f, 0.9f, 1.0f};
	GLfloat diffuse[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat specular[] = {0.0f, 0.0f, 0.0f, 0.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 50.0f);
    glEnable(GL_LIGHTING);
	GLfloat light0_position[] = { 50.0f, 10.0f, 200.0f, 0.0f };
	GLfloat light1_position[] = {200.0f, 10.0f, -200.0f, 0.0f };
	GLfloat light2_position[] = {-200.0f, 10.0f, -200.0f, 0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);

	groundTexture = new GLTexture("grid.bmp");
}

//////////////////////////////////////////////////////////////////////////////
// Initialize the main drawing window
//////////////////////////////////////////////////////////////////////////////

void InitMainWindow(){
	int width = 800;
	int height = 600;

	  //create GLUT window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Mocap Player");

	  //setup callback methods
	glutDisplayFunc(appRender);
	glutIdleFunc(appProcess);
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
	glutKeyboardFunc(processKeyboard);

	GLInit(width, height);
}

//////////////////////////////////////////////////////////////////////////////
// find a given clip
//////////////////////////////////////////////////////////////////////////////

clip *findClip(char c) {
	for (unsigned int n=0; n<ClipList.size(); n++) {
		if (ClipList[n]->cname == c) return ClipList[n];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Initialize the PlayList data structure
//////////////////////////////////////////////////////////////////////////////

void initPlayList() {
	int frameCount = 0;
	for (unsigned int n=0; n<strlen(PlayString); n++) {    // create play list
		char cname = PlayString[n];
		clip *mclip = findClip(cname);
		int f1a = mclip->f1;
		int f2a = mclip->f2;
		int fs = f2a - f1a + 1; 
		int f1r = frameCount;
		frameCount += fs;
		int f2r = frameCount;
		PlayList.push_back(new playclip(f1a,f2a,f1r,f2r,fs,mclip));   // add new playclip to PlayList
	}
}

//////////////////////////////////////////////////////////////////////////////
// read the playlist specification from a file
//////////////////////////////////////////////////////////////////////////////

void loadPlayList(char *fname) {
	char buffer[100];
	FILE *fp = fopen(fname,"rb");
	int pe=0;     // play list entries
	char name[100];
	int f1,f2;

	printf("Reading %s\n",fname);
	while (1) {
		fgets(buffer,100,fp);
		if (feof(fp)) break;
//		printf("%s",buffer);
		std::vector<char*> tokens = getTokens(buffer);
		if (tokens.size() == 0)
			continue;
		if (tokens[0][0]=='#')    // comment line? -- skip over
			continue;
		if (!strncmp(buffer,"skeleton",strlen("skeleton"))) {     // load skeleton
			sscanf(tokens[1],"%s",name);
			af->loadASFCharacter(name);    
		} else if (!strncmp(buffer,"motion",strlen("motion"))) {  // load motion file
			sscanf(tokens[1],"%s",name);
			af->readMotion(name);      // read motion file
		} else if (!strncmp(buffer,"splice",strlen("splice"))) {  // swap upper body motion
			f1 = atof(tokens[1]);
			f2 = atof(tokens[2]);
			int nframes = atof(tokens[3]);
			af->splice(f1,f2,nframes);
		} else if (!strncmp(buffer,"clip",strlen("clip"))) {      // define a motion clip
			f1 = atoi(tokens[1]);
			f2 = atoi(tokens[2]);
			char c = tokens[3][0];
			sscanf(tokens[4],"%s",name);
			ClipList.push_back(new clip(f1,f2,c,name));    
		} else if (!strncmp(buffer,"playlist",strlen("playlist"))) {      // create a playlist
			sscanf(tokens[1],"%s",PlayString);
			initPlayList();
		} else {
			printf("Unrecognized keyword: %s\n",buffer);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// main():    entry point 
//////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv){
    glutInit(&argc, argv);                // required Glut initialization
	InitMainWindow();                     // setup drawing window
	af = new ArticulatedFigure();         // create articulated figure
	loadPlayList("play.txt");             // read skeleton, motions, and play list
	camera.target = Point3d(0,20,0);      // camera setup
	camera.camDistance = -100;
	glutMainLoop();                       // start the main loop
	delete af;                            // exit cleanup
	return 0;
}
