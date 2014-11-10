#ifdef TESTING_HEADER_INCLUDES
#include <SDL2/SDL.h>
#include "commons.h"
#endif

int
init_menu(SDL_Renderer *r, const struct Dim2D *within);

void
render_menu(SDL_Renderer *r);

void
destroy_menu(void);

enum Screen
send_menu_event(const SDL_Event *e);

