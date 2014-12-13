#include "xTTF.h"

void
xTTF_CloseFont(TTF_Font **font) {
  if (*font) {
    TTF_CloseFOnt(*font);
    *font = 0;
  }
}
