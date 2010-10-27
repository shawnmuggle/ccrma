UNAME := $(shell uname)

CXX=g++
ifeq ($(UNAME), Linux)
	FLAGS=-D__UNIX_JACK__ -c #-g -O0 -fno-inline
	LIBS=-lstdc++ -lm -ljack -lglut -lGL -lGLU -lpthread
endif
ifeq ($(UNAME), Darwin)
	FLAGS=-D__MACOSX_CORE__ -c #-g -O0 -fno-inline
	LIBS=-lstdc++ -lm \
	-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit

endif

OBJS=	RtAudio.o SeeSound.o chuck_fft.o gfx.o vector3d.o

all: SeeSound

SeeSound: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS)

SeeSound.o: SeeSound.cpp RtAudio.h chuck_fft.h gfx.h vector3d.h
	$(CXX) $(FLAGS) SeeSound.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

chuck_fft.o: chuck_fft.c chuck_fft.h
	$(CXX) $(FLAGS) chuck_fft.c

gfx.o: gfx.cpp gfx.h vector3d.h
	$(CXX) $(FLAGS) gfx.cpp

vector3d.o: vector3d.cpp vector3d.h
	$(CXX) $(FLAGS) vector3d.cpp

clean:
	rm -f *~ *# *.o SeeSound