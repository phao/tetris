#ifndef SCREENS_H
#define SCREENS_H

#include <SDL2/SDL.h>

#include "2D.h"

enum ScreenId {
  MENU_SCREEN, GAME_SCREEN, SCORES_SCREEN
};

static const struct SDL_Color DEFAULT_FG_COLOR = {225, 225, 225, 255};

enum {
  NUM_SCREENS = 3
};

struct GameContext {
  SDL_Renderer *rend;
  PixelDim2D dim;
};

typedef void (*ScreenDestroyFn)(void);
typedef int (*ScreenHandleEventFn)(const SDL_Event *e);
typedef int (*ScreenUpdateFn)(void);
typedef int (*ScreenRenderFn)(void);
typedef int (*ScreenFocusFn)(void);

struct ScreenObject {
  ScreenDestroyFn destroy;
  ScreenHandleEventFn handle_event;
  ScreenUpdateFn update;
  ScreenRenderFn render;
  ScreenFocusFn focus;
};

/**
 * The given ScreenObject data will be copied interally by the implementation
 * of the register_screen function.
 */
void
register_screen(const enum ScreenId which,
                const struct ScreenObject *screen);

void
change_screen(const enum ScreenId which);

#endif
