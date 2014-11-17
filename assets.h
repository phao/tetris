#ifdef TESTING_HEADER_INCLUDES
#include <SDL2/SDL_ttf.h>
#endif

static const struct SDL_Color DEFAULT_BG_COLOR = {25, 25, 25, 255};
static const struct SDL_Color DEFAULT_FG_COLOR = {225, 225, 225, 255};
static const int LARGE_FONT_SIZE = 60;
static const int MEDIUM_FONT_SIZE = 42;

int
init_assets(void);

TTF_Font*
get_large_font(void);

TTF_Font*
get_medium_font(void);

void
destroy_assets(void);
