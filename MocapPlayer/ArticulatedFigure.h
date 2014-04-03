#ifndef _ArticulatedFigure_h_
#define _ArticulatedFigure_h_

#include "Joint.h"
#include "Link.h"
#include <stdio.h>
#include <stdlib.h>

class ASFBoneInfo;

class ArticulatedFigure{
public:
	Link* root;                        // root link
	std::vector<Link*> links;          // list of all links
	std::vector<DOFInfo*> rootDofs;    // DOFs for the root
	Quaternion QRoot;                  // root orienation
	Vector3d PRoot;                    // root position
	static const int maxFrames = 100000;       // max # of frames
	static const int maxChannels = 80;        // max # of channels, i.e., angle data per frame
	float data[maxFrames][maxChannels];      // frame data
	void playBack(int f);              // plays back frames f
	void playBack(float *frameData);   // plays back frame from the given data
	void playBack2(int f1, int f2, double w1);
	void splice(int f1, int f2, int nf);  // swaps upper body motions
	void postProcess(int f1, int f2);  // post process to compute facing directions
	int frameCount;					   // current max frame count
	int totalDOFs;                     // total DOFs for the character
	double Cx, Cz, Cth, Wth;           // current x,y, theta, and World theta of current frame

public:
	ArticulatedFigure(void);                // constructor
	void loadASFCharacter(char* ASFFName);       // load character from ASF file
	void readRootInformation(FILE* fp);          // read root link info
	void readBoneDataInformation(FILE* fp, std::vector<ASFBoneInfo*>* tempBones);    // read bone info for each bone
	void readHierarchyInformation(FILE* fp, std::vector<ASFBoneInfo*>* tempBones);   // read skeleton hierarchy
	void readAMCFrame(FILE* fp);         // read single frame
	void readMotion(char *fname);        // reads and stores frames
	~ArticulatedFigure(void);            // destructor
	Link* getLinkByName(char* lName);    // find link with a given name
	void draw(int flags);                // recursively draw articulated body
	void drawR(int flags);               // recursively draw articulated body, relative
	void print(char *fname);             // prints AF summary information
};

////////////////////////////////////////////////////////////////////////////
// ASFBoneInfo:   holds info on ASF bones
////////////////////////////////////////////////////////////////////////////

class ASFBoneInfo{
public:
	int id;                 // bone ID
	char name[100];         // bone name
	Vector3d direction;     // direction of bone, in local coords
	double length;          // length of bone, in local coords
	Matrix4x4 C;   // joint Euler angles given wrt a local frame obtained by using this rotation
	std::vector<DOFInfo*> dofs;    // store info regarding DOFs for this bone

	ASFBoneInfo(){                 // default constructor
		id = 0;
		name[0] = '\0';
		direction = Vector3d(1,0,0);
		length = 0;
		C.loadIdentity();
	}
	ASFBoneInfo(FILE* fp);         // constructor from file info
	~ASFBoneInfo(){                // destructor
		for (unsigned int i=0;i<dofs.size();i++)
			delete dofs[i];
	}
};

  // reads DOF info from an ASF-style string; returns DynamicArray of pointers to DOFInfo
std::vector<DOFInfo*> readDOFs(char* tempLine);  // reads DOF info from an ASF-style string

#endif


