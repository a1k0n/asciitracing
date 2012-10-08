// this is just a ray tracing demo to start with
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <algorithm>
#include "palette.h"
#include "vec3.h"

float frame_ = 0;

vec3 lcol(0.7,0.5,0.5);
float mshiny[] = {100,10,10,10};
vec3 mcol[] = {
  vec3(1.0f, 1.0f, 1.0f),
  vec3(0.0f, 0.0f, 1.0f),
  vec3(0.0f, 0.4f, 1.0f),
  vec3(0.2f, 1.0f, 0.0f)};

float pow8(float t) { t*=t; t*=t; t*=t; return t; }
float length8(float x, float y) {
  return pow(pow8(x) + pow8(y), 1.0/8);
}

float sdTorus88(const vec3& p, float s, float t)
{
  return length8(length8(p.x, p.z) - s, p.y) - t;
}

float udRoundBox(const vec3& p, const vec3& b, float r)
{
  return length(max(abs(p)-b,0.0))-r;
}

float sdSphere( vec3 p, float s )
{
  return length(p)-s;
}

// returns minimum distance to scene, material m, and normal n
float dist(const vec3 &p, int *m) {
  *m = -1;
  float d = 1e30;
  float dplane = p.y + 30;
  if (dplane < d) {
    //*m = ((lrint(p.x*0.03)&1)^(lrint(p.z*0.03)&1));
    *m = 0;
    d = dplane;
  }
  float douter = sdTorus88(p, 70, 15);
  if (douter < d) {
    *m = 2;
    d = douter;
  }
  float dinner = udRoundBox(p, vec3(20, 5, 20), 5);
  if (dinner < d) {
    *m = 2;
    d = dinner;
  }
#if 0
  //vec3 thingycenter = vec3(0, 40+20*sin(frame_*0.1), 0);
  vec3 q = vec3(fmod(fabs(p.x), 100) - 50, p.y - 40, p.z);
  //vec3 thingycenter = vec3(0, 40, 0);
  float dbox = udRoundBox(q, vec3(10, 20, 30), 8);
  float dsphere = sdSphere(q, 25);
  float dthingy = std::max(-dsphere, dbox);
  if (dthingy < d) {
    *m = 2;
    d = dthingy;
  }
#endif
#if 0
  float dsphere2 = sdSphere(p - vec3(0,40,80), 15);
  if (dsphere2 < d) {
    *m = 3;
    d = dsphere2;
  }
#endif
  return d;
}

// nearest color fg/bg, dithering offset o
void nearestcolor(vec3 c, int o, int *fg, int *bg) {
  // brute force
  float minerr = 65536*30;
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

float shadow(const vec3& ro, const vec3& rd, float mint, float maxt) {
  int m;
  float res = 1.0;
  for (float t = mint; t < maxt; ) {
    float h = dist(ro+rd*t, &m);
    if (h < 0.001) {
      //printf("h=%g hit\n", h);
      return 0;
    }
    res = std::min( res, 4.0f*h/t );
    //printf("res=%g, h=%g t=%g 2h/t=%g\n", res, h, t, 2.0f*h/t);
    t += h;
  }
  return res;
}

vec3 lighting(const vec3 &p, const vec3& n, int m, const vec3& lightpos) {
  vec3 lightdir = normalize(lightpos - p);
  float s = std::max(0.3f, shadow(p, lightdir, 0.01, length(p-lightpos)));
  float l = std::max(0.1f, std::max(-(lightdir*n), 0.0f) * s);
  return mcol[m]*l + lcol*pow(l, mshiny[m]);
}

// ordered dithering Bayer matrix:
const int bayer[] = {1,9,3,11,13,5,15,7,4,12,2,10,16,8,14,6};
// use bayer[i&3 + (j&3)*4] / 17
int main()
{
  int x,y;
  for(;;) {
    vec3 campos = vec3(100*sin(frame_*0.02), 120 + 40*sin(frame_*0.03), -100*cos(frame_*0.01));
    vec3 camz = normalize(campos*-1);
    //vec3 lightpos = vec3(200,400,0);
    vec3 lightpos = campos;
    //vec3 lightpos = vec3(100*sin(frame_*0.08), 50, -100*cos(frame_*0.04));
    //vec3 lightpos = vec3(50*sin(frame_*0.02), 50, 0);
    vec3 lightpos2 = vec3(0, 200, 0);
    vec3 camx = normalize(cross(camz, vec3(0,1,0)));
    vec3 camy = normalize(cross(camx, camz));
    //printf("camera: %g,%g,%g  (%g,%g,%g right)\n", campos.x, campos.y, campos.z, camx.x, camx.y, camx.z);
    for(y=0;y<24;y++) {
      for(x=0;x<80;x++) {
        vec3 color = vec3(0,0,0);
        int fg, bg;
#ifdef AA
        for(float xx = -0.25;xx<=0.25;xx+=0.5) { // 2 x samples
          for(float yy = -0.75;yy<=0.75;yy+=0.5) { // 4 y samples
            vec3 dir = normalize(vec3(x-40.0f+xx,25.0f-2.0f*y+yy,20.0f));
#else
            vec3 dir = normalize(vec3(x-40.0f,25.0f-2.0f*y,50.0f));
#endif
            dir = camx*dir.x + camy*dir.y + camz*dir.z;
            // ray = (0) + dir*t
            float t = 0;
            int m = -1;
            for (int iter = 0; iter < 64 && t < 1e6; iter++) {
              vec3 p = dir*t + campos;
              float d = dist(p, &m);
              if (d < 0.001) {
                // we hit something.  let's get a normal vector.
                vec3 n = normalize(vec3(d - dist(p+vec3(0.01,0,0), &m),
                                        d - dist(p+vec3(0,0.01,0), &m),
                                        d - dist(p+vec3(0,0,0.01), &m)));
                color = color + lighting(p, n, m, lightpos);
                //color = color + lighting(p, n, m, lightpos2)*0.2;
                break;
              }
              t += d;
            }
#ifdef AA
          }
        }
        nearestcolor(color * 0.125, -48+4*bayer[(x&3) + (y&3)*4], &fg, &bg);
#else
        nearestcolor(color, -48+4*bayer[(x&3) + (y&3)*4], &fg, &bg);
#endif
        //printf("%g %g %g -> %d\n", color.x, color.y, color.z, fg);
        if(bg >= 0) printf("\x1b[%d;%d;%dm#", !!(fg&8), 40+(bg&7), 30+(fg&7));
        else if(fg >= 0) printf("\x1b[0;%d;%dm#", !!(fg&8), 30+(fg&7));
        else printf("\x1b[0m ");
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
