#ifndef ASSETS_H
#define ASSETS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

enum {
  LARGE_FONT_SIZE = 46,
  MEDIUM_FONT_SIZE = 34,
  SMALL_FONT_SIZE = 22,
  BLOCK_WIDTH = 30,
  BLOCK_HEIGHT = 30,
  NUM_SONGS = 5
};

int
init_assets(SDL_Renderer *r);

TTF_Font*
get_large_font(void);

TTF_Font*
get_medium_font(void);

TTF_Font*
get_small_font(void);

void
destroy_assets(void);

SDL_Texture*
get_tetris_block_img(void);

SDL_Texture*
get_bg_img(void);

#endif
