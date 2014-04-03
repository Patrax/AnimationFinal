
# GL_LIBS       = -lGL -lGLU -lglut
GL_LIBS       = -framework OpenGL -framework GLUT
CC            = gcc
CCC           = g++
CFLAGS        = -c -g  -Wno-deprecated
CCCFLAGS      = -c -g -Wno-deprecated
# CFLAGS        = -c -g
# CCCFLAGS      = -c -g -DLINUX
LIBS	      = $(GL_LIBS) -lm
LINKER	      = g++
MAKEFILE      = Makefile
OBJS = $(CPPSRCS:.cpp=.o) $(CSRCS:.c=.o)
PROGRAM	      = mplay
RANLIB        = ranlib
CPPSRCS = \
	AnimUtils.cpp \
	ArticulatedFigure.cpp \
	BMPIO.cpp \
	GLCamera.cpp \
	GLTexture.cpp \
	Image.cpp \
	ImageIO.cpp \
	Joint.cpp \
	Link.cpp \
	Matrix4x4.cpp \
	MocapPlayer.cpp \
	Point3d.cpp \
	Quaternion.cpp \
	ThreeTuple.cpp \
	Vector3d.cpp

all:		$(PROGRAM)

%.o : %.cpp
	$(CCC) $(CCCFLAGS) $(CPPFLAGS) $*.cpp

%.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $*.c

$(PROGRAM):	$(OBJS)
		@echo -n "Loading $(PROGRAM) ... "
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;		@rm -f $(OBJS) *~ $(PROGRAM)

tags:    $(SRCS); @etags $(SRCS)
