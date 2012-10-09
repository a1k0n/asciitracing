#include "vec3.h"

// compute dithered nearest color fg/bg for pixel location x,y
void nearestcolor(const vec3& color, int x, int y, int *fg, int *bg);
static inline void printcolor(int fg, int bg) {
#if 0
  // not using bgs for now
  if(bg >= 0) printf("\x1b[%d;%d;%dm#", !!(fg&8), 40+(bg&7), 30+(fg&7));
  else if(fg >= 0) printf("\x1b[0;%d;%dm#", !!(fg&8), 30+(fg&7));
  else printf("\x1b[0m ");
#else
  if(fg >= 0) printf("\x1b[%d;%dm#", !!(fg&8), 30+(fg&7));
  else printf(" ");
#endif
}

