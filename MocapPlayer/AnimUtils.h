#ifndef _AnimUtils_h_
#define _AnimUtils_h_

//  A few classes and methods that are generally useful for character animation

#include <vector>
#include "Point3d.h"
#include "Vector3d.h"
#include "Quaternion.h"
#include "GLHeaders.h"

#ifdef LINUX
#include <string.h>
#endif

#define PI 3.1415
#define RAD(x) (((x) * PI)/180.0)

//////////////////////////////////////////////////////////////////////////////////////
// DOFInfo:    simple class for modeling rotation-or-translation degrees of freedom
//////////////////////////////////////////////////////////////////////////////////////

class DOFInfo{
public:
	Vector3d axis;     // axis
	bool rot;          // rotational or translational?

	DOFInfo(){                         // constructor
		axis = Vector3d(1,0,0);
		rot = true;
	};
	DOFInfo(const DOFInfo& other){     // copy constructor
		axis = other.axis;
		rot = other.rot;
	}
	DOFInfo(Vector3d axis, bool rot){  // constructor withi initialization 
		this->axis = axis;
		this->rot = rot;
	};
	~DOFInfo(){}                       // destructor
};


//////////////////////////////////////////////////////////////////////////////////////
// lTrim():    returns pointer to first non-white space character
//////////////////////////////////////////////////////////////////////////////////////

inline char* lTrim(char* buffer){
	while (*buffer==' ' || *buffer=='\t' || *buffer=='\n' || *buffer=='\r')
		buffer++;
	return buffer;
}

inline char* rTrim(char* buffer){
	int index = (int)strlen(buffer) - 1;
	while (index>=0){
		if (buffer[index]==' ' || buffer[index]=='\t' || buffer[index]=='\n' || buffer[index]=='\r'){
			buffer[index] = '\0';
			index--;
		}
		else
			break;
	}
	return buffer;
}

inline char* trim(char* buffer){
	return rTrim(lTrim(buffer));
}


//////////////////////////////////////////////////////////////////////////////////////
// getTokens():    splits string into tokens
//////////////////////////////////////////////////////////////////////////////////////

inline std::vector<char*> getTokens(char* input){
	std::vector<char*> result;
	input = lTrim(input);
	//read in the strings one by one - assume that each tokens are less than 100 chars in length
	while (input[0]!='\0'){
		result.push_back(input);
		char tempStr[100];
		sscanf(input, "%s", tempStr);
		input = lTrim(input + strlen(tempStr));
	}
	return result;
}

void drawSphere(Point3d origin, double r, int nrPoints);      // draws 
void drawCylinder(double r, Vector3d v, Point3d org, int nrPoints = 11);
void drawText(float x, float y, char *text);

#endif

