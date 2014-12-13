#include <SDL2/SDL_mixer.h>

#include "xSDL.h"
#include "error.h"
#include "music.h"

static Mix_Music *music;

int
init_music(void) {
  COND_EGOTO_IF0(
    (Mix_Init(MIX_INIT_FLAC) & MIX_INIT_FLAC) == MIX_INIT_FLAC,
    e_cleanup, Mix_GetError());

  COND_EGOTO_LT0(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048),
    e_cleanup, Mix_GetError());

  music = Mix_LoadMUS("music.flac");
  COND_EGOTO_IF0(music, e_cleanup, Mix_GetError());

  Mix_VolumeMusic(30);
  return 0;

e_cleanup:
  destroy_music();
  return -1;
}

int
play_new(void) {
  /*
   * Yes, the SDL_mixer API is weird... FadeOutMusic returns 0 on failure (and
   * 1 on success), and FadeInMusic returns negative on failure (and 0 on
   * success).
   */

  if (Mix_PlayingMusic()) {
    COND_ERET_IF0(Mix_FadeOutMusic(1500), -1, Mix_GetError());
  }
  COND_ERET_LT0(Mix_FadeInMusic(music, -1, 1500), Mix_GetError());
  return 0;
}

void
destroy_music(void) {
  xMix_FreeMusic(&music);
  Mix_Quit();
}
