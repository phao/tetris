CC=gcc
#DEBUG_OPTS=-DNDEBUG
DEBUG_OPTS=-g
#OPTIMIZATION_OPTS=-O3 -march=native
OPTIMIZATION_OPTS=-O0
CC_DEFAULT_OPTS=-Wall -Wextra -Werror -std=c99 -pedantic

OBJS=main.o menu.o error.o text_image.o game.o assets.o 2D.o xSDL.o
HEADERS=menu.h.gch text_image.h.gch game.h.gch assets.h.gch scores.h.gch \
	2D.h.gch error.h.gch screens.h.gch xSDL.h.gch

.c.o:
	$(CC) $(CC_DEFAULT_OPTS) $(DEBUG_OPTS) $(OPTIMIZATION_OPTS) -c $<

%.h.gch:%.h
	$(CC) $(CC_DEFAULT_OPTS) -DTESTING_HEADER_INCLUDES $<

headers: $(HEADERS)
	rm -f *.gch

build: $(OBJS)
	$(CC) *.o -o main -lSDL2 -lSDL2_ttf

clean:
	rm -f *.o main

redo: clean build
