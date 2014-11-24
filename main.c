#include <stdio.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "2D.h"
#include "assets.h"
#include "error.h"
#include "text_image.h"
#include "menu.h"
#include "screens.h"
#include "game.h"

struct Video {
  SDL_Window *window;
  SDL_Renderer *renderer;
};

static const int WIN_WIDTH = 540;
static const int WIN_HEIGHT = 640;
static const char *WIN_TITLE = "Tetris";

static struct Video VIDEO;

struct ScreenObject AllScreens[NUM_SCREENS];
static struct ScreenObject *current;

static int
init_video(void) {
  VIDEO.window = SDL_CreateWindow(WIN_TITLE, SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
  COND_ERROR_SET(VIDEO.window, e_bad_window, SDL_GetError);

  VIDEO.renderer = SDL_CreateRenderer(VIDEO.window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  COND_ERROR_SET(VIDEO.renderer, e_bad_renderer, SDL_GetError);

  return 0;

e_bad_renderer:
  SDL_DestroyWindow(VIDEO.window);
e_bad_window:
  return -1;
}

static int
init_screens(void) {
  struct Dim2D dim;

  dim.w = WIN_WIDTH;
  dim.h = WIN_HEIGHT;

  COND_ERROR(init_menu(VIDEO.renderer, &dim) == 0, e_bad_menu);
  COND_ERROR(init_game(VIDEO.renderer, &dim) == 0, e_bad_game);

  current = AllScreens + MENU_SCREEN;
  return 0;

e_bad_game:
  AllScreens[MENU_SCREEN].destroy();
e_bad_menu:
  return -1;
}

static void
destroy_video(void) {
  SDL_DestroyRenderer(VIDEO.renderer);
  SDL_DestroyWindow(VIDEO.window);
}

static int
init(void) {
  int i;

  for (i = 0; i < NUM_SCREENS; i++) {
    AllScreens[i] = (struct ScreenObject) {0, 0, 0, 0, 0};
  }

  COND_ERROR_SET(SDL_Init(SDL_INIT_VIDEO) == 0, e_bad_sdl, SDL_GetError);
  COND_ERROR(init_video() == 0, e_bad_video);
  COND_ERROR_SET(TTF_Init() == 0, e_bad_ttf, TTF_GetError);
  COND_ERROR(init_assets(VIDEO.renderer) == 0, e_bad_assets);
  
  COND_ERROR(init_screens() == 0, e_bad_screens);
  
  return 0;

e_bad_screens:
  destroy_assets();
e_bad_assets:
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
    // This conditional is only for during development.
    if (AllScreens[i].destroy) {
      AllScreens[i].destroy();
    }
  }
  destroy_video();
  TTF_Quit();
  SDL_Quit();
}

static void
render(void) {
  SDL_Texture *bg = get_bg_img();
  SDL_Renderer *r = VIDEO.renderer;
  SDL_RenderCopy(r, bg, 0, 0);
  current->render(r);
  SDL_RenderPresent(r);
}

static void
game_loop(void) {
  SDL_Event e;

  assert(current);
  current->focus();
  enum ScreenId s = SELF;
  for (;;) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        return;
      }
      s = current->handle_event(&e);
      if (s != SELF) {
        break;
      }
    }
    if (s == SELF) {
      s = current->update();
    }
    if (s != SELF) {
      assert(s < (int)NUM_SCREENS);
      assert(s >= 0);
      current = AllScreens + s;
      current->focus();
      s = SELF;
    }
    render();
  }
}

int
main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;
  
  if (init() < 0) {
    if (errorFn) {
      fprintf(stderr, "Error: %s\n", errorFn());
    }
    else {
      fprintf(stderr, "Error signalled, but errorFn is null.\n");
    }
    return 1;
  }
  game_loop();
  cleanup();
  return 0;
}
