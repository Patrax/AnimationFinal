CPP_FILES = $(wildcard *.cpp)
OBJS      = $(CPP_FILES:.cpp=.o)
PROGRAM	  = go

all: $(PROGRAM)

%.o : %.cpp
	g++ -g -Wno-deprecated -c -o $@ $<

$(PROGRAM): $(OBJS)
	g++ $(OBJS) -framework OpenGL -framework GLUT -lm -o $(PROGRAM)

# $(PROGRAM): $(OBJS)
# 	g++ $(OBJS) -lGL -lGLU -lglut -lm -o $(PROGRAM)

clean:
	@rm -rf *.o $(PROGRAM)
