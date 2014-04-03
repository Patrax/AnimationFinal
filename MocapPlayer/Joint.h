#ifndef _Joint_h_
#define _Joint_h_

#include "Point3d.h"
#include "Quaternion.h"
#include "AnimUtils.h"
#include <vector>
class Link;

// joint for an articulated figure
//
// Each joint has a parent link and a child link
// The joint has position (0,0,0) in child linke coordinates
// and has position Link.axis in parent link coordinates
 
class Joint{
public:
	Link* parent;
	Link* child;
	Quaternion qRel;              // relative orientation between parent and child
	std::vector<DOFInfo*> dofs;   // info on list of DOFS for joint
	int ndofs;                     // number of DOFs
	int offset;                   // index offset into data vector for a given frame
public:
	Joint(void);
	inline Link* getChild(){ return child;	}   // reference to child link
	inline Link* getParent(){ return parent;}   // reference to parent link
	void draw(int flags);          // recursively draws child link(s)
	virtual ~Joint(void);          // destructor
	void setParent(Link* pLink);   // sets the parent
	void setChild(Link* cLink);    // sets the child
};

#endif
