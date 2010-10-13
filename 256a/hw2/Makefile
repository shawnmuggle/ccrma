CXX=g++
INCLUDES=
FLAGS=-D__LINUX_ALSA__ -c #-g -O0 -fno-inline
OSX_FLAGS=-D__UNIX_JACK__ -D__MACOSX_CORE__ -c
LINUX_FLAGS=-D__LINUX__ALSA__ -D__LINUX_ALSASEQ__ -c
LIBS=-lstdc++ -lm
LINUX_LIBS=-lasound -lpthread
OSX_LIBS= -ljack -framework CoreMIDI -framework CoreFoundation -framework CoreAudio

OBJS=	RtAudio.o RtMidi.o Stringy.o

all: Stringy

Stringy: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS) $(OSX_LIBS)

Stringy.o: Stringy.cpp Stringy.h RtAudio.h
	$(CXX) $(OSX_FLAGS) Stringy.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(OSX_FLAGS) RtAudio.cpp

RtMidi.o: rtmidi/RtMidi.h rtmidi/RtMidi.cpp rtmidi/RtError.h
	$(CXX) $(OSX_FLAGS) $(OSX_LIBS) rtmidi/RtMidi.cpp

clean:
	rm -f *~ *# *.o Stringy