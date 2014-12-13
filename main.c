#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "2D.h"
#include "assets.h"
#include "error.h"
#include "text_image.h"
#include "screens.h"
#include "menu.h"
#include "game.h"
#include "scores.h"
#include "music.h"

#include "xSDL.h"

enum {
  WIN_WIDTH = 540,
  WIN_HEIGHT = 640
};

static const char *WIN_TITLE = "Tetris";

static SDL_Window *window;
static SDL_Renderer *rend;
static PixelDim2D screen_size;
static ScreenObject all_screens[NUM_SCREENS];
static ScreenObject *current;

static int
init_video(void) {
  window = SDL_CreateWindow(WIN_TITLE, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
  COND_ERET_IF0(window, -1, SDL_GetError());

  rend = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  COND_ERET_IF0(rend, -1, SDL_GetError());

  return 0;
}

static int
init_screens(void) {
  screen_size.w = WIN_WIDTH;
  screen_size.h = WIN_HEIGHT;

  COND_PRET_LT0(init_menu(rend, &screen_size));
  COND_PRET_LT0(init_game(rend, &screen_size));
  COND_PRET_LT0(init_scores(rend, &screen_size));
  current = all_screens + MENU_SCREEN;
  return 0;
}

static int
init(void) {
  COND_ERET_LT0(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO), SDL_GetError());
  COND_PRET_LT0(init_video());
  COND_ERET_LT0(TTF_Init(), TTF_GetError());
  COND_ERET_IF0((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == IMG_INIT_PNG, -1,
    IMG_GetError());
  COND_PRET_LT0(init_assets(rend));
  COND_PRET_LT0(init_music());
  COND_PRET_LT0(init_screens());

  return 0;
}

static int
game_loop(void) {
  SDL_assert(current);
  play_new();
  COND_PRET_LT0(current->focus());
  SDL_Texture *bg = get_bg_img();

  for (;;) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        return 0;
      }
      COND_PRET_LT0(current->handle_event(&e));
    }
    COND_PRET_LT0(current->update());
    COND_ERET_LT0(SDL_RenderCopy(rend, bg, 0, 0), SDL_GetError());
    COND_PRET_LT0(current->render());
    SDL_RenderPresent(rend);
  }

  return 0;
}

static const char *
fmt_message(const char *msg, const char *alt) {
  return msg ? (*msg ? msg : alt) : alt;
}

static void
report_error(void) {
  ErrorInfo *err = get_error();
  const char *fmt = "\t%s: %s: L%d: %s\n"
    "\t\t%s\n";
  fputs("Error! Aborting program.\n"
         "Bread Crumbs:\n"
         "=============\n", stderr);
  for (ErrorInfo *p = err; p; p = p->next) {
    fprintf(stderr, fmt,
      fmt_message(p->file_name.data, "(missing file name)"),
      fmt_message(p->func_name.data, "(missing function name)"),
      p->line,
      fmt_message(p->msg.data, "(missing message)"),
      fmt_message(p->code.data, "(missing code)"));
  }
  fmt = "\n\tSDL_GetError() => %s\n"
    "\tIMG_GetError() => %s\n"
    "\tTTF_GetError() => %s\n"
    "\tMix_GetError() => %s\n";
  fprintf(stderr, fmt,
    fmt_message(SDL_GetError(), "(empty)"),
    fmt_message(IMG_GetError(), "(empty)"),
    fmt_message(TTF_GetError(), "(empty)"),
    fmt_message(Mix_GetError(), "(empty)"));
  if (err) {
    free_error(err);
  }
}

static void
cleanup(void) {
  for (int i = 0; i < NUM_SCREENS; i++) {
    if (all_screens[i].destroy) {
      all_screens[i].destroy();
    }
  }
  xSDL_DestroyRenderer(&rend);
  xSDL_DestroyWindow(&window);
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

static void
error_quit() {
  report_error();
  cleanup();
  free_error(0);
  exit(EXIT_FAILURE);
}

void
register_screen(const ScreenId which, const ScreenObject *screen) {
  const int screen_i = (int) which;
  SDL_assert(screen_i >= 0);
  SDL_assert(screen_i < NUM_SCREENS);
  all_screens[screen_i] = *screen;
}

void
change_screen(const ScreenId which) {
  const int screen_i = (int) which;
  SDL_assert(screen_i >= 0);
  SDL_assert(screen_i < NUM_SCREENS);
  current = all_screens + screen_i;
  if (which == GAME_SCREEN) {
    play_new();
  }
  COND_PGOTO_LT0(current->focus(), err);
  return;

err:
  error_quit();
}

int
main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  COND_PGOTO_LT0(init(), err);
  COND_PGOTO_LT0(game_loop(), err);
  cleanup();
  return 0;

err:
  error_quit();
}
