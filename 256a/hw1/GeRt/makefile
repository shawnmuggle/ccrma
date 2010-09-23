CXX=g++
INCLUDES=
FLAGS=-D__MACOSX_CORE__ -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon -lstdc++ -lm

OBJS=   RtAudio.o MikeSine.o

MikeSine: $(OBJS)
	$(CXX) -o MikeSine $(OBJS) $(LIBS)

MikeSine.o: MikeSine.cpp RtAudio.h
	$(CXX) $(FLAGS) MikeSine.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

clean:
	rm -f *~ *# *.o MikeSine