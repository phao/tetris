#ifdef TESTING_HEADER_INCLUDES
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "2D.h"
#endif

struct TextImage {
  struct Dim2D dim;
  struct Point2D pos;
  SDL_Texture *image;
};

void
render_text_image(SDL_Renderer *r, const struct TextImage *t);

/**
 * Sets up a text image for you. It'll do the necessary steps to get a texture
 * in ti->image. You're responsible for destroying that texture with
 * SDL_DestroyTexture before destroying the renderer.
 *
 * It'll also initialize the dimension values. The position, though, is for the
 * user to setup.
 */
int
init_text_image(struct TextImage *ti,
                TTF_Font *font,
                const char *text,
                SDL_Renderer *r);

int
in_bounds(int x, int y, const struct TextImage *ti);
