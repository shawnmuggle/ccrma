CXX       = g++
CXXFLAGS  = -g -O0 -Wall -c
LDFLAGS   = -g -O0 -Wall
RM        = rm -f
EXEC      = minigl

all: $(EXEC)

$(EXEC): minigl.o main.o
	$(CXX) $(LDFLAGS) $^ -o $@

minigl.o: minigl.cpp minigl.h
	$(CXX) $(CXXFLAGS) $< -o $@

main.o: main.cpp minigl.h
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	$(RM) *~ *.bak *.o $(EXEC)
