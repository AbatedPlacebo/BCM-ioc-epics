CXX_FLAGS = -lm -lgsl
all: main

main.o: main.o.d

main: main.o 
	g++ $^ $(CXX_FLAGS) -o $@

%.o: %.c Makefile
	gcc  -c -o $@  $<

%.o.d: %.c Makefile
	gcc -MM -MT $(@:.d=) -MT $(@)  $< > $@

-include *.d

clean:
	rm -f *.o *.o.d
	rm -f main tags

