CXX_FLAGS = -I .
CXX_FLAGS += -lm -lgsl
VPATH = .

# ADD MORE WARNINGS!
WARNING := -Wall -Wextra

all: main 

%.o: %.o.d 

main: $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
	g++ $(CXX_FLAGS) $^ -o $@

%.o: %.c Makefile
	g++  $(CXX_FLAGS) -c -o $@  $<

%.o.d: %.c Makefile
	g++ $(CXX_FLAGS) $(WARNING) -MM -MT $(@:.d=) -MT $(@)  $< > $@

-include *.d
                                                                                                                                                                                          
clean:
	rm -f *.o *.o.d
	rm -f main tags
