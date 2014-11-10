#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "commons.h"

struct MenuScreen {
  struct Dim2D within;
  struct TextImage title, new_game, scores;
};

static struct MenuScreen MENU;

void
destroy_menu(void) {
  SDL_DestroyTexture(MENU.title.image);
  SDL_DestroyTexture(MENU.new_game.image);
  SDL_DestroyTexture(MENU.scores.image);
}

int
init_menu(SDL_Renderer *r, const struct Dim2D *within) {
  TTF_Font *title_font, *button_font;

  MENU.within = *within;
  title_font = TTF_OpenFont(FONT_FILE, LARGE_FONT_SIZE);
  COND_ERROR_SET(title_font, e_bad_title_font, TTF_GetError);

  button_font = TTF_OpenFont(FONT_FILE, MEDIUM_FONT_SIZE);
  COND_ERROR_SET(button_font, e_bad_button_font, TTF_GetError);

  COND_ERROR(
    init_text_image(&MENU.title, title_font, "Tetris", r) >= 0,
    e_bad_title_text);

  COND_ERROR(
    init_text_image(&MENU.new_game, button_font, "New Game", r) >= 0,
    e_bad_new_game_text);

  COND_ERROR(
    init_text_image(&MENU.scores, button_font, "High Scores", r) >= 0,
    e_bad_scores_text);

  return 0;

e_bad_scores_text:
  SDL_DestroyTexture(MENU.new_game.image);
e_bad_new_game_text:
  SDL_DestroyTexture(MENU.title.image);
e_bad_title_text:
  TTF_CloseFont(button_font);
e_bad_button_font:
  TTF_CloseFont(title_font);
e_bad_title_font:
  return -1;
}

void
render_menu(SDL_Renderer *r) {
  const int TOP_TITLE_OFFSET = 50;
  const int TOP_MENU_OFFSET = 250;

  SDL_SetRenderDrawColor(r, DEFAULT_BG_COLOR.r, DEFAULT_BG_COLOR.g,
    DEFAULT_BG_COLOR.b, DEFAULT_BG_COLOR.a);
  SDL_RenderClear(r);

  render_text_image(r, &MENU.title,
    hor_center_within(&MENU.title.dim, &MENU.within),
    TOP_TITLE_OFFSET);

  render_text_image(r, &MENU.new_game,
    hor_center_within(&MENU.new_game.dim, &MENU.within),
    TOP_MENU_OFFSET);

  render_text_image(r, &MENU.scores,
    hor_center_within(&MENU.scores.dim, &MENU.within),
    TOP_MENU_OFFSET + MENU.new_game.dim.h);

  SDL_RenderPresent(r);
}

enum Screen
send_menu_event(const SDL_Event *e) {
  return SAME_SCREEN;
}

