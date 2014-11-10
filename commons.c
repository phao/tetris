#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "commons.h"

const char *FONT_FILE = "arcade.ttf";
const int LARGE_FONT_SIZE = 60;
const int MEDIUM_FONT_SIZE = 42;
const int SMALL_FONT_SIZE = 24;

const struct SDL_Color DEFAULT_BG_COLOR = {25, 25, 25, 255};
const struct SDL_Color DEFAULT_FG_COLOR = {225, 225, 225, 255};

void
render_text_image(SDL_Renderer *r,
                  const struct TextImage *t,
                  int x,
                  int y)
{
  SDL_Rect region;

  region.w = t->dim.w;
  region.h = t->dim.h;
  region.x = x;
  region.y = y;
  SDL_RenderCopy(r, t->image, 0, &region);
}

int
init_text_image(struct TextImage *it,
                TTF_Font *font,
                const char *text,
                SDL_Renderer *r)
{
  SDL_Surface *stext;

  stext = TTF_RenderText_Solid(font, text, DEFAULT_FG_COLOR);
  if (!stext) {
    errorFn = TTF_GetError;
    return -1;
  }
  it->image = SDL_CreateTextureFromSurface(r, stext);
  if (!it->image) {
    errorFn = SDL_GetError;
    SDL_FreeSurface(stext);
    return -1;
  }
  it->dim = (struct Dim2D) {stext->w, stext->h};
  SDL_FreeSurface(stext);
  return 0;
}

int
hor_center_within(const struct Dim2D *thing,
                  const struct Dim2D *within)
{
  return (within->w - thing->w)/2;
}
