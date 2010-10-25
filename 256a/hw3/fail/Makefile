UNAME := $(shell uname)

CXX=g++
ifeq ($(UNAME), Linux)
	FLAGS=-D__UNIX_JACK__ -c #-g -O0 -fno-inline
	LIBS=-lstdc++ -lm -ljack
endif
ifeq ($(UNAME), Darwin)
	FLAGS=-D__MACOSX_CORE__ -c #-g -O0 -fno-inline
	LIBS=-lstdc++ -lm \
	-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit

endif

OBJS=	RtAudio.o SeeSound.o

all: SeeSound

SeeSound: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS)

SeeSound.o: SeeSound.cpp SeeSound.h RtAudio.h
	$(CXX) $(FLAGS) SeeSound.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o SeeSound