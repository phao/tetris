#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "error.h"
#include "xassert.h"
#include "assets.h"

static const char *FONT_FILE = "arcade.ttf";
static const char *TETRIS_BLOCK_FILE = "tetris_block.png";
static const char *BG_IMG_FILE = "bg.png";

static TTF_Font *large_font, *medium_font, *small_font;
static SDL_Texture *tetris_block, *bg_img;

#define xassert_valid_assets() do { xassert(large_font); \
  xassert(medium_font); xassert(small_font); xassert(tetris_block); \
  xassert(bg_img); } while (0)

#define xassert_null_assets() do { xassert(!large_font); \
  xassert(!medium_font); xassert(!small_font); xassert(!tetris_block); \
  xassert(!bg_img); } while (0)

int
init_assets(SDL_Renderer *r) {
  xassert_null_assets();
  
  large_font = TTF_OpenFont(FONT_FILE, LARGE_FONT_SIZE);
  COND_ERROR_SET(large_font, e_bad_large_font, TTF_GetError);

  medium_font = TTF_OpenFont(FONT_FILE, MEDIUM_FONT_SIZE);
  COND_ERROR_SET(medium_font, e_bad_medium_font, TTF_GetError);
  
  small_font = TTF_OpenFont(FONT_FILE, SMALL_FONT_SIZE);
  COND_ERROR_SET(small_font, e_bad_small_font, TTF_GetError);

  SDL_Surface *aux_img = IMG_Load(TETRIS_BLOCK_FILE);
  COND_ERROR_SET(aux_img, e_bad_tetris_block, IMG_GetError);
  tetris_block = SDL_CreateTextureFromSurface(r, aux_img);
  SDL_FreeSurface(aux_img);
  COND_ERROR_SET(tetris_block, e_bad_tetris_block, SDL_GetError);
  
  aux_img = IMG_Load(BG_IMG_FILE);
  COND_ERROR_SET(aux_img, e_bad_bg_img, IMG_GetError);
  bg_img = SDL_CreateTextureFromSurface(r, aux_img);
  SDL_FreeSurface(aux_img);
  COND_ERROR_SET(bg_img, e_bad_bg_img, SDL_GetError);
  
  xassert_valid_assets();
  
  return 0;

e_bad_bg_img:
  SDL_DestroyTexture(tetris_block);
  tetris_block = 0;
e_bad_tetris_block:
  TTF_CloseFont(small_font);
  small_font = 0;
e_bad_small_font:
  TTF_CloseFont(medium_font);
  medium_font = 0;
e_bad_medium_font:
  TTF_CloseFont(large_font);
  large_font = 0;
e_bad_large_font:
  return -1;
}

TTF_Font*
get_large_font(void) {
  xassert_valid_assets();
  return large_font;
}

TTF_Font*
get_medium_font(void) {
  xassert_valid_assets();
  return medium_font;
}

TTF_Font*
get_small_font(void) {
  xassert_valid_assets();
  return small_font;
}

SDL_Texture*
get_tetris_block_img(void) {
  xassert_valid_assets();
  return tetris_block;
}

SDL_Texture*
get_bg_img(void) {
  xassert_valid_assets();
  return bg_img;
}

void
destroy_assets(void) {
  xassert_valid_assets();
  
  TTF_CloseFont(large_font);
  TTF_CloseFont(medium_font);
  TTF_CloseFont(small_font);
  SDL_DestroyTexture(tetris_block);
  SDL_DestroyTexture(bg_img);
  
  large_font = 0;
  medium_font = 0;
  small_font = 0;
  tetris_block = 0;
  bg_img = 0;
  
  xassert_null_assets();
}
