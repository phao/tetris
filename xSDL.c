#include <SDL2/SDL.h>

int
xSDL_SetRenderDrawColor(SDL_Renderer *r, const SDL_Color *c) {
  return SDL_SetRenderDrawColor(r, c->r, c->g, c->b, c->a);
}

int
xSDL_ColorEq(const SDL_Color *c1, const SDL_Color *c2) {
  return c1->r == c2->r &&
    c1->g == c2->g &&
    c1->b == c2->b &&
    c1->a == c2->a;
}

int
xSDL_SetTextureColorMod(SDL_Texture *t, const SDL_Color *c) {
  return SDL_SetTextureColorMod(t, c->r, c->g, c->b);
}