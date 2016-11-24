CC = clang -g
CFLAGS = -O2 -march=native -mtune=native -ffast-math -Wall
CPPFLAGS = $(shell pkg-config --cflags sdl2 SDL2_image)
LDLIBS = $(shell pkg-config --libs sdl2 SDL2_image)

raycaster: raycaster.o effect.o vector.o matrix.o

raycaster.o: raycaster.c common.h vector.h matrix.h effect.h
effect.o: effect.c effect.h common.h
matrix.o: matrix.c common.h matrix.h vector.h
vector.o: vector.c vector.h

clean:
	rm -f raycaster *.o *~
