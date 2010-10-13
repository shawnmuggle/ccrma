UNAME := $(shell uname)

CXX=g++
ifeq ($(UNAME), Linux)
	FLAGS=-D__UNIX_JACK__ -D__LINUX_ALSASEQ__ -c -g -O0 -fno-inline
	LIBS=-lstdc++ -lm -ljack -lasound -lpthread
endif
ifeq ($(UNAME), Darwin)
	FLAGS=-D__UNIX_JACK__ -D__MACOSX_CORE__ -c -g -O0 -fno-inline
	LIBS=-lstdc++ -lm -ljack -framework CoreMIDI -framework CoreFoundation -framework CoreAudio
endif

OBJS=	RtAudio.o RtMidi.o Stringy.o

all: Stringy

Stringy: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS)

Stringy.o: Stringy.cpp Stringy.h RtAudio.h
	$(CXX) $(FLAGS) Stringy.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

RtMidi.o: rtmidi/RtMidi.h rtmidi/RtMidi.cpp rtmidi/RtError.h
	$(CXX) $(FLAGS) $(LIBS) rtmidi/RtMidi.cpp

clean:
	rm -f *~ *# *.o Stringy