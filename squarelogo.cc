// sphere tracing thing using Square's logo, as it's an easy thing to represent
// with constructive solid geometry.
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <algorithm>
#include "render.h"
#include "vec3.h"

float frame_ = 0;

// light colors, material colors
vec3 lcol(0.7,0.5,0.5);
vec3 mcol[] = {
  vec3(1.7f, 1.7f, 1.7f),  // "floor" plane
  vec3(0.0f, 0.0f, 0.5f),  // "floor" checkerboard color (not currently used)
  vec3(0.0f, 0.4f, 1.0f)}; // logo color
// material shininess
float mshiny[] = {100,10,10};

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

// returns minimum distance to scene, material m
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

// ambient occlusion hack for soft shadows
float shadow(const vec3& ro, const vec3& rd, float mint, float maxt) {
  int m;
  float res = 1.0;
  for (float t = mint; t < maxt; ) {
    float h = dist(ro+rd*t, &m);
    if (h < 0.001) {
      return 0;
    }
    // the constant (20) tunes the hardness/softness of the shadows
    res = std::min( res, 20.0f*h/t );
    t += h;
  }
  return res;
}

vec3 lighting(const vec3 &p, const vec3& n, int m, const vec3& lightpos) {
  vec3 lightdir = normalize(lightpos - p);
  float s = std::max(0.3f, shadow(p, lightdir, 0.01, length(p-lightpos)));
  float l = std::max(0.1f, std::max(-dot(lightdir,n), 0.0f) * s);
  return mcol[m]*l + lcol*pow(l, mshiny[m]);
}

int main()
{
  int x,y;
  for(;;) {
    vec3 campos = vec3(150*sin(frame_*0.02), 50 + 40*sin(frame_*0.03), -150*cos(frame_*0.02));
    vec3 lightpos = vec3(200.0*sin(frame_*0.05),100,campos.z);
    vec3 camz = normalize(campos*-1);
    vec3 camx = normalize(cross(camz, vec3(0,1,0)));
    vec3 camy = normalize(cross(camx, camz));
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
        printcolor(fg, bg);
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

