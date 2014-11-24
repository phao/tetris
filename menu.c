#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "2D.h"
#include "error.h"
#include "text_image.h"
#include "screens.h"
#include "assets.h"
#include "menu.h"

struct MenuScreen {
  struct Dim2D within;
  struct TextImage title, new_game, scores;
};

enum {
  TOP_TITLE_OFFSET = 50,
  TOP_MENU_OFFSET = 250
};

static struct ScreenObject self;
static struct MenuScreen menu;

static int
destroy(const struct GameContext *gx) {
  (void) gx;
  
  assert(menu.title.image);
  assert(menu.new_game.image);
  assert(menu.scores.image);
  SDL_DestroyTexture(menu.title.image);
  SDL_DestroyTexture(menu.new_game.image);
  SDL_DestroyTexture(menu.scores.image);
  return 0;
}

static enum ScreenId
handle_event(const struct GameContext *gx, const SDL_Event *e) {
  (void) gx;
  
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    if (in_bounds(x, y, &menu.new_game)) {
      return GAME_SCREEN;
    }
    else if (in_bounds(x, y, &menu.scores)) {
      return SCORES_SCREEN;
    }
  }
  return SELF;
}

static enum ScreenId
update(const struct GameContext *gx) {
  (void) gx;
  
  return SELF;
}

static int
focus(const struct GameContext *gx) {
  (void) gx;
  
  return 0;
}

static int
render(const struct GameContext *gx) {
  render_text_image(gx->r, &menu.title);
  render_text_image(gx->r, &menu.new_game);
  render_text_image(gx->r, &menu.scores);
  return 0;
}

int
init_menu(const struct GameContext *gx) {
  TTF_Font *title_font, *button_font;

  menu.within = gx->dim;
  title_font = get_large_font();
  button_font = get_medium_font();

  COND_ERROR(
    init_text_image(&menu.title, title_font, "Tetris", gx->r) == 0,
    e_bad_title_text);
  menu.title.pos.x = hor_center_within(&menu.title.dim, &menu.within);
  menu.title.pos.y = TOP_TITLE_OFFSET;

  COND_ERROR(
    init_text_image(&menu.new_game, button_font, "New Game", gx->r) == 0,
    e_bad_new_game_text);
  menu.new_game.pos.x = hor_center_within(&menu.new_game.dim, &menu.within);
  menu.new_game.pos.y = TOP_MENU_OFFSET;

  COND_ERROR(
    init_text_image(&menu.scores, button_font, "High Scores", gx->r) == 0,
    e_bad_scores_text);
  menu.scores.pos.x = hor_center_within(&menu.scores.dim, &menu.within);
  menu.scores.pos.y = TOP_MENU_OFFSET + menu.new_game.dim.h;

  self = (struct ScreenObject) {
    .destroy = destroy,
    .handle_event = handle_event,
    .update = update,
    .render = render,
    .focus = focus
  };
  register_screen(MENU_SCREEN, &self);

  return 0;

e_bad_scores_text:
  SDL_DestroyTexture(menu.new_game.image);
e_bad_new_game_text:
  SDL_DestroyTexture(menu.title.image);
e_bad_title_text:
  return -1;
}
