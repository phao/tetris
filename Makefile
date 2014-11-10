CC=gcc
DEBUG_OPTS=
OPTIMIZATION_OPTS=-O3 -march=native
CC_DEFAULT_OPTS=-Wall -Wextra -std=c99 -pedantic

OBJS=main.o
HEADERS=

.c.o:
	$(CC) $(CC_DEFAULT_OPTS) $(DEBUG_OPTS) $(OPTIMIZATION_OPTS) -c $<

%.h.gch:%.h
	$(CC) $(CC_DEFAULT_OPTS) -DTESTING_HEADER_INCLUDES $<

test_headers: $(HEADERS)
	rm -f *.gch

all: test_headers $(OBJS)
	$(CC) *.o -o main -lSDL2 -lSDL2_ttf

run: all
	./main

clear:
	rm -f *.o main
