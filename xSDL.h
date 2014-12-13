#ifndef X_SDL_H
#define X_SDL_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

int
xSDL_SetRenderDrawColor(SDL_Renderer *r, const SDL_Color *c);

int
xSDL_ColorEq(const SDL_Color *c1, const SDL_Color *c2);

int
xSDL_SetTextureColorMod(SDL_Texture *t, const SDL_Color *c);

void
xSDL_DestroyRenderer(SDL_Renderer **r);

void
xSDL_DestroyWindow(SDL_Window **w);

void
xSDL_DestroyTexture(SDL_Texture **t);

void
xTTF_CloseFont(TTF_Font **f);

void
xMix_FreeMusic(Mix_Music **music);

#endif
