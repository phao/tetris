#ifndef XSDL_H
#define XSDL_H

#include <SDL2/SDL.h>

int
xSDL_SetRenderDrawColor(SDL_Renderer *r, const SDL_Color *c);

int
xSDL_ColorEq(const SDL_Color *c1, const SDL_Color *c2);

int
xSDL_SetTextureColorMod(SDL_Texture *t, const SDL_Color *c);

#endif
