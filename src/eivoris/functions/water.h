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

// Four crossing travelling wave bands. The returned vector contains
// (height, dHeight/dX, dHeight/dZ), so the same field drives both
// actual near-camera vertex displacement and matching surface normals.
vec3 eivorisWaterWaveField(vec2 worldXZ, float time) {
  vec3 field = vec3(0.0);

  float p0 = dot(worldXZ, vec2(0.80, 0.60)) * 1.85 + time * 1.72;
  float p1 = dot(worldXZ, vec2(-0.48, 0.88)) * 2.95 - time * 2.18;
  float p2 = dot(worldXZ, vec2(0.96, -0.28)) * 4.35 + time * 2.72;
  float p3 = dot(worldXZ, vec2(0.32, 0.95)) * 6.10 - time * 3.25;

  float s0 = sin(p0), c0 = cos(p0);
  float s1 = sin(p1), c1 = cos(p1);
  float s2 = sin(p2), c2 = cos(p2);
  float s3 = sin(p3), c3 = cos(p3);

  field.x += 0.0720 * s0;
  field.yz += 0.0720 * 1.85 * c0 * vec2(0.80, 0.60);

  field.x += 0.0520 * s1;
  field.yz += 0.0520 * 2.95 * c1 * vec2(-0.48, 0.88);

  field.x += 0.0340 * s2;
  field.yz += 0.0340 * 4.35 * c2 * vec2(0.96, -0.28);

  field.x += 0.0200 * s3;
  field.yz += 0.0200 * 6.10 * c3 * vec2(0.32, 0.95);

  return field;
}

// Additional smooth value-noise detail breaks up perfectly regular waves.
vec2 eivorisWaterSlope(vec2 worldXZ, float time) {
  vec2 p0 = worldXZ * vec2(0.75, 0.62)
          + vec2(time * 0.11, -time * 0.07);
  vec2 p1 = worldXZ * 1.65
          + vec2(-time * 0.19, time * 0.13);
  vec2 p2 = worldXZ * 3.25
          + vec2(time * 0.34, time * 0.29);
  vec2 p3 = worldXZ * 5.4
          + vec2(-time * 0.43, time * 0.37);

  vec3 n0 = eivorisNoiseGradient(p0);
  vec3 n1 = eivorisNoiseGradient(p1);
  vec3 n2 = eivorisNoiseGradient(p2);
  vec3 n3 = eivorisNoiseGradient(p3);

  vec2 g0 = n0.yz * vec2(0.75, 0.62);
  vec2 g1 = n1.yz * 1.65;
  vec2 g2 = n2.yz * 3.25;
  vec2 g3 = n3.yz * 5.4;
  vec2 noiseSlope = g0 * 0.105 + g1 * 0.060 + g2 * 0.030 + g3 * 0.016;

  vec3 wave = eivorisWaterWaveField(worldXZ, time);
  return wave.yz * 1.75 + noiseSlope;
}

float eivorisWaterFresnel(float cosine, float f0) {
  float f = 1.0 - clamp(cosine, 0.0, 1.0);
  float f2 = f * f;
  return f0 + (1.0 - f0) * f2 * f2 * f;
}

// Compatibility entry point used by the upstream wet-ground/actor materials.
// Keep this shared helper name even though EIVORIS water uses its own wrapper.
float calculateFresnel(float cosR, float r0) {
  return eivorisWaterFresnel(cosR, r0);
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  // A low-amplitude height field drives real vertex movement near the
  // player. The fade prevents distant chunk geometry from shimmering.
  vec3 waveField = eivorisWaterWaveField(gPos.xz, t);
  float displacementFade = 1.0 - smoothstep(14.0, 26.0, camDist);
  if (fractCposY > 0.0) {
    wPos.y += waveField.x * displacementFade;
  }

  // Keep dense normal ripples visible at close range and at distance.
  vec2 slope = eivorisWaterSlope(gPos.xz, t);
  float detailFade = 1.0 - smoothstep(128.0, 208.0, camDist);
  vec2 waterSlope = slope * mix(0.82, 1.0, detailFade);

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
