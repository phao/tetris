#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "2D.h"
#include "screens.h"
#include "assets.h"
#include "text_image.h"
#include "error.h"

enum {
  NUM_SCORES = 5,
  PADDING_PX = 15
};

static const struct SDL_Color PANEL_BG_COLOR = {0, 0, 0, 255};
static const struct SDL_Color PANEL_BORDER_COLOR = {255, 255, 255, 255};

static struct TextImage exit_hint, title;
static struct TextImage score_texts[NUM_SCORES];
static int scores[NUM_SCORES];
static int used_scores = 0;

static struct ScreenObject self;

static int
destroy(const struct GameContext *gx) {
  (void) gx;
  return 0;
}

static enum ScreenId
handle_event(const struct GameContext *gx, const SDL_Event *e) {
  (void) gx;
  
  if (e->type == SDL_KEYDOWN) {
    if (e->key.keysym.sym == SDLK_ESCAPE) {
      return MENU_SCREEN;
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
  enum {
    SCORE_CHARS_LIMIT = 30
  };
  
  char score_chars[SCORE_CHARS_LIMIT];
  TTF_Font *medium_font = get_medium_font();
  
  for (int i = 0; i < used_scores; i++) {
    if (score_texts[i].image != 0) {
      SDL_DestroyTexture(score_texts[i].image);
    }
    snprintf(score_chars, SCORE_CHARS_LIMIT, "%d", scores[i]);
    int err = init_text_image(score_texts + i, medium_font, score_chars, gx->r);
    if (err < 0) {
      return err;
    }
    score_texts[i].pos.x = gx->dim.w/2 - score_texts[i].dim.w/2;
    score_texts[i].pos.y = 2*LARGE_FONT_SIZE + PADDING_PX*(i+1) + 
      i*MEDIUM_FONT_SIZE;
  }
  return 0;
}

static int
render(const struct GameContext *gx) {
  render_text_image(gx->r, &title);
  render_text_image(gx->r, &exit_hint);
  
  for (int i = 0; i < used_scores; i++) {
    render_text_image(gx->r, score_texts + i);
  }
  
  return 0;
}

int
init_scores(const struct GameContext *gx) {
  TTF_Font *small_font = get_small_font();
  TTF_Font *large_font = get_large_font();
  
  COND_ERROR(
    init_text_image(&exit_hint, small_font, "Hit ESC to go back.", gx->r) == 0,
    e_bad_exit_hint);
  exit_hint.pos.x = gx->dim.w - exit_hint.dim.w;
  exit_hint.pos.y = gx->dim.h - exit_hint.dim.h;
  
  COND_ERROR(
    init_text_image(&title, large_font, "High Scores", gx->r) == 0,
    e_bad_title);
  title.pos.x = gx->dim.w/2 - title.dim.w/2;
  title.pos.y = LARGE_FONT_SIZE;

  self = (struct ScreenObject) {
    .focus = focus,
    .render = render,
    .update = update,
    .handle_event = handle_event,
    .destroy = destroy
  };
  register_screen(SCORES_SCREEN, &self);

  return 0;

e_bad_title:
  SDL_DestroyTexture(exit_hint.image);
  exit_hint.image = 0;
e_bad_exit_hint:
  return -1;
}

int
add_score(int score) {
  for (int i = 0; i < used_scores; i++) {
    if (scores[i] < score) {
      int aux = scores[i];
      scores[i] = score;
      score = aux;
    }
  }
  if (used_scores < NUM_SCORES) {
    scores[used_scores] = score;
    used_scores++;
  }
  return 0;
}
