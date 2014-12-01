#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "2D.h"
#include "assets.h"
#include "error.h"
#include "text_image.h"
#include "screens.h"
#include "menu.h"
#include "game.h"
#include "scores.h"
#include "music.h"
#include "xassert.h"

enum {
  WIN_WIDTH = 540,
  WIN_HEIGHT = 640
};

static const char *WIN_TITLE = "Tetris";

static SDL_Window *window;
static struct GameContext gx;
static const struct ScreenObject *all_screens[NUM_SCREENS];
static const struct ScreenObject *current;

static void
report_error(void) {
  if (errorFn) {
    fprintf(stderr, "Error: %s\n", errorFn());
  }
  else {
    fprintf(stderr, "Error signalled, but errorFn is null.\n");
  }
}

static int
init_video(void) {
  window = SDL_CreateWindow(WIN_TITLE, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
  COND_ERROR_SET(window, e_bad_window, SDL_GetError);

  gx.r = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  COND_ERROR_SET(gx.r, e_bad_renderer, SDL_GetError);

  return 0;

e_bad_renderer:
  SDL_DestroyWindow(window);
  window = 0;
e_bad_window:
  return -1;
}

static int
init_screens(void) {
  COND_ERROR(init_menu(&gx) == 0, e_bad_menu);
  COND_ERROR(init_game(&gx) == 0, e_bad_game);
  COND_ERROR(init_scores(&gx) == 0, e_bad_scores);

  current = all_screens[MENU_SCREEN];
  return 0;

e_bad_scores:
  all_screens[GAME_SCREEN]->destroy(&gx);
e_bad_game:
  all_screens[MENU_SCREEN]->destroy(&gx);
e_bad_menu:
  return -1;
}

static void
destroy_video(void) {
  SDL_DestroyRenderer(gx.r);
  SDL_DestroyWindow(window);
}

static int
init(void) {
  gx.dim.w = WIN_WIDTH;
  gx.dim.h = WIN_HEIGHT;

  COND_ERROR_SET(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) == 0, e_bad_sdl, 
    SDL_GetError);
  COND_ERROR(init_video() == 0, e_bad_video);
  COND_ERROR_SET(TTF_Init() == 0, e_bad_ttf, TTF_GetError);
  COND_ERROR_SET((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == IMG_INIT_PNG,
    e_bad_img, IMG_GetError);
  COND_ERROR(init_assets(gx.r) == 0, e_bad_assets);
  COND_ERROR(init_music() == 0, e_bad_music);
  COND_ERROR(init_screens() == 0, e_bad_screens);
  
  return 0;

e_bad_screens:
  destroy_music();
e_bad_music:
  destroy_assets();
e_bad_assets:
  IMG_Quit();
e_bad_img:
  TTF_Quit();
e_bad_ttf:
  destroy_video();
e_bad_video:
  SDL_Quit();
e_bad_sdl:
  return -1;
}

static void
cleanup(void) {
  int i;

  for (i = 0; i < NUM_SCREENS; i++) {
    if (all_screens[i]->destroy(&gx) < 0) {
      fprintf(stderr, "Error while destroying screen: %d.\n", i);
      report_error();
    }
  }
  destroy_video();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}

static int
render(void) {
  SDL_Texture *bg = get_bg_img();
  SDL_RenderCopy(gx.r, bg, 0, 0);
  int err = current->render(&gx);
  if (err < 0) {
    return err;
  }
  SDL_RenderPresent(gx.r);
  return 0;
}

static int
game_loop(void) {
  SDL_Event e;
  
  xassert(current);
  
  current->focus(&gx);
  play_new();
  
  enum ScreenId s = SELF;
  int err = 0;
  
  for (;;) {
    // Events
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        return 0;
      }
      s = current->handle_event(&gx, &e);
      if (s == ERROR) {
        return -1;
      }
      else if (s != SELF) {
        break;
      }
    }
    
    // Screen Update
    if (s == SELF) {
      s = current->update(&gx);
      if (s == ERROR) {
        return -1;
      }
    }
    
    // Screen Change?
    if (s != SELF) {
      xassert(s < (int)NUM_SCREENS);
      xassert(s >= 0);
      current = all_screens[s];
      err = current->focus(&gx);
      if (err < 0) {
        return err;
      }
      s = SELF;
      play_new();
    }
    
    // Render.
    err = render();
    if (err < 0) {
      return err;
    }
  }
  return 0;
}

void
register_screen(enum ScreenId which, const struct ScreenObject *screen) {
  all_screens[which] = screen;
}

int
main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  
  if (init() < 0) {
    report_error();
    return 1;
  }
  else if (game_loop() < 0) {
    report_error();
    cleanup();
    return 1;
  }
  else {
    cleanup();
    return 0;
  }
}
