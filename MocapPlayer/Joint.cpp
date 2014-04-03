#include "Joint.h"
#include "Link.h"

#include "GLHeaders.h"

/**
	constructor
*/
Joint::Joint(void){
	parent = child = NULL;
}

/**
	destructor.
*/
Joint::~Joint(void){
	if (child)
		delete child;
	for (unsigned int i=0;i<dofs.size();i++)
		delete dofs[i];
}


/**
	This method is used to draw the child link and recursively draw all of its children after
*/
void Joint::draw(int flags){
	//prepare the OGL transformation matrices...

	Matrix4x4 toParent;
	toParent.loadIdentity();

	//we will apply the matrix that corresponds to the relative orientation
	qRel.getRotationMatrix(&toParent);

	glPushMatrix();
	double values[16];
	toParent.getOGLValues(values);
	glMultMatrixd(values);	

	//first we will draw the current link
	child->draw(flags);
	//and make sure that as we draw its children we start where the current link ends.
	glTranslated(child->linkAxis.x, child->linkAxis.y, child->linkAxis.z);
	//now set the openGL matrix to this relative orientation
	child->drawChildren(flags);

	glPopMatrix();

}

/**
	This method sets the parent of the joint
*/
void Joint::setParent(Link* pLink){
	this->parent = pLink;
	pLink->children.push_back(this);
}

/**
	This method sets the child of the joint
*/
void Joint::setChild(Link* cLink){
	this->child = cLink;
	cLink->parent = this;
}



