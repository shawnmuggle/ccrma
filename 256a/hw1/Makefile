CXX=g++
INCLUDES=
FLAGS=-D__UNIX_JACK__ -c
LIBS=-lstdc++ -lm -ljack

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