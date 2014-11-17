#include <SDL2/SDL_ttf.h>

#include "error.h"
#include "assets.h"

static const char *FONT_FILE = "arcade.ttf";

static TTF_Font *large_font, *medium_font;

int
init_assets(void) {
  large_font = TTF_OpenFont(FONT_FILE, LARGE_FONT_SIZE);
  COND_ERROR_SET(large_font, e_bad_large_font, TTF_GetError);

  medium_font = TTF_OpenFont(FONT_FILE, MEDIUM_FONT_SIZE);
  COND_ERROR_SET(medium_font, e_bad_medium_font, TTF_GetError);

  return 0;

e_bad_medium_font:
  TTF_CloseFont(large_font);
e_bad_large_font:
  return -1;
}

TTF_Font*
get_large_font(void) {
  return large_font;
}

TTF_Font*
get_medium_font(void) {
  return medium_font;
}

void
destroy_assets(void) {
  TTF_CloseFont(large_font);
  TTF_CloseFont(medium_font);
}
