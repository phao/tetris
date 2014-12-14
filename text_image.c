#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "xSDL.h"
#include "error.h"
#include "assets.h"
#include "text_image.h"

int
render_text_image(struct TextImage *ti) {
  SDL_Rect region = {
    .x = ti->pos.x, .y = ti->pos.y,
    .w = ti->dim.w, .h = ti->dim.h
  };
  COND_ERET_LT0(SDL_RenderCopy(ti->rend, ti->image, 0, &region),
    SDL_GetError());
  return 0;
}

int
init_text_image(struct TextImage *ti,
                TTF_Font *font,
                const char *text,
                SDL_Renderer *rend,
                const SDL_Color *color)
{
  ti->image = 0;
  SDL_Surface *stext = TTF_RenderText_Solid(font, text, *color);
  COND_EGOTO_IF0(stext, e_cleanup, TTF_GetError());
  ti->image = SDL_CreateTextureFromSurface(rend, stext);
  COND_EGOTO_IF0(ti->image, e_cleanup, SDL_GetError());
  ti->dim = (PixelDim2D) {stext->w, stext->h};
  ti->pos = (PixelPoint2D) {0, 0};
  SDL_FreeSurface(stext);
  ti->rend = rend;
  return 0;

e_cleanup:
  SDL_FreeSurface(stext);
  return -1;
}

int
in_bounds(const struct TextImage *ti, int x, int y) {
  return x >= ti->pos.x
    && x <= (ti->pos.x + ti->dim.w)
    && y >= ti->pos.y
    && y <= (ti->pos.y + ti->dim.h);
}

void
destroy_text_image(struct TextImage *ti) {
  xSDL_DestroyTexture(&ti->image);
}
