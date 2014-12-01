#ifndef ASSETS_H
#define ASSETS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

static const struct SDL_Color DEFAULT_BG_COLOR = {25, 25, 25, 255};
static const struct SDL_Color DEFAULT_FG_COLOR = {225, 225, 225, 255};
static const int LARGE_FONT_SIZE = 46;
static const int MEDIUM_FONT_SIZE = 34;
static const int SMALL_FONT_SIZE = 22;
static const int BLOCK_WIDTH = 30;
static const int BLOCK_HEIGHT = 30;
static const int NUM_SONGS = 5;

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
