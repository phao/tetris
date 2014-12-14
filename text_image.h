#ifndef TEXT_IMAGE_H
#define TEXT_IMAGE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "2D.h"

struct TextImage {
  PixelDim2D dim;
  PixelPoint2D pos;
  SDL_Texture *image;
  SDL_Renderer *rend;
};

int
render_text_image(struct TextImage *t);

/**
 * Sets up a text image for you. It'll do the necessary steps to get a texture
 * in ti->image. You're responsible for destroying that texture with
 * SDL_DestroyTexture before destroying the renderer.
 *
 * It'll also initialize the dimension values. The position, though, is for the
 * user to setup. This function will zero the position.
 */
int
init_text_image(struct TextImage *ti,
                TTF_Font *font,
                const char *text,
                SDL_Renderer *rend,
                const SDL_Color *color);

int
in_bounds(const struct TextImage *ti, int x, int y);

void
destroy_text_image(struct TextImage *ti);

#endif
