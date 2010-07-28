CC=gcc
CPP=g++
LD=gcc

LD_LIB=-lm -lncurses

all: nc_test

nc_test: main.o
	$(LD) -o $@ $^ $(LD_LIB)

clean:
	rm -f *.o
	rm -f nc_test

