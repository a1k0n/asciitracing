#include "palette.h"
#include "vec3.h"

const int bayer_matrix[] = {
   1, 9, 3,11,
  13, 5,15, 7,
   4,12, 2,10,
  16, 8,14, 6};

// nearest color fg/bg, dithering offset o
void nearestcolor(const vec3& c, int x, int y, int *fg, int *bg) {
  // brute force
  float minerr = 65536*30;
  int o = -48+4*bayer_matrix[(x&3) + (y&3)*4];
  *fg = -1; *bg = -1;
  for(int i = 0; i < 17; i++) {
#if 0
    for(int j = 0; j < 9; j++) {
      float er = 0.4*palette_[j*3] + 0.6*palette_[i*3] + o - 255*c.x;
      float eg = 0.4*palette_[j*3+1] + 0.6*palette_[i*3+1] + o - 255*c.y;
      float eb = 0.4*palette_[j*3+2] + 0.6*palette_[i*3+2] + o - 255*c.z;
      float vr = palette_[j*3] - palette_[i*3];
      float vg = palette_[j*3+1] - palette_[i*3+1];
      float vb = palette_[j*3+2] - palette_[i*3+2];
      int err = er*er + eg*eg + eb*eb + 0.3*(vr*vr + vg*vg + vb*vg);
      if (err < minerr) { minerr = err; *fg = i-1; *bg = j-1; }
#else
    float er = palette_[i*3] + o - 255*c.x;
    float eg = palette_[i*3+1] + o - 255*c.y;
    float eb = palette_[i*3+2] + o - 255*c.z;
    int err = er*er + eg*eg + eb*eb;
#endif
#if 0
      float ey = 0.257*er + 0.504*eg + 0.098*eb;
      float ev = 0.439*er - 0.368*eg - 0.071*eb;
      float eu = -0.148*er - 0.291*eg + 0.439*eb;
      int err = ey*ey + 2*ev*ev + 2*eu*eu;
#endif
      if (err < minerr) { minerr = err; *fg = i-1; }
#if 0
    }
#endif
  }
  //printf("minerr=%d besti=%d c=%g,%g,%g\n", minerr, besti, c.x, c.y, c.z);
}

