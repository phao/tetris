#include <assert.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef const char *(*ErrorFn)(void);

enum Screen {
  MENU, GAME, SCORES
};

struct Dim2D {
  int w, h;
};

struct Video {
  struct Dim2D dim;
  SDL_Window *window;
  SDL_Renderer *renderer;
};

struct TextImage {
  int w, h;
  SDL_Texture *image;
};

struct MenuScreen {
  struct Dim2D within;
  struct TextImage title, new_game, scores;
};

struct Tetris {
  struct Video video;
  struct MenuScreen menu;
};

const struct SDL_Color DEFAULT_BG_COLOR = {25, 25, 25, 255};
const struct SDL_Color DEFAULT_FG_COLOR = {225, 225, 225, 255};

ErrorFn errorFn;

// If (COND) is false-ish, then set errorFn to (FN) and goto (GOTO)
#define COND_ERROR_SET(COND, GOTO, FN) if (!(COND)) { \
  errorFn = (FN); goto GOTO; }

// IF (COND) is false-ish, then goto (GOTO)
#define COND_ERROR(COND, GOTO) if (!(COND)) { goto GOTO; }

int
init_video(struct Video *v, const char *title) {
  assert(title;

  v->window = v->renderer = 0;

  v->window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, v->dim.w, v->dim.h, SDL_WINDOW_SHOWN);
  COND_ERROR_SET(v->window, e_bad_window, SDL_GetError);

  v->renderer = SDL_CreateRenderer(v->window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  COND_ERROR_SET(v->renderer, e_bad_renderer, SDL_GetError);

  assert(v->window);
  assert(v->renderer);

  return 0;

e_bad_renderer:
  SDL_DestroyWindow(v->window);
  v->window = 0;
e_bad_window:
  return -1;
}

void
close_video(struct Video *v) {
  SDL_DestroyRenderer(v->renderer);
  SDL_DestroyWindow(v->window);
}

int
init_menu_text(struct TextImage *title,
               TTF_Font *font,
               const char *text,
               SDL_Renderer *r)
{
  SDL_Surface *stext;

  stext = TTF_RenderText_Solid(font, text, DEFAULT_FG_COLOR);
  if (!stext) {
    errorFn = TTF_GetError;
    return -1;
  }
  title->image = SDL_CreateTextureFromSurface(r, stext);
  if (!title->image) {
    errorFn = SDL_GetError;
    SDL_FreeSurface(stext);
    return -1;
  }
  title->w = stext->w;
  title->h = stext->h;
  SDL_FreeSurface(stext);
  return 0;
}

void
destroy_menu_text(struct TextImage *t) {
  SDL_DestroyTexture(t->image);
}

int
init_menu_screen(struct MenuScreen *m, SDL_Renderer *r) {
  const char *FONT_FILE = "arcade.ttf";
  const int TITLE_FONT_SIZE = 60;
  const int BUTTON_FONT_SIZE = 48;

  TTF_Font *title_font, *button_font;

  title_font = TTF_OpenFont(FONT_FILE, TITLE_FONT_SIZE);
  COND_ERROR_SET(title_font, e_bad_title_font, TTF_GetError);

  button_font = TTF_OpenFont(FONT_FILE, BUTTON_FONT_SIZE);
  COND_ERROR_SET(button_font, e_bad_button_font, TTF_GetError);

  COND_ERROR(init_menu_text(&m->title, title_font, "Tetris", r) >= 0,
    e_bad_title_text);

  COND_ERROR(init_menu_text(&m->new_game, button_font, "New Game", r) >= 0,
    e_bad_new_game_text);

  COND_ERROR(init_menu_text(&m->new_game, button_font, "High Scores", r) >= 0,
    e_bad_scores_text);

  return 0;

e_bad_scores_text:
  destroy_menu_text(&m->new_game);
e_bad_new_game_text:
  destroy_menu_text(&m->title);
e_bad_title_text:
  TTF_CloseFont(button_font);
e_bad_button_font:
  TTF_CloseFont(title_font);
e_bad_title_font:
  return -1;
}

int
init_game(struct Tetris *g, const char *title) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    errorFn = SDL_GetError;
    return -1;
  }
  if (init_video(&g->video, title) < 0) {
    SDL_Quit();
    return -1;
  }
  if (TTF_Init() < 0) {
    close_video(&g->video);
    SDL_Quit();
    errorFn = TTF_GetError;
    return -1;
  }
  g->menu.within = g->video.dim;
  init_menu_screen(&g->menu, g->video.renderer);
  return 0;
}

void
close_all(struct Video *v) {
  close_video(v);
  TTF_Quit();
  SDL_Quit();
}

void
menu_screen(struct MenuScreen *m) {
}

void
screens_loop(struct Tetris *g) {
  menu_screen(&g->menu);
}

int
run_game(struct Tetris *g, const char *title) {
  if (init_game(g, title) < 0) {
    return -1;
  }
  screens_loop(g);
  close_all(&g->video);
  return 0;
}

int
main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  const int WIN_WIDTH = 640;
  const int WIN_HEIGHT = 640;
  const char *WIN_TITLE = "Tetris";

  struct Tetris tetris;

  tetris.video.dim = (struct Dim2D) {WIN_WIDTH, WIN_HEIGHT};
  if (run_game(&tetris, WIN_TITLE) < 0) {
    fprintf(stderr, "Error: %s\n", errorFn());
    return 1;
  }
  return 0;
}

