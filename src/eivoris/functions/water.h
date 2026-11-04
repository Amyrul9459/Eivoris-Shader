#ifndef WATER_H
#define WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"

// EIVORIS procedural water: independent implementation inspired by the
// multi-scale derivative-noise approach observed in FirmamentCraft.
// No FirmamentCraft source or binary is copied into this project.

// Stable, inexpensive 2D hash for procedural value noise.
float eivorisHash21(vec2 p) {
  p = fract(p * vec2(123.34, 456.21));
  p += dot(p, p + 45.32);
  return fract(p.x * p.y);
}

// Cubic value noise and its analytic XY derivative.
// Returns (noise, dNoise/dx, dNoise/dy).
vec3 eivorisNoiseGradient(vec2 p) {
  vec2 cell = floor(p);
  vec2 f = fract(p);

  float a = eivorisHash21(cell);
  float b = eivorisHash21(cell + vec2(1.0, 0.0));
  float c = eivorisHash21(cell + vec2(0.0, 1.0));
  float d = eivorisHash21(cell + vec2(1.0, 1.0));

  // Hermite interpolation and its derivative.
  vec2 u = f * f * (3.0 - 2.0 * f);
  vec2 du = 6.0 * f * (1.0 - f);

  float ab = mix(a, b, u.x);
  float cd = mix(c, d, u.x);
  float value = mix(ab, cd, u.y);

  float dx0 = (b - a) * du.x;
  float dx1 = (d - c) * du.x;
  float dx = mix(dx0, dx1, u.y);
  float dy = (cd - ab) * du.y;

  return vec3(value, dx, dy);
}

// Three animated scales: broad rolling pattern, medium ripples, fine detail.
// Derivatives are scaled back into world-space slope units.
vec2 eivorisWaterSlope(vec2 worldXZ, float time) {
  vec2 p0 = worldXZ * vec2(0.75, 0.62)
          + vec2(time * 0.11, -time * 0.07);
  vec2 p1 = worldXZ * 1.65
          + vec2(-time * 0.19, time * 0.13);
  vec2 p2 = worldXZ * 3.25
          + vec2(time * 0.27, time * 0.21);

  vec3 n0 = eivorisNoiseGradient(p0);
  vec3 n1 = eivorisNoiseGradient(p1);
  vec3 n2 = eivorisNoiseGradient(p2);

  vec2 g0 = n0.yz * vec2(0.75, 0.62);
  vec2 g1 = n1.yz * 1.65;
  vec2 g2 = n2.yz * 3.25;

  // Lower amplitudes at higher frequencies keep the water calm,
  // while the gradient remains visible close to the camera.
  return g0 * 0.105 + g1 * 0.046 + g2 * 0.014;
}

float eivorisWaterFresnel(float cosine, float f0) {
  float f = 1.0 - clamp(cosine, 0.0, 1.0);
  float f2 = f * f;
  return f0 + (1.0 - f0) * f2 * f2 * f;
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  // Fragment-normal-only water: no vertex displacement, so the surface
  // never rises through neighboring block boundaries.
  vec2 slope = eivorisWaterSlope(gPos.xz, t);

  // Retain full nearby ripple definition; fade only a little at distance.
  float detailFade = 1.0 - smoothstep(72.0, 144.0, camDist);
  vec2 waterSlope = slope * mix(0.72, 1.0, detailFade);

  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      nrm = normalize(vec3(waterSlope.x, -1.0, waterSlope.y));
    #else
      nrm = vec3(0.0, -1.0, 0.0);
    #endif
  } else {
    vec2 sideSlope = waterSlope * 0.32;
    nrm = normalize(vec3(normalize(viewDir.xz) + sideSlope, -0.12));
  }

  float cosR = dot(nrm, viewDir);
  vec3 reflectedDir = viewDir - 2.0 * cosR * nrm;

  // Keep the existing RenderDragon sky-reflection path. Cloud reflections
  // remain disabled; the procedural normal field supplies the water detail.
  vec3 waterRefl = nlRenderSky(skycol, env, reflectedDir, t, false);

  float torchGlint = 0.5 + 0.5 * sin(16.0 * reflectedDir.x)
                                  * sin(16.0 * reflectedDir.z);
  waterRefl += torchColor * NL_TORCHLIGHT_INTENSITY * lit.x
             * torchGlint * torchGlint;

  // Tight directional sun highlight, intentionally restrained.
  vec3 sunDir = normalize(light);
  float sunSpec = pow(max(dot(reflectedDir, sunDir), 0.0), 72.0);
  waterRefl += vec3(1.0, 0.91, 0.76) * sunSpec * 0.16 * lit.y;

  if (!env.end) waterRefl *= 0.05 + lit.y * 1.14;

  cosR = abs(cosR);
  float fresnel = eivorisWaterFresnel(cosR, 0.075);
  float opacity = 1.0 - cosR;

  color.rgb *= 0.22 * NL_WATER_TINT * (1.0 - 0.76 * fresnel);
  color.a = mix(COLOR.a * NL_WATER_TRANSPARENCY, 1.0, opacity * opacity);
  return vec4(waterRefl, fresnel);
}
#endif
