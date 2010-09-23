CXX=g++
INCLUDES=
FLAGS=-D__MACOSX_CORE__ -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon -lstdc++ -lm

OBJS=	RtAudio.o SunSine.o

all: SunSine

SunSine: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS)

SunSine.o: SunSine.cpp RtAudio.h
	$(CXX) $(FLAGS) SunSine.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o SunSine