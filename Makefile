CC=gcc
#DEBUG_OPTS=-NDEBUG
DEBUG_OPTS=-g
#OPTIMIZATION_OPTS=-O3 -march=native
OPTIMIZATION_OPTS=
CC_DEFAULT_OPTS=-Wall -Wextra -std=c99 -pedantic

OBJS=main.o commons.o menu.o errorfn.o
HEADERS=commons.h.gch menu.h.gch

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
