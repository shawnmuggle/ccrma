CXX=g++
INCLUDES=
FLAGS=-D__UNIX_JACK__ -c -g -O0 -fno-inline
OSX_FLAGS=-D__MACOSX_CORE__ -c
LIBS=-lstdc++ -lm -ljack
OSX_LIBS=-framework CoreMIDI -framework CoreFoundation -framework CoreAudio

OBJS=	RtAudio.o RtMidi.o Stringy.o

all: Stringy

Stringy: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS) $(OSX_LIBS)

Stringy.o: Stringy.cpp Stringy.h RtAudio.h
	$(CXX) $(FLAGS) Stringy.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

RtMidi.o: rtmidi/RtMidi.h rtmidi/RtMidi.cpp rtmidi/RtError.h
	$(CXX) $(OSX_FLAGS) rtmidi/RtMidi.cpp

clean:
	rm -f *~ *# *.o Stringy