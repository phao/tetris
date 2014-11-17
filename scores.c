#include <SDL2/SDL.h>

#include "commons.h"
#include "screens.h"
#include "game.h"

static const struct SDL_Color PANEL_BG_COLOR = {0, 0, 0, 255};
static const struct SDL_Color PANEL_BORDER_COLOR = {255, 255, 255, 255};

static void
destroy(void) {
}

static enum ScreenId
handle_event(const SDL_Event *e) {
  (void) e;
  return SELF;
}

static enum ScreenId
update(void) {
  return SELF;
}

static void
focus(void) {
}

static void
render(SDL_Renderer *r) {
  (void) r;
}

int
init_scores(SDL_Renderer *r, const struct Dim2D *within) {
  (void) r;
  (void) within;

  AllScreens[SCORES_SCREEN] = (struct ScreenObject) {
    .focus = focus,
    .render = render,
    .update = update,
    .handle_event = handle_event,
    .destroy = destroy
  };

  return 0;
}
