#ifndef WATER_H
#define WATER_H
#include "utils.h"
#include "detection.h"
#include "sky.h"
#include "clouds.h"
#include "noise.h"

// EIVORIS WATER 0.3 — irregular, asymmetric geometric wave field.
// The field is continuous in world XZ; adjacent water vertices evaluate
// the same height. No horizontal displacement is applied.

float eivorisHash21(vec2 p) {
  p = fract(p * vec2(123.34, 456.21));
  p += dot(p, p + 45.32);
  return fract(p.x * p.y);
}

float eivorisValueNoise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);
  f = f * f * (3.0 - 2.0 * f);
  float a = eivorisHash21(i);
  float b = eivorisHash21(i + vec2(1.0, 0.0));
  float c = eivorisHash21(i + vec2(0.0, 1.0));
  float d = eivorisHash21(i + vec2(1.0, 1.0));
  return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float eivorisWaterRaw(vec2 p, highp float t) {
  // Smoothly varying phase and amplitude prevent a synchronized, tiled look.
  float broad = eivorisValueNoise(p * 0.115 + vec2(0.013 * t, -0.009 * t));
  float detail = eivorisValueNoise(p * 0.31 + vec2(-0.021 * t, 0.017 * t));
  float phaseJitter = (broad - 0.5) * 2.2 + (detail - 0.5) * 0.9;
  float amplitude = 0.78 + broad * 0.42;

  vec2 d0 = normalize(vec2(0.82, 0.57));
  vec2 d1 = normalize(vec2(-0.48, 0.88));
  vec2 d2 = normalize(vec2(0.96, -0.28));
  vec2 d3 = normalize(vec2(-0.91, -0.39));

  float q0 = 6.2831853 / 8.0 * dot(p, d0) - 0.61 * t + phaseJitter;
  float q1 = 6.2831853 / 4.7 * dot(p, d1) - 0.93 * t + phaseJitter * 1.7 + 1.4;
  float q2 = 6.2831853 / 2.9 * dot(p, d2) - 1.31 * t - phaseJitter * 1.2 + 3.1;
  float q3 = 6.2831853 / 2.15 * dot(p, d3) - 1.67 * t + detail * 2.0;

  // Unequal directions, wavelengths and speeds create a rougher composite.
  return amplitude * (
      0.090 * sin(q0)
    + 0.048 * sin(q1)
    + 0.026 * sin(q2)
    + 0.012 * sin(q3)
  );
}

float eivorisWaterHeight(vec2 p, highp float t) {
  float raw = eivorisWaterRaw(p, t);

  // Suppress upward crests; strengthen downward troughs.
  // Positive displacement is tightly capped to protect adjacent block sides.
  float asymmetric = raw < 0.0 ? raw * 1.22 : raw * 0.20;
  return clamp(asymmetric, -0.19, 0.025);
}

vec2 eivorisWaterGradient(vec2 p, highp float t) {
  // Finite differences follow the asymmetric/noise-modulated height field.
  const float e = 0.075;
  float hx1 = eivorisWaterHeight(p + vec2(e, 0.0), t);
  float hx0 = eivorisWaterHeight(p - vec2(e, 0.0), t);
  float hz1 = eivorisWaterHeight(p + vec2(0.0, e), t);
  float hz0 = eivorisWaterHeight(p - vec2(0.0, e), t);
  return vec2((hx1 - hx0) / (2.0 * e), (hz1 - hz0) / (2.0 * e));
}

float calculateFresnel(float cosR, float r0) {
  float f = 1.0 - clamp(abs(cosR), 0.0, 1.0);
  float f2 = f * f;
  return r0 + (1.0 - r0) * f2 * f2 * f;
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  vec2 worldXZ = gPos.xz;
  float distanceFade = 1.0 - smoothstep(42.0, 76.0, camDist);

  // Vertical-only displacement: no vertex can drift horizontally into a
  // neighboring solid block. Crest height is capped at +0.025 block.
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      wPos.y += eivorisWaterHeight(worldXZ, t) * distanceFade;
    #endif
  }

  vec2 slope = eivorisWaterGradient(worldXZ, t) * distanceFade;
  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      nrm = normalize(vec3(slope.x, -1.0, slope.y));
    #else
      nrm = vec3(0.0, -1.0, 0.0);
    #endif
  } else {
    vec2 sideBump = vec2(
      movingNoise2D(gPos.xz + gPos.yy, NL_WATER_WAVE_SPEED * t, 0.6),
      movingNoise2D(gPos.zx + gPos.yy, NL_WATER_WAVE_SPEED * t + 1.7, 0.6)
    );
    nrm = normalize(vec3(normalize(viewDir.xz) + sideBump * NL_WATER_BUMP * 0.45, -0.12));
  }

  float cosR = dot(nrm, viewDir);
  vec3 reflectedDir = viewDir - 2.0 * cosR * nrm;
  vec3 waterRefl = nlRenderSky(skycol, env, reflectedDir, t, false);
  waterRefl += torchColor * NL_TORCHLIGHT_INTENSITY * lit.x
             * pow(max(0.0, 0.5 + 0.5 * sin(16.0 * reflectedDir.x)
                                      * sin(16.0 * reflectedDir.z)), 2.0);
  if (!env.end) waterRefl *= 0.05 + lit.y * 1.14;
  waterRefl *= 1.0 - 0.68 * env.rainFactor;

  float fresnel = calculateFresnel(cosR, 0.055);
  float opacity = 1.0 - abs(cosR);
  color.rgb *= 0.22 * NL_WATER_TINT * (1.0 - 0.78 * fresnel);
  color.a = mix(COLOR.a * NL_WATER_TRANSPARENCY, 1.0, opacity * opacity);
  return vec4(waterRefl, fresnel);
}
#endif
