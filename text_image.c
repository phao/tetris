#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "2D.h"
#include "error.h"
#include "assets.h"
#include "text_image.h"

void
render_text_image(SDL_Renderer *r, const struct TextImage *t) {
  SDL_Rect region;

  region.w = t->dim.w;
  region.h = t->dim.h;
  region.x = t->pos.x;
  region.y = t->pos.y;
  SDL_RenderCopy(r, t->image, 0, &region);
}

int
init_text_image(struct TextImage *ti,
                TTF_Font *font,
                const char *text,
                SDL_Renderer *r)
{
  SDL_Surface *stext;

  stext = TTF_RenderText_Solid(font, text,
    DEFAULT_FG_COLOR);
  if (!stext) {
    errorFn = TTF_GetError;
    return -1;
  }
  ti->image = SDL_CreateTextureFromSurface(r, stext);
  if (!ti->image) {
    errorFn = SDL_GetError;
    SDL_FreeSurface(stext);
    return -1;
  }
  ti->dim = (struct Dim2D) {stext->w, stext->h};
  SDL_FreeSurface(stext);
  return 0;
}

int
in_bounds(int x, int y, const struct TextImage *ti) {
  return x >= ti->pos.x && x <= (ti->pos.x + ti->dim.w) &&
    y >= ti->pos.y && y <= (ti->pos.y + ti->dim.h);
}
