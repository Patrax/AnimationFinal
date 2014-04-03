#define WIN32_LEAN_AND_MEAN

#ifdef WIN32

#include <windows.h>		// The windows header file needs to be here...

#endif


#ifdef __APPLE__
#include <GLUT/glut.h> // why does Apple have to put glut.h here...
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h> // ...when everyone else puts it here?
#endif




