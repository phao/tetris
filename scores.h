#ifndef SCORES_H
#define SCORES_H

#include <SDL2/SDL.h>

#include "screens.h"
#include "2D.h"

int
init_scores(SDL_Renderer *g_rend_, const PixelDim2D *screen_dim_);

void
add_score(int score);

#endif
