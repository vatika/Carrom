CC = g++
CFLAGS = -Wall
PROG = sampleCollision
LDFLAGS= -framework GLUT -framework OpenGL -framework Cocoa 
SRCS = sampleCollision.cpp
LIBS = -lglut -lGL -lGLU

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(LDFLAGS) -o $(PROG) $(SRCS)

clean:
	rm -f $(PROG)
