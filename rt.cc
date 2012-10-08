// this is just a ray tracing demo to start with
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include "palette.h"

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
    vector3f k = p*(d*.02) + v3(frame_*0.05, 0, frame_*0.07);
    //printf("%g,%g,%g, %d,%d\n", k.x, k.y, k.z, lrint(k.x), lrint(k.z));
    *m = ((lrint(k.x)&1)^(lrint(k.z)&1));
    *n = v3(0,1,0);
  }
  vector3f c = v3(5.0f*cos(frame_*0.03), 0, 30+10.0f*sin(frame_*0.01));
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
  vector3f lcol = {0.7,0.5,0.5};
  float mshiny[] = {10,10,10};
  vector3f mcol[] = {
    {0.0f, 1.6f, 0.0f},
    {0.0f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.0f}};
  for(;;) {
    vector3f light = normalize(v3(0.2f*sin(frame_*0.01), 1, -1));
    for(y=0;y<24;y++) {
      for(x=0;x<80;x++) {
        vector3f color = v3(0,0,0);
        for(float xx = -0.25;xx<=0.25;xx+=0.5) { // 2 x samples
          for(float yy = -0.75;yy<=0.75;yy+=0.5) { // 4 y samples
            vector3f p = normalize(v3(x-40.0f+xx,25.0f-2.0f*y+yy,40.0f)), n;
            int m = -1;
            float d = dist(p, &m, &n);
            //printf("%d %d %g %g %g %g %g %g %d\n", x, y, xx, yy, p.x, p.y, p.z, d, m);
            if (m != -1) {
              float l = light*n;
              //printf("m=%d l=%g shiny=%g\n", m, l, pow(l, mshiny[m]));
              if (l > 0) color = color + mcol[m]*l + lcol*pow(l, mshiny[m]);
            }
          }
        }
        int fg = nearest(color * 0.125, -48+4*bayer[(x&3) + (y&3)*4]);
        //printf("%g %g %g -> %d\n", color.x, color.y, color.z, fg);
        if(fg>=0) printf("\x1b[%d;%dm#", !!(fg&8), 30+(fg&7));
        else putchar(32);
      }
      printf("\x1b[0m\n");
    }
    fflush(stdout);
    usleep(20000);
    frame_ += 1;
    printf("\x1b[24A");
  }
}

#if 0
      int bg = 0;
      if(bg)
        printf("\x1b[%d;%d;%dm#", !!(fg&8), 40+bg, 30+(fg&7));
      else
#endif
