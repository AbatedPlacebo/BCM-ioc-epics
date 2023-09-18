CXXFLAGS = -I .. -I ../../../inc
VPATH = .. 

all: main

main.o: main.o.d

main: main.o 
		g++ $^ -o $@

%.o: %.cpp Makefile
		g++ $(CXXFLAGS) -c -o $@  $<

%.o.d: %.cpp Makefile
		g++ $(CXXFLAGS) -MM -MT $(@:.d=) -MT $(@)  $< > $@

-include *.d

clean:
		rm -f *.o *.o.d
		rm -f main tags
