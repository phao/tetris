CC=gcc
DEBUG_OPTS=-DNDEBUG -DSDL_ASSERT_LEVEL=0
#DEBUG_OPTS=-g3 -DSDL_ASSERT_LEVEL=3
OPTIMIZATION_OPTS=-O3 -march=native -flto -fwhole-program
#OPTIMIZATION_OPTS=-O0
CC_DEFAULT_OPTS=-Wall -Wextra -Werror -std=c99 -pedantic -pipe
LIB_FLAGS=-lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer

CC_CMD=$(CC) $(CC_DEFAULT_OPTS) $(DEBUG_OPTS) $(OPTIMIZATION_OPTS)

OBJS=main.o menu.o error.o text_image.o game.o assets.o 2D.o xSDL.o scores.o \
	music.o

.c.o:
	$(CC_CMD) -c $<

$(shell $(CC_CMD) -MM *.c > deps)

include deps

build: $(OBJS)
	$(CC_CMD) *.o -o main $(LIB_FLAGS)

clean:
	rm -f *.o main
