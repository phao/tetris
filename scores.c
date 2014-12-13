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
static SDL_Renderer *g_rend;
static PixelDim2D screen_dim;

static void
destroy(void) {
  destroy_text_image(&exit_hint);
  destroy_text_image(&title);
  for (int i = 0; i < NUM_SCORES; i++) {
    destroy_text_image(score_texts + i);
  }
}

static int
handle_event(const SDL_Event *e) {
  if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
    change_screen(MENU_SCREEN);
  }
  return 0;
}

static int
update(void) {
  return 0;
}

static int
focus(void) {
  enum {
    SCORE_CHARS_LIMIT = 30
  };

  char score_chars[SCORE_CHARS_LIMIT];
  TTF_Font *medium_font = get_medium_font();

  for (int i = 0; i < used_scores; i++) {
    destroy_text_image(score_texts + i);
    snprintf(score_chars, SCORE_CHARS_LIMIT, "%d", scores[i]);
    COND_PRET_LT0(init_text_image(score_texts + i, medium_font, score_chars,
      g_rend, &DEFAULT_FG_COLOR));
    score_texts[i].pos.x = screen_dim.w/2 - score_texts[i].dim.w/2;
    score_texts[i].pos.y = 2*LARGE_FONT_SIZE + PADDING_PX*(i+1) +
      i*MEDIUM_FONT_SIZE;
  }

  return 0;
}

static int
render(void) {
  COND_PRET_LT0(render_text_image(&title));
  COND_PRET_LT0(render_text_image(&exit_hint));

  for (int i = 0; i < used_scores; i++) {
    COND_PRET_LT0(render_text_image(score_texts + i));
  }

  return 0;
}

int
init_scores(SDL_Renderer *g_rend_, const PixelDim2D *screen_dim_) {
  g_rend = g_rend_;
  screen_dim = *screen_dim_;

  TTF_Font *small_font = get_small_font();
  TTF_Font *large_font = get_large_font();

  COND_PGOTO_LT0(
    init_text_image(&exit_hint, small_font, "Hit ESC to go back.", g_rend,
      &DEFAULT_FG_COLOR),
    e_cleanup);
  exit_hint.pos.x = screen_dim.w - exit_hint.dim.w;
  exit_hint.pos.y = screen_dim.h - exit_hint.dim.h;

  COND_PGOTO_LT0(
    init_text_image(&title, large_font, "High Scores", g_rend,
      &DEFAULT_FG_COLOR),
    e_cleanup);
  title.pos.x = screen_dim.w/2 - title.dim.w/2;
  title.pos.y = LARGE_FONT_SIZE;

  const ScreenObject self = {
    .focus = focus,
    .render = render,
    .update = update,
    .handle_event = handle_event,
    .destroy = destroy
  };
  register_screen(SCORES_SCREEN, &self);

  return 0;

e_cleanup:
  destroy();
  return -1;
}

void
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
}
