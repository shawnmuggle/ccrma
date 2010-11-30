UNAME := $(shell uname)

CXX=g++
ifeq ($(UNAME), Linux)
	FLAGS=-D__UNIX_JACK__ -c #-g -O0 -fno-inline
	LIBS=-lstdc++ -lm -ljack -lpthread -lGL -lGLU -lglut
endif
ifeq ($(UNAME), Darwin)
	FLAGS=-D__MACOSX_CORE__ -D__MAC__ -c #-g -O0 -fno-inline
	LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
		-framework IOKit -framework Carbon  -framework OpenGL \
		-framework GLUT -framework Foundation -framework SDL \
	 	-lstdc++ -lm -ljack -lSDL

endif

OBJS=	RtAudio.o FlyingDream.o gfx.o vector3d.o ugens.o quaternion.o matrix.o point3d.o vector_math.h OgreQuaternion.o OgreVector3.o

all: FlyingDream

FlyingDream: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS)

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

quaternion.o: Quaternion.cpp Quaternion.h MathsStruct.h
	$(CXX) $(FLAGS) Quaternion.cpp

matrix.o: Matrix.cpp Matrix.h MathsStruct.h
	$(CXX) $(FLAGS) Matrix.cpp

point3d.o: Point3D.cpp Point3D.h
	$(CXX) $(FLAGS) Point3D.cpp

OgreQuaternion.o: OgreQuaternion.cpp OgreQuaternion.h
	$(CXX) $(FLAGS) OgreQuaternion.cpp
OgreVector3.o:  OgreVector3.cpp OgreVector3.h
	$(CXX) $(FLAGS) OgreVector3.cpp

clean:
	rm -f *~ *# *.o FlyingDream