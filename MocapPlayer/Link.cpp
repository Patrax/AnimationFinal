#include "Link.h"

#include "GLHeaders.h"
#include "AnimUtils.h"

/**
	Default constructor
*/
Link::Link(void){
	parent = NULL;	
}

/**
	destructor
*/
Link::~Link(void){
	//delete all the joints that belong to this link
	for (unsigned int i=0;i<children.size();i++)
		delete children[i];
}

/**
	This method returns the parent joint of the link
*/
Joint* Link::getParent(){
	return parent;
}

/**
	This method returns the number of children
*/
int Link::getChildCount(){
	return int(children.size());
}

/**
	This method returns the link's ith joint.
*/
Joint* Link::getChild(int i){
	return children[i];
}

/**
	This method is used to draw the current link.
*/
void Link::draw(int flags){
	drawCylinder(0.5, linkAxis, Point3d());
	drawSphere(Point3d()+linkAxis, 0.6, 5);

//	glBegin(GL_LINES);
//		glVertex3d(0,0,0);
//		glVertex3d(linkAxis.x, linkAxis.y, linkAxis.z);
//	glEnd();
}

/**
	This recursively draws all of its children.
*/
void Link::drawChildren(int flags){
	//and then we will recursively draw all the its children
	for (unsigned int i=0;i<children.size();i++)
		children[i]->draw(flags);
}

/**
	This method returns a reference to the array of children joints that is associated with this link
*/
std::vector<Joint*>* Link::getChildren(){
	return &children;
}


