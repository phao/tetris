#include <SDL2/SDL_mixer.h>

#include "error.h"

static Mix_Music *music;

int
init_music(void) {
  COND_ERROR_SET((Mix_Init(MIX_INIT_FLAC) & MIX_INIT_FLAC) != 0,
    e_bad_mix, Mix_GetError);
    
  COND_ERROR_SET(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == 0,
    e_bad_open_audio, Mix_GetError);
    
  music = Mix_LoadMUS("music.flac");
  COND_ERROR_SET(music, e_bad_load_music, Mix_GetError);
  
  Mix_VolumeMusic(30);
  
  return 0;

e_bad_load_music:
  // Nothing to do here.
e_bad_open_audio:
  Mix_Quit();
e_bad_mix:
  return -1;
}

int
play_new(void) {
  // Yes, the SDL_mixer API is weird... FadeOutMusic returns 0 on failure (and
  // 1 on success), and FadeInMusic returns -1 on failure (and 0 on success).
  if (Mix_PlayingMusic()) {
    if (!Mix_FadeOutMusic(1500)) {
      return -1;
    }
  }
  if (Mix_FadeInMusic(music, -1, 1500) < 0) {
    return 0;
  }
  return -1;
}

int
destroy_music(void) {
  Mix_FreeMusic(music);
  Mix_Quit();
  return 0;
}