CC=g++
CFLAGS=-Wall -Werror -std=c++11 -I/usr/include/SDL/
LIBS=-lSDL2 \
     -lSDL2_gfx \
     -lSDL2_ttf

all: schelling

debug: CFLAGS += -g -O2
debug: all

Display.o: Display.cpp
	g++ ${CFLAGS} -o $@ -c $^

Sim.o: Sim.cpp
	g++ ${CFLAGS} -o $@ -c $^

schelling: Display.o Sim.o schelling.cpp
	g++ ${CFLAGS} -o $@ $^ ${LIBS}

clean:
	rm -f schelling
	rm -f *.o
