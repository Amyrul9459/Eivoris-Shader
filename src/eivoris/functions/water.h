#ifndef WATER_H
#define WATER_H
#include "utils.h"
#include "detection.h"
#include "sky.h"
#include "clouds.h"
#include "noise.h"

// EIVORIS WATER 0.5 — cross-directional, domain-warped geometric waves.
// Continuous world-space field: adjacent water blocks share the same wave surface.
// No horizontal vertex displacement is used.

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

// Domain warping bends the wave crests so they do not form long synchronized bands.
float eivorisWaterRaw(vec2 p, highp float t) {
  vec2 warp;
  warp.x = eivorisValueNoise(p * 0.072 + vec2(0.017 * t, 0.011 * t));
  warp.y = eivorisValueNoise(p * 0.081 + vec2(-0.013 * t, 0.019 * t) + 17.3);
  vec2 q = p + (warp - 0.5) * 3.4;

  float broad = eivorisValueNoise(p * 0.095 + vec2(0.009 * t, -0.012 * t));
  float detail = eivorisValueNoise(p * 0.29 + vec2(-0.024 * t, 0.021 * t));
  float amp = 0.82 + broad * 0.34;

  // Five directions and incommensurate wavelengths break up parallel wave trains.
  float a = sin(1.71 * dot(q, normalize(vec2(0.91, 0.42))) - 0.63 * t + broad * 1.7);
  float b = sin(2.38 * dot(q, normalize(vec2(-0.37, 0.93))) - 0.91 * t + detail * 2.1 + 1.2);
  float c = sin(3.14 * dot(q, normalize(vec2(0.58, -0.81))) - 1.19 * t - broad * 1.4 + 2.4);
  float d = sin(4.25 * dot(q, normalize(vec2(-0.96, -0.28))) - 1.53 * t + detail * 2.8 + 0.6);
  float e = sin(5.35 * dot(q, normalize(vec2(0.18, 0.98))) - 1.87 * t + broad * 2.5 - 1.7);

  // Moving low-frequency noise changes local height and roughness over time.
  float organic = (eivorisValueNoise(q * 0.17 + vec2(0.031 * t, -0.027 * t)) - 0.5) * 2.0;
  return amp * (0.050 * a + 0.037 * b + 0.026 * c + 0.018 * d + 0.010 * e)
       + organic * (0.018 + detail * 0.012);
}

float eivorisWaterHeight(vec2 p, highp float t) {
  float raw = eivorisWaterRaw(p, t);
  // Keep troughs stronger than crests, but avoid flattening every positive crest.
  float asymmetric = raw < 0.0 ? raw * 1.12 : raw * 0.48;
  return clamp(asymmetric, -0.17, 0.055);
}

vec2 eivorisWaterGradient(vec2 p, highp float t) {
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

  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      wPos.y += eivorisWaterHeight(worldXZ, t) * distanceFade;
    #endif
  }

  // Keep lighting normals coherent with the displaced surface.
  vec2 slope = eivorisWaterGradient(worldXZ, t) * distanceFade * 0.78;
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
    nrm = normalize(vec3(normalize(viewDir.xz) + sideBump * NL_WATER_BUMP * 0.30, -0.12));
  }

  float cosR = dot(nrm, viewDir);
  vec3 reflectedDir = viewDir - 2.0 * cosR * nrm;
  vec3 skyReflectionDir = normalize(vec3(reflectedDir.x, -reflectedDir.y, reflectedDir.z));
  vec3 waterRefl = nlRenderSky(skycol, env, reflectedDir, t, false);

  #ifdef NL_CLOUD_AURORA_REFLECTION
    if (reflectedDir.y < 0.0) {
      vec4 cloudRefl = nlCloudAuroraReflection(skycol, env, reflectedDir, wPos, CAMERA_POS, t);
      waterRefl = mix(waterRefl, cloudRefl.rgb, cloudRefl.a);
    }
  #endif

  float sunAlignment = max(dot(skyReflectionDir, normalize(env.sunDir)), 0.0);
  float sunDisk = pow(sunAlignment, 180.0);
  float sunBloom = pow(sunAlignment, 34.0);
  float sunVisibility = (1.0 - env.rainFactor) * smoothstep(-0.20, 0.08, env.dayFactor);
  vec3 sunColor = mix(vec3(1.0, 0.48, 0.22), vec3(1.0, 0.86, 0.66),
                      smoothstep(-0.10, 0.65, env.dayFactor));
  waterRefl += sunColor * (1.35 * sunDisk + 0.16 * sunBloom) * sunVisibility;

  float moonAlignment = max(dot(skyReflectionDir, normalize(env.moonDir)), 0.0);
  float moonGlint = pow(moonAlignment, 110.0) * smoothstep(0.05, 0.55, -env.dayFactor);
  waterRefl += vec3(0.48, 0.60, 0.88) * moonGlint * (1.0 - env.rainFactor);

  float torchSpark = pow(max(0.0, dot(skyReflectionDir, normalize(vec3(viewDir.x, 0.35, viewDir.z)))), 24.0);
  waterRefl += torchColor * NL_TORCHLIGHT_INTENSITY * lit.x * torchSpark * 0.08;

  if (!env.end) waterRefl *= 0.05 + lit.y * 1.14;
  waterRefl *= 1.0 - 0.68 * env.rainFactor;

  float fresnel = calculateFresnel(cosR, 0.055);
  float opacity = 1.0 - abs(cosR);
  color.rgb *= 0.22 * NL_WATER_TINT * (1.0 - 0.78 * fresnel);
  color.a = mix(COLOR.a * NL_WATER_TRANSPARENCY, 1.0, opacity * opacity);
  return vec4(waterRefl, fresnel);
}
#endif
