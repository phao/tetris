#ifdef TESTING_HEADER_INCLUDES
#include <SDL2/SDL.h>
#endif

int
xSDL_SetRenderDrawColor(SDL_Renderer *r, const SDL_Color *c);

int
xSDL_ColorEq(const SDL_Color *c1, const SDL_Color *c2);

int
xSDL_SetTextureColorMod(SDL_Texture *t, const SDL_Color *c);