#include <stdio.h>

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

static struct MenuScreen MENU;

static void
destroy(void) {
  SDL_DestroyTexture(MENU.title.image);
  SDL_DestroyTexture(MENU.new_game.image);
  SDL_DestroyTexture(MENU.scores.image);
}

static enum ScreenId
handle_event(const SDL_Event *e) {
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    if (in_bounds(x, y, &MENU.new_game)) {
      return GAME_SCREEN;
    }
    else if (in_bounds(x, y, &MENU.scores)) {
      return SCORES_SCREEN;
    }
  }
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
  SDL_SetRenderDrawColor(r, DEFAULT_BG_COLOR.r, DEFAULT_BG_COLOR.g,
    DEFAULT_BG_COLOR.b, DEFAULT_BG_COLOR.a);
  SDL_RenderClear(r);
  render_text_image(r, &MENU.title);
  render_text_image(r, &MENU.new_game);
  render_text_image(r, &MENU.scores);
  SDL_RenderPresent(r);
}

int
init_menu(SDL_Renderer *r, const struct Dim2D *within) {
  TTF_Font *title_font, *button_font;

  MENU.within = *within;
  title_font = get_large_font();
  button_font = get_medium_font();

  COND_ERROR(
    init_text_image(&MENU.title, title_font, "Tetris", r) == 0,
    e_bad_title_text);
  MENU.title.pos.x = hor_center_within(&MENU.title.dim, &MENU.within);
  MENU.title.pos.y = TOP_TITLE_OFFSET;

  COND_ERROR(
    init_text_image(&MENU.new_game, button_font, "New Game", r) == 0,
    e_bad_new_game_text);
  MENU.new_game.pos.x = hor_center_within(&MENU.new_game.dim, &MENU.within);
  MENU.new_game.pos.y = TOP_MENU_OFFSET;

  COND_ERROR(
    init_text_image(&MENU.scores, button_font, "High Scores", r) == 0,
    e_bad_scores_text);
  MENU.scores.pos.x = hor_center_within(&MENU.scores.dim, &MENU.within);
  MENU.scores.pos.y = TOP_MENU_OFFSET + MENU.new_game.dim.h;

  AllScreens[MENU_SCREEN] = (struct ScreenObject) {
    .destroy = destroy,
    .handle_event = handle_event,
    .update = update,
    .render = render,
    .focus = focus
  };

  return 0;

e_bad_scores_text:
  SDL_DestroyTexture(MENU.new_game.image);
e_bad_new_game_text:
  SDL_DestroyTexture(MENU.title.image);
e_bad_title_text:
  return -1;
}
