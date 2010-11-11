UNAME := $(shell uname)

CXX=g++
ifeq ($(UNAME), Linux)
	FLAGS=-D__UNIX_JACK__ -c #-g -O0 -fno-inline
	LIBS=-lstdc++ -lm -ljack -lpthread -lGL -lGLU -lglut
endif
ifeq ($(UNAME), Darwin)
	FLAGS=-D__UNIX_JACK__ -D__MAC__ -c #-g -O0 -fno-inline
	LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
		-framework IOKit -framework Carbon  -framework OpenGL \
		-framework GLUT -framework Foundation \
		-framework AppKit -lstdc++ -lm -ljack

endif

OBJS=	RtAudio.o FlyingDream.o gfx.o vector3d.o ugens.o

all: FlyingDream

FlyingDream: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS) $(OSC_OBJECTS)

FlyingDream.o: FlyingDream.cpp FlyingDream.h RtAudio.h gfx.h vector3d.h ugens.h
	$(CXX) $(FLAGS) FlyingDream.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

gfx.o: gfx.cpp gfx.h vector3d.h
	$(CXX) $(FLAGS) gfx.cpp

vector3d.o: vector3d.cpp vector3d.h
	$(CXX) $(FLAGS) vector3d.cpp

ugens.o: ugens.cpp ugens.h
	$(CXX) $(FLAGS) ugens.cpp

clean:
	rm -f *~ *# *.o FlyingDream