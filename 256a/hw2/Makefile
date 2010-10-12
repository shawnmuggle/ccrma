CXX=g++
INCLUDES=
FLAGS=-D__UNIX_JACK__ -c
LIBS=-lstdc++ -lm -ljack

OBJS=	RtAudio.o Stringy.o

all: Stringy

Stringy: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS)

Stringy.o: Stringy.cpp Stringy.h RtAudio.h
	$(CXX) $(FLAGS) Stringy.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o Stringy