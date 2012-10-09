// sphere tracing demo with a few random elements
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
  vec3(1.0f, 1.0f, 1.0f),
  vec3(0.0f, 0.0f, 1.0f),
  vec3(1.0f, 0.2f, 0.0f),
  vec3(0.2f, 1.0f, 0.0f)};

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
  float dplane = p.y;
  if (dplane < d) {
    *m = ((lrint(p.x*0.03)&1)^(lrint(p.z*0.03)&1));
    d = dplane;
  }
#if 1
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
#if 1
  float dsphere2 = sdSphere(p - vec3(0,40,80), 15);
  if (dsphere2 < d) {
    *m = 3;
    d = dsphere2;
  }
#endif
  return d;
}

float shadow(const vec3& ro, const vec3& rd, float mint, float maxt) {
  int m;
  float res = 1.0;
  for (float t = mint; t < maxt; ) {
    float h = dist(ro+rd*t, &m);
    if (h < 0.001) {
      return 0;
    }
    res = std::min( res, 10.0f*h/t );
    t += h;
  }
  return res;
}

vec3 lighting(const vec3 &p, int m, const vec3& lightpos) {
  vec3 lightdir = normalize(lightpos - p);
  // yet another trick from iq: use the distance field to compute dot(light,
  // normal) instead of explicitly finding a normal.
  // http://www.pouet.net/topic.php?which=7535&page=1
  int _m;
  float diffuse = std::max(0.0f, 10.0f*dist(p+lightdir*0.1, &_m));
  float s = std::max(0.3f, shadow(p, lightdir, 0.01, length(p-lightpos)));
  float l = std::max(0.1f, diffuse * s);
  return mcol[m]*l + lcol*pow(l, mshiny[m]);
}

int main()
{
  int x,y;
  for(;;) {
    vec3 campos = vec3(100*sin(frame_*0.01), 110 + 100*sin(frame_*0.03), -100*cos(frame_*0.01));
    vec3 camz = normalize(campos*-1);
    //vec3 lightpos = vec3(200,400,campos.z);
    //vec3 lightpos = campos;
    //vec3 lightpos = vec3(100*sin(frame_*0.08), 50, -100*cos(frame_*0.04));
    vec3 lightpos = vec3(100*sin(frame_*0.037), 200, campos.z);
    vec3 lightpos2 = vec3(0, 200, 0);
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
            vec3 dir = normalize(vec3(x-40.0f,25.0f-2.0f*y,50.0f));
#endif
            dir = camx*dir.x + camy*dir.y + camz*dir.z;
            float t = 0;
            int m = -1;
            for (int iter = 0; iter < 64 && t < 1e6; iter++) {
              vec3 p = dir*t + campos;
              float d = dist(p, &m);
              if (d < 0.001) {
                color = color + lighting(p, m, lightpos);
                break;
              }
              t += d;
            }
#ifdef AA
          }
        }
        nearestcolor(color * 0.125, x, y, &fg, &bg);
#else
        nearestcolor(color, x, y, &fg, &bg);
#endif
        printcolor(fg, bg);
      }
      printf("\x1b[0m\n");
    }
    fflush(stdout);
    usleep(20000);
    frame_ += 1;
    printf("\x1b[24A");
  }
}
