#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

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

void
xSDL_DestroyRenderer(SDL_Renderer **r) {
  if (*r) {
    SDL_DestroyRenderer(*r);
    *r = 0;
  }
}

void
xSDL_DestroyWindow(SDL_Window **w) {
  if (*w) {
    SDL_DestroyWindow(*w);
    *w = 0;
  }
}

void
xSDL_DestroyTexture(SDL_Texture **t) {
  if (*t) {
    SDL_DestroyTexture(*t);
    *t = 0;
  }
}

void
xTTF_CloseFont(TTF_Font **f) {
  if (*f) {
    TTF_CloseFont(*f);
    *f = 0;
  }
}

void
xMix_FreeMusic(Mix_Music **music) {
  if (*music) {
    Mix_FreeMusic(*music);
    *music = 0;
  }
}

