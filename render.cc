#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "palette.h"
// ascii art sphere tracing
// with dithering
// we need to assume a certain palette i guess, probably should be optimized for
// goobuntu no?

float frame_ = 0;

struct vector3f { float x,y,z; };
vector3f v3(float a, float b, float c) { return vector3f{a,b,c}; }
vector3f operator+(const vector3f &a, const vector3f &b) {
  return {a.x+b.x,a.y+b.y,a.z+b.z};
}
vector3f operator-(const vector3f &a, const vector3f &b) {
  return {a.x-b.x,a.y-b.y,a.z-b.z};
}
vector3f operator*(const vector3f &a, float b) {
  return {a.x*b,a.y*b,a.z*b};
}
float operator*(const vector3f &a, const vector3f &b) {
  return a.x*b.x+a.y*b.y+a.z*b.z;
}
vector3f normalize(const vector3f &v) {
  return v*(1.0/sqrt(v*v));
}

// returns distance to scene, material m, and normal n
float dist(const vector3f &p, int *m, vector3f *n) {
  *m = -1;
  float d = 1e30;
  // checkerboard on the bottom
  if (p.y < 0) {
    d = -40/p.y;
    vector3f k = p*(d*.02) + v3(frame_*0.01, 0, frame_*0.02);
    //printf("%g,%g,%g, %d,%d\n", k.x, k.y, k.z, lrint(k.x), lrint(k.z));
    *m = ((lrint(k.x)&1)^(lrint(k.z)&1));
    *n = v3(0,1,0);
  }
  vector3f c = v3(5.0f*cos(frame_*0.03), 0, 50+10.0f*sin(frame_*0.01));
  float r2 = 15*15;
  float cp = c*p;
  float p2 = p*p;
  float disc = cp*cp - p2*(c*c - r2);
  if (disc > 0) {
    float t = (cp - sqrt(disc))/p2;
    if (t < d) {
      d = t;
      *m = 2;
      *n = normalize(p*t - c);
    }
  }
  return d;
}

// nearest color, offset o
// default gnome-terminal color scheme Tango:
// #2E2E34343636:#CCCC00000000:#4E4E9A9A0606:#C4C4A0A00000:
// #34346565A4A4:#757550507B7B:#060698209A9A:#D3D3D7D7CFCF:
// #555557575353:#EFEF29292929:#8A8AE2E23434:#FCFCE9E94F4F:
// #72729F9FCFCF:#ADAD7F7FA8A8:#3434E2E2E2E2:#EEEEEEEEECEC
int nearest(vector3f c, int o) {
  // brute force
  float minerr = 65536*3;
  int besti = 0;
  for(int i = 0; i < 17; i++) {
    float er = palette_[i*3] + o - 255*c.x;
    float eg = palette_[i*3+1] + o - 255*c.y;
    float eb = palette_[i*3+2] + o - 255*c.z;
#if 0
    float ey = 0.257*er + 0.504*eg + 0.098*eb;
    float ev = 0.439*er - 0.368*eg - 0.071*eb;
    float eu = -0.148*er - 0.291*eg + 0.439*eb;
    int err = ey*ey + 2*ev*ev + 2*eu*eu;
#else
    int err = er*er + eg*eg + eb*eb;
#endif
    if (err < minerr) { minerr = err; besti = i-1; }
  }
  //printf("minerr=%d besti=%d c=%g,%g,%g\n", minerr, besti, c.x, c.y, c.z);
  return besti;
}

// ordered dithering Bayer matrix:
const int bayer[] = {1,9,3,11,13,5,15,7,4,12,2,10,16,8,14,6};
// use bayer[i&3 + (j&3)*4] / 17
int main()
{
  int x,y;
  for(y=0;y<24;y++) {
    for(x=0;x<80;x++) {
      vector3f color = v3(y/23.0,x/79.0,0);
      int fg = nearest(color, -48+6*bayer[(x&3) + (y&3)*4]);
      //printf("%g %g %g -> %d\n", color.x, color.y, color.z, fg);
      if(fg>=0) printf("\x1b[%d;%dm#", !!(fg&8), 30+(fg&7));
      else putchar(32);
    }
    printf("\x1b[0m\n");
  }
}

#if 0
      int bg = 0;
      if(bg)
        printf("\x1b[%d;%d;%dm#", !!(fg&8), 40+bg, 30+(fg&7));
      else
#endif
