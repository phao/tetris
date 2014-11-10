#ifdef TESTING_HEADER_INCLUDES
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

typedef const char *(*ErrorFn)(void);

enum Screen {
  NONE, SAME_SCREEN, MENU_SCREEN, GAME_SCREEN, SCORE_SCREEN
};

struct Dim2D {
  int w, h;
};

struct TextImage {
  struct Dim2D dim;
  SDL_Texture *image;
};

extern const char *FONT_FILE;

extern const int LARGE_FONT_SIZE;
extern const int MEDIUM_FONT_SIZE;
extern const int SMALL_FONT_SIZE;

extern const struct SDL_Color DEFAULT_BG_COLOR;
extern const struct SDL_Color DEFAULT_FG_COLOR;

extern ErrorFn errorFn;

/**
 * If (COND) is false-ish, then set errorFn to (FN) and goto (GOTO).
 */
#define COND_ERROR_SET(COND, GOTO, FN) if (!(COND)) { \
  errorFn = (FN); goto GOTO; }

/**
 * IF (COND) is false-ish, then goto (GOTO).
 */
#define COND_ERROR(COND, GOTO) if (!(COND)) { goto GOTO; }

/**
 * Computes the x position that *thing would have to be in order to be
 * rendered in the horizontal center of *within.
 */
int
hor_center_within(const struct Dim2D *thing,
                  const struct Dim2D *within);

/**
 * Sets up a text image for you. It'll do the necessary steps to get a texture
 * in it->image. You're responsible for destroying that texture with
 * SDL_DestroyTexture before destroying the renderer.
 *
 * It'll also initialize the dimension values.
 */
int
init_text_image(struct TextImage *it,
                TTF_Font *font,
                const char *text,
                SDL_Renderer *r);

void
render_text_image(SDL_Renderer *r,
                  const struct TextImage *t,
                  int x,
                  int y);
