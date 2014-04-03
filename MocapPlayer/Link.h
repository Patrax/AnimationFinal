#ifndef _Link_h_
#define _Link_h_

#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Joint.h"
#include <vector>

// methods for a link in an articulated figure

class Link{
public:
	int ID;              // link ID
	char name[100];      // link name
	Quaternion qRel;     // orientation relative to parent
	Joint* parent;       // parent joint
	std::vector<Joint*> children;   // list of child joints
	Vector3d linkAxis;   // link axis to draw, in local coordinates

public:
	Link(void);               // default constructor
	virtual ~Link(void);      // destructor
	Joint* getParent();       // returns parent joint
	int getChildCount();      // returns number of children
	Joint* getChild(int i);   // returns i'th joint
	void draw(int flags);     // draw joint
	void drawChildren(int flags);                      // recursively draws all children
	std::vector<Joint*>* getChildren();                // returns ref to array of child joints
	inline char* getName(){ return name; }             // return name of current link
	inline void setID(int newID){ this->ID = newID; }  // set id of current body
	inline int getID(){ return ID; }                   // return ID of current body
	inline void setName(char* newName){                // assign name to current link
		if (newName == NULL)
			return;
		strcpy(this->name, newName);
	}
};

#endif
