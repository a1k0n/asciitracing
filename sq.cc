// sphere tracing thing using Square's logo, as it's an easy thing to represent
// with constructive solid geometry.
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <algorithm>
#include "render.h"
#include "vec3.h"

float frame_ = 0;

vec3 lcol(0.7,0.5,0.5);
float mshiny[] = {100,10,10,10};
vec3 mcol[] = {
  vec3(0.7f, 0.7f, 0.7f),
  vec3(0.0f, 0.0f, 0.5f),
  vec3(0.0f, 0.4f, 1.0f),
  vec3(0.2f, 1.0f, 0.0f)};

// http://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float pow8(float t) { t*=t; t*=t; t*=t; return t; }
float length8(float x, float y) {
  return pow(pow8(x) + pow8(y), 1.0/8);
}

float sdTorus88(const vec3& p, float s, float t)
{
  return length8(length8(p.x, p.y) - s, p.z) - t;
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
  float dplane = p.y + 100;
  if (dplane < d) {
    // uncomment the next line for a checkerboard instead of a flat plane
    //*m = ((lrint(p.x*0.01)&1)^(lrint(p.z*0.01)&1));
    *m = 0;
    d = dplane;
  }
  // outer torus
  float douter = sdTorus88(p, 70, 15);
  if (douter < d) {
    *m = 2;
    d = douter;
  }
  // inner box
  float dinner = udRoundBox(p, vec3(20, 20, 5), 10);
  if (dinner < d) {
    *m = 2;
    d = dinner;
  }
  return d;
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
    vec3 campos = vec3(150*sin(frame_*0.02), 50 + 40*sin(frame_*0.03), -150*cos(frame_*0.02));
    vec3 camz = normalize(campos*-1);
    //vec3 lightpos = vec3(0,200,-400);
    //vec3 lightpos = vec3(200.0*sin(frame_*0.05),100,200.0*sin(frame_*0.1));
    vec3 lightpos = vec3(200.0*sin(frame_*0.05),100,campos.z);
    //vec3 lightpos = campos;
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
            vec3 dir = normalize(vec3(x-40.0f,25.0f-2.0f*y,40.0f));
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
              if (t > 6000)
                break;
            }
#ifdef AA
          }
        }
        nearestcolor(color * 0.125, x, y, &fg, &bg);
#else
        nearestcolor(color, x, y, &fg, &bg);
#endif
        //printf("%g %g %g -> %d\n", color.x, color.y, color.z, fg);
        if(bg >= 0) printf("\x1b[%d;%d;%dm#", !!(fg&8), 40+(bg&7), 30+(fg&7));
        else if(fg >= 0) printf("\x1b[0;%d;%dm#", !!(fg&8), 30+(fg&7));
        else printf("\x1b[0m ");
        if (y == 23 && x == 65) { printf("\x1b[0;1;30mandy@a1k0n.net"); break; }
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
