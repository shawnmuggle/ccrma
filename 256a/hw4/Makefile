UNAME := $(shell uname)

CXX=g++
ifeq ($(UNAME), Linux)
	FLAGS=-D__UNIX_JACK__ -D__LINUX_ALSASEQ__ -c #-g -O0 -fno-inline
	LIBS=-lstdc++ -lm -ljack -lasound -lpthread
endif
ifeq ($(UNAME), Darwin)
	FLAGS=-D__UNIX_JACK__ -D__MACOSX_CORE__ -c -g -O0 -fno-inline
	LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
		-framework IOKit -framework Carbon  -framework OpenGL \
		-framework GLUT -framework Foundation \
		-framework AppKit -lstdc++ -lm -ljack

endif

OBJS=	RtAudio.o Webleep.o
OSC_OBJECTS =  oscpack/osc/OscOutboundPacketStream.o oscpack/osc/OscTypes.o oscpack/ip/posix/NetworkingUtils.o oscpack/ip/posix/UdpSocket.o oscpack/ip/IpEndpointName.o oscpack/osc/OscReceivedElements.o oscpack/osc/OscPrintReceivedElements.o

all: Webleep oscpack

Webleep: $(OBJS)
	$(CXX) -o $@ $+ $(LIBS) $(OSC_OBJECTS)

Webleep.o: Webleep.cpp Webleep.h RtAudio.h 
	$(CXX) $(FLAGS) Webleep.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

oscpack:
	-make -f oscpack/Makefile all

clean:
	rm -f *~ *# *.o Webleep