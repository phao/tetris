#ifndef SCORES_H
#define SCORES_H

#include <SDL2/SDL.h>
#include "2D.h"
#include "screens.h"

int
init_scores(const struct GameContext *gx);

int
add_score(int score);

#endif
