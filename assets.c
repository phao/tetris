#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "xSDL.h"
#include "error.h"
#include "assets.h"

static const char *FONT_FILE = "arcade.ttf";
static const char *TETRIS_BLOCK_FILE = "tetris_block.png";
static const char *BG_IMG_FILE = "bg.png";

static TTF_Font *large_font, *medium_font, *small_font;
static SDL_Texture *tetris_block, *bg_img;

#define ASSERT_VALID_ASSETS() \
  do { \
    SDL_assert(large_font); \
    SDL_assert(medium_font); \
    SDL_assert(small_font); \
    SDL_assert(tetris_block); \
    SDL_assert(bg_img); \
  } while (0)

#define ASSERT_NULL_ASSETS() \
  do { \
    SDL_assert(!large_font); \
    SDL_assert(!medium_font); \
    SDL_assert(!small_font); \
    SDL_assert(!tetris_block); \
    SDL_assert(!bg_img); \
  } while (0)

int
init_assets(SDL_Renderer *r) {
  ASSERT_NULL_ASSETS();

  /*
   * Seems weird having to re-load the same font file several times just
   * because the size changes. But, again, idk how TTF files work.
   */

  large_font = TTF_OpenFont(FONT_FILE, LARGE_FONT_SIZE);
  COND_ERET_IF0(large_font, -1, TTF_GetError());

  medium_font = TTF_OpenFont(FONT_FILE, MEDIUM_FONT_SIZE);
  COND_ERET_IF0(medium_font, -1, TTF_GetError());

  small_font = TTF_OpenFont(FONT_FILE, SMALL_FONT_SIZE);
  COND_ERET_IF0(small_font, -1, TTF_GetError());

  SDL_Surface *aux_img = IMG_Load(TETRIS_BLOCK_FILE);
  COND_ERET_IF0(aux_img, -1, IMG_GetError());
  tetris_block = SDL_CreateTextureFromSurface(r, aux_img);
  SDL_FreeSurface(aux_img);
  COND_ERET_IF0(tetris_block, -1, SDL_GetError());

  aux_img = IMG_Load(BG_IMG_FILE);
  COND_ERET_IF0(aux_img, -1, IMG_GetError());
  bg_img = SDL_CreateTextureFromSurface(r, aux_img);
  SDL_FreeSurface(aux_img);
  COND_ERET_IF0(bg_img, -1, SDL_GetError());

  ASSERT_VALID_ASSETS();

  return 0;
}

TTF_Font*
get_large_font(void) {
  ASSERT_VALID_ASSETS();
  return large_font;
}

TTF_Font*
get_medium_font(void) {
  ASSERT_VALID_ASSETS();
  return medium_font;
}

TTF_Font*
get_small_font(void) {
  ASSERT_VALID_ASSETS();
  return small_font;
}

SDL_Texture*
get_tetris_block_img(void) {
  ASSERT_VALID_ASSETS();
  return tetris_block;
}

SDL_Texture*
get_bg_img(void) {
  ASSERT_VALID_ASSETS();
  return bg_img;
}

void
destroy_assets(void) {
  xSDL_DestroyTexture(&bg_img);
  xSDL_DestroyTexture(&tetris_block);
  xTTF_CloseFont(&small_font);
  xTTF_CloseFont(&medium_font);
  xTTF_CloseFont(&large_font);
}
