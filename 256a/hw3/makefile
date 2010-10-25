
CXX=g++
INCLUDES=
FLAGS=-D__MACOSX_CORE__ -c
LIBS=-framework CoreAudio -framework CoreMIDI -framework CoreFoundation \
	-framework IOKit -framework Carbon  -framework OpenGL \
	-framework GLUT -framework Foundation \
	-framework AppKit -lstdc++ -lm

OBJS=   RtAudio.o VisualSine.o chuck_fft.o

VisualSine: $(OBJS)
	$(CXX) -o VisualSine $(OBJS) $(LIBS)

VisualSine.o: VisualSine.cpp RtAudio.h chuck_fft.h
	$(CXX) $(FLAGS) VisualSine.cpp

RtAudio.o: RtAudio.h RtAudio.cpp RtError.h
	$(CXX) $(FLAGS) RtAudio.cpp

chuck_fft.o: chuck_fft.c chuck_fft.h
	$(CXX) $(FLAGS) chuck_fft.c

clean:
	rm -f *~ *# *.o VisualSine
