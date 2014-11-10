#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "commons.h"
#include "menu.h"

struct Video {
  SDL_Window *window;
  SDL_Renderer *renderer;
};

static const int WIN_WIDTH = 640;
static const int WIN_HEIGHT = 640;
static const char *WIN_TITLE = "Tetris";

static struct Video VIDEO;

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
  return init_menu(VIDEO.renderer, &dim);
}

static void
destroy_video(void) {
  SDL_DestroyRenderer(VIDEO.renderer);
  SDL_DestroyWindow(VIDEO.window);
}

static int
init_game(void) {
  COND_ERROR_SET(SDL_Init(SDL_INIT_VIDEO) == 0, e_bad_sdl, SDL_GetError);
  COND_ERROR(init_video() == 0, e_bad_video);
  COND_ERROR_SET(TTF_Init() == 0, e_bad_ttf, TTF_GetError);
  COND_ERROR(init_screens() == 0, e_bad_screens);
  return 0;

e_bad_screens:
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
  destroy_menu();
  destroy_video();
  TTF_Quit();
  SDL_Quit();
}

static void
game_loop(void) {
  SDL_Event e;
  enum Screen screen;

  for (;;) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        return;
      }
      screen = send_menu_event(&e);
      if (screen == NONE) {
        return;
      }
    }
    render_menu(VIDEO.renderer);
  }
}

int
main(int argc, char *argv[]) {
  (void) argc;
  (void) argv;

  if (init_game() < 0) {
    fprintf(stderr, "Error: %s\n", errorFn());
    return 1;
  }
  game_loop();
  cleanup();
  return 0;
}

