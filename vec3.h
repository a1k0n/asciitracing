#ifndef __VEC3_H
#define __VEC3_H

#include <math.h>
#include <algorithm>  // for std::min/max

struct vec3 {
  float x,y,z;
  vec3() {}
  vec3(float a, float b, float c) { x=a; y=b; z=c; }
};

static inline vec3 operator+(const vec3 &a, const vec3 &b) {
  return vec3(a.x+b.x,a.y+b.y,a.z+b.z);
}
static inline vec3 operator-(const vec3& a, const vec3& b) {
  return vec3(a.x-b.x,a.y-b.y,a.z-b.z);
}
static inline vec3 operator*(const vec3& a, float b) {
  return vec3(a.x*b,a.y*b,a.z*b);
}
static inline float operator*(const vec3& a, const vec3& b) {
  return a.x*b.x+a.y*b.y+a.z*b.z;
}
static inline float length(const vec3& v) { return sqrt(v*v); }
static inline vec3 normalize(const vec3 &v) {
  return v*(1.0/length(v));
}
static inline vec3 abs(const vec3& v) {
  return vec3(fabs(v.x), fabs(v.y), fabs(v.z));
}
static inline vec3 max(const vec3& v, float f) {
  return vec3(std::max(v.x, f), std::max(v.y, f), std::max(v.z, f));
}
static inline vec3 min(const vec3& v, float f) {
  return vec3(std::min(v.x, f), std::min(v.y, f), std::min(v.z, f));
}
static inline vec3 cross(const vec3& a, const vec3& b) {
  return vec3(a.y*b.z - a.z*b.y,
              a.z*b.x - a.x*b.z,
              a.x*b.y - a.y*b.x);
}

#endif

