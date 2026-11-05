#ifndef RAIN_H
#define RAIN_H

#include "clouds.h"
#include "detection.h"
#include "noise.h"
#include "sky.h"
#include "water.h"

float nlWindblow(vec3 pos, float t){
  vec2 p = pos.xy/(1.0+pos.z);
  float val = sin(4.0*p.x + 2.0*p.y + 2.0*t + 3.0*p.y*p.x)*sin(p.y*2.0 + 0.2*t);
  val += sin(p.y - p.x + 0.2*t);
  return 0.25*val*val;
}

// World-anchored animated ring field retained as a future rain-splash input.
// It does not alter block albedo, roughness, normals, or reflections by itself.
float eivorisRainImpact(vec2 worldXZ, float t) {
  vec2 cell = floor(worldXZ * 0.82);
  vec2 local = fract(worldXZ * 0.82) - 0.5;
  float seed = fastRand(cell);
  vec2 center = (vec2(fract(seed * 17.13), fract(seed * 31.71)) - 0.5) * 0.66;
  float phase = fract(t * (0.82 + seed * 0.34) + seed);
  float radius = length(local - center);

  float radiusA = 0.04 + phase * 0.43;
  float radiusB = 0.04 + fract(phase + 0.48) * 0.43;
  float ringA = exp(-abs(radius - radiusA) * 92.0);
  float ringB = exp(-abs(radius - radiusB) * 82.0);
  float lifeA = smoothstep(0.02, 0.12, phase) * (1.0 - smoothstep(0.82, 1.0, phase));
  float lifeB = smoothstep(0.02, 0.12, fract(phase + 0.48))
              * (1.0 - smoothstep(0.82, 1.0, fract(phase + 0.48)));
  float centerDimple = exp(-radius * radius * 150.0)
                     * (1.0 - smoothstep(0.0, 0.18, phase));
  return clamp(ringA * lifeA * 0.62 + ringB * lifeB * 0.34
             + centerDimple * 0.22, 0.0, 1.0);
}

// Ground-reflection hook intentionally disabled.
// EIVORIS does not render permanent puddles or rain-dependent wet coatings.
vec4 nlRefl(
  inout vec4 color, nl_skycolor skycol, nl_environment env, vec3 viewDir, vec3 wPos, vec3 tiledCpos,
  vec3 CAMERA_POS, vec3 torchColor, vec2 lit, float camDist, float renderDist, highp float t
) {
  return vec4(0.0);
}

#endif
