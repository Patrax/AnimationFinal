#ifndef _MocapPlayer_h_
#define _MocapPlayer_h_

#include <vector>

#define MOUSE_LBUTTON 1
#define MOUSE_RBUTTON 2
#define MOUSE_MBUTTON 3
#define MOUSE_WHEEL_DOWN 4
#define MOUSE_WHEEL_UP 5

#define MOUSE_DOWN 1
#define MOUSE_DRAG 2
#define MOUSE_UP 3
#define MOUSE_MOVE 4

class clip {
public:
	int f1,f2;       // start frame, end frame
	char cname;      // single-character name
	char name[100];  // name
public:
	clip(int f1, int f2, char cname, char *name);  // constructor
};

class playclip {
public:
	int f1a,f2a;    // start, end frame (absolute)
	int f1r,f2r;    // start, end frame (relative, within playlist)
	int fs;			// size of this clip
	clip *mclip;    // pointer to motion clip
public:
	playclip(int f1a, int f2a, int f1r, int f2r, int fs, clip *mclip);   // constructor
};

std::vector<clip*> ClipList;          // list of all labeled clips
std::vector<playclip*> PlayList;      // list of all clips in the current play list


#endif

