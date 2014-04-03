#include "AnimUtils.h"
#ifdef __APPLE__
#include <GLUT/glut.h> // why does Apple have to put glut.h here...
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h> // ...when everyone else puts it here?
#endif

///////////////////////////////////////////////////////////////
// draws a coordinate frame
///////////////////////////////////////////////////////////////

void drawFrame(float s)
{
	glPushMatrix();
	glScalef(s,s,s);
	glBegin(GL_LINE_STRIP);

	glColor3f(1,0,0);
	glVertex3f(0,0,0);
	glVertex3f(1,0,0);

	glColor3f(0,1,0);
	glVertex3f(0,0,0);
	glVertex3f(0,1,0);

	glColor3f(0,0,1);
	glVertex3f(0,0,0);
	glVertex3f(0,0,1);

	glEnd();
	glPopMatrix();
}

/////////////////////////////////////////////////////////
//  PROC:  drawText()
/////////////////////////////////////////////////////////

void drawText(float x, float y, char *text) {
  glColor3f( 0, 0, 0 );       /* Black text */
  glRasterPos2f( x,y);     /* Text start position */
  for( int i=0; i<(int)strlen( text ); i++ )
    glutBitmapCharacter( GLUT_BITMAP_HELVETICA_18, text[i] );
}

///////////////////////////////////////////////////////////////
// draws a ground arrow
///////////////////////////////////////////////////////////////

void drawArrow(float s, float x, float z, float th)
{
	glPushMatrix();
	glTranslatef(x,0,z);
	glScalef(s,s,s);
	glRotatef(th,0,-1,0);

	glDisable(GL_LIGHTING);
	glColor3f(0,0,1);
	glBegin(GL_POLYGON);
  	  glVertex3f(0,0,0.1f);
	  glVertex3f(0,0,-0.1f);
  	  glVertex3f(0.8f,0,-0.1f);
	  glVertex3f(0.8f,0,0.1f);
	glEnd();
	glBegin(GL_POLYGON);
	  glVertex3f(0.8f,0,-0.2f);
	  glVertex3f(1.0f,0,0);
	  glVertex3f(0.8f,0,0.2f);
	glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

/**
	This method draws a sphere of radius r, centered at the origin. It uses nrPoints for the approximation.
*/
void drawSphere(Point3d origin, double r, int nrPoints){
	int j;
	double i, angle = PI/nrPoints;
	//this is the normal vector
	Vector3d n, v;

	glPushMatrix();
	glTranslated(origin.x, origin.y, origin.z);

	Point3d p, q;
	
	glBegin(GL_QUAD_STRIP);
		p.x = r*cos(-PI/2);
		p.y = r*sin(-PI/2);
		p.z = 0;
		for (i=-PI/2+angle;i<=PI/2;i+=angle)
		{
			q.x = r*cos(i);
			q.y = r*sin(i);
			q.z = 0;
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(p).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(p.x,p.y,p.z);
			//make sure we compute the normal as well as the node coordinates
			n = Vector3d(q).toUnit();
			glNormal3d(n.x, n.y, n.z);
			glVertex3d(q.x,q.y,q.z);

			for (j=0;j<=2*nrPoints;j++){
				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(q.x * cos(j * angle), q.y, q.x * sin(j * angle));
				n = v; n.toUnit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(v.x, v.y, v.z);

				//make sure we compute the normal as well as the node coordinates
				v = Vector3d(p.x * cos(j * angle), p.y, p.x * sin(j * angle));
				n = v; n.toUnit();
				glNormal3d(n.x, n.y, n.z);
				glVertex3d(v.x, v.y, v.z);
			}
			p = q;
		}

	glEnd();
	glPopMatrix();
}

/**
	This method draws a cylinder of thinkness r, along the vector dir.
*/
void drawCylinder(double r, Vector3d v, Point3d org, int nrPoints){
	//we'll start out by getting a vector that is perpendicular to the given vector.
	Vector3d n;
	Vector3d axis = v;
	axis.toUnit();
	int i;
	//try to get a vector that is not colinear to v.
	if (v.x != 0 || v.y != 0)
		n = Vector3d(v.x, v.y, v.z + 1);
	else if (v.y != 0 || v.z != 0)
		n = Vector3d(v.x, v.y+1, v.z);
	else
		n = Vector3d(v.x+1, v.y, v.z);

	n = n.crossProductWith(v);

	if (v.length() < 0.0001 || n.length() < 0.0001)
		return;
	(n.toUnit()) *= r;
	glBegin(GL_TRIANGLE_STRIP);

	//now, we we'll procede by rotating the vector n around v, and create the cylinder that way.
	for (i=0;i<=nrPoints;i++){
		Vector3d p = Quaternion::getRotationQuaternion(2*i*PI/nrPoints, axis).rotate(n);
		Vector3d normal = p; normal.toUnit();
		glNormal3d(normal.x, normal.y, normal.z);
		Point3d p1 = org + p;
		glVertex3d(p1.x, p1.y, p1.z);
		Point3d p2 = org + v + p;
		glVertex3d(p2.x, p2.y, p2.z);
	}

	glEnd();

}


