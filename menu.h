#ifndef MENU_H
#define MENU_H

#include <SDL2/SDL.h>

#include "screens.h"
#include "2D.h"

int
init_menu(SDL_Renderer *g_rend_, const PixelDim2D *screen_dim_);

#endif
