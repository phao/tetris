CC=gcc
#DEBUG_OPTS=-DNDEBUG
DEBUG_OPTS=-g -DSDL_ASSERT_LEVEL=3
#OPTIMIZATION_OPTS=-O3 -march=native -flto -fwhole-program
OPTIMIZATION_OPTS=-O0
CC_DEFAULT_OPTS=-Wall -Wextra -Werror -std=c99 -pedantic -pipe
LIB_FLAGS=-lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer

OBJS=main.o menu.o error.o text_image.o game.o assets.o 2D.o xSDL.o scores.o \
	music.o

.c.o:
	$(CC) $(CC_DEFAULT_OPTS) $(DEBUG_OPTS) $(OPTIMIZATION_OPTS) -c $<

build: $(OBJS)
	$(CC) $(OPTIMIZATION_OPTS) *.o -o main $(LIB_FLAGS)

clean:
	rm -f *.o main
