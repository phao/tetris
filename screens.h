#ifdef TESTING_HEADER_INCLUDES
#include <SDL2/SDL.h>
#endif

/*
 * A screen is supposed to support some operations through functions it
 * defines:
 *
 * - init
 * - destroy
 * - handle event
 * - update
 * - render
 * - focus
 *
 * For all screens, their init functions will be called in the beginning of
 * the game. Their destroy functions will be called at the end of the game.
 *
 * When a window is activated, it's focus function will be called. Activating
 * a window happens in the beginning of the game, when the first initial
 * window is shown. It also happens when a window gives out control to another
 * window.
 *
 * A window hands over control to another window through the update or
 * handle event function. These functions should return a value in the enum
 * Screen type, indicating what should happens next.
 *
 * - If a screen update or handle event function returns SELF, it means the
 * control should continue with them.
 * - On the case of anything else, which indicates another window, control
 * is handed over to that window. It's focus function will be called and then
 * it'll be in control. If a window returns a enum Screen value that indicates
 * itself, nothing different happens (its focus will still be called, and so
 * forth). You can use such mechanism as some sort of reset.
 *
 */

enum ScreenId {
  MENU_SCREEN, GAME_SCREEN, SCORES_SCREEN, SELF
};

enum {
  NUM_SCREENS = 3
};

typedef void (*ScreenDestroyFn)(void);
typedef enum ScreenId (*ScreenHandleEventFn)(const SDL_Event *e);
typedef enum ScreenId (*ScreenUpdateFn)(void);
typedef void (*ScreenRenderFn)(SDL_Renderer *r);
typedef void (*ScreenFocusFn)(void);

struct ScreenObject {
  ScreenDestroyFn destroy;
  ScreenHandleEventFn handle_event;
  ScreenUpdateFn update;
  ScreenRenderFn render;
  ScreenFocusFn focus;
};

extern struct ScreenObject AllScreens[NUM_SCREENS];
