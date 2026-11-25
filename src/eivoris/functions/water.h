#ifndef WATER_H
#define WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"
#include "clouds.h"
#include "noise.h"

// EIVORIS WATER ENGINE 0.2.2
// Independent world-space wave model. NEWB is used only for engine integration.
// The surface field is continuous across adjacent water blocks.

float eivorisHash21(vec2 p) {
  p = fract(p * vec2(123.34, 456.21));
  p += dot(p, p + 45.32);
  return fract(p.x * p.y);
}

float eivorisNoise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);
  f = f * f * (3.0 - 2.0 * f);
  float a = eivorisHash21(i);
  float b = eivorisHash21(i + vec2(1.0, 0.0));
  float c = eivorisHash21(i + vec2(0.0, 1.0));
  float d = eivorisHash21(i + vec2(1.0, 1.0));
  return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

// Directional wave profile. Unequal wavelengths, speeds and phase offsets
// prevent the surface from collapsing into synchronized parallel bands.
float eivorisWave(vec2 p, vec2 direction, float wavelength, float speed,
                  float amplitude, float phase, highp float t) {
  float k = 6.2831853 / wavelength;
  float theta = k * dot(p, direction) - speed * t + phase;
  // Sharpen the forward face slightly while keeping a rounded trough.
  float s = sin(theta);
  return amplitude * (s + 0.18 * sin(2.0 * theta + 0.7));
}

float eivorisWaterHeight(vec2 p, highp float t) {
  float n0 = eivorisNoise(p * 0.105 + vec2(0.013 * t, -0.009 * t));
  float n1 = eivorisNoise(p * 0.245 + vec2(-0.021 * t, 0.017 * t) + 19.7);

  // Slowly moving domain warp makes wave paths bend and drift over time.
  vec2 warp;
  warp.x = eivorisNoise(p * 0.071 + vec2(0.010 * t, 0.014 * t));
  warp.y = eivorisNoise(p * 0.083 + vec2(-0.016 * t, 0.008 * t) + 31.4);
  vec2 q = p + (warp - 0.5) * 2.2;

  float waves = 0.0;
  waves += eivorisWave(q, normalize(vec2(0.91, 0.41)), 7.2, 0.58, 0.038, 0.2, t);
  waves += eivorisWave(q, normalize(vec2(-0.36, 0.93)), 4.8, 0.86, 0.026, 2.1, t);
  waves += eivorisWave(q, normalize(vec2(0.57, -0.82)), 3.45, 1.13, 0.017, 4.3, t);
  waves += eivorisWave(q, normalize(vec2(-0.96, -0.27)), 2.65, 1.47, 0.010, 1.4, t);

  // Broad swell and fine unevenness are independent from the directional waves.
  float swell = (n0 - 0.5) * 0.035;
  float rough = (n1 - 0.5) * 0.020;
  float raw = waves + swell + rough;

  // Asymmetric crest/trough profile: visible dips, restrained upward rise.
  float shaped = raw < 0.0 ? raw * 1.18 : raw * 0.72;
  return clamp(shaped, -0.135, 0.065);
}

// Central differences evaluate the actual deformed height field, including warp.
vec2 eivorisWaterGradient(vec2 p, highp float t) {
  const float e = 0.06;
  float xp = eivorisWaterHeight(p + vec2(e, 0.0), t);
  float xm = eivorisWaterHeight(p - vec2(e, 0.0), t);
  float zp = eivorisWaterHeight(p + vec2(0.0, e), t);
  float zm = eivorisWaterHeight(p - vec2(0.0, e), t);
  return vec2((xp - xm) / (2.0 * e), (zp - zm) / (2.0 * e));
}

float calculateFresnel(float cosTheta, float f0) {
  float f = 1.0 - clamp(abs(cosTheta), 0.0, 1.0);
  float f2 = f * f;
  return f0 + (1.0 - f0) * f2 * f2 * f;
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  vec2 worldXZ = gPos.xz;
  float distanceFade = 1.0 - smoothstep(38.0, 72.0, camDist);

  // Actual vertex displacement: the water mesh moves vertically in world space.
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      wPos.y += eivorisWaterHeight(worldXZ, t) * distanceFade;
    #endif
  }

  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      vec2 slope = eivorisWaterGradient(worldXZ, t) * distanceFade;
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

  // Reflect sky along the displaced surface normal.
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
  float sunDisk = pow(sunAlignment, 190.0);
  float sunHalo = pow(sunAlignment, 38.0);
  float daylight = (1.0 - env.rainFactor) * smoothstep(-0.20, 0.08, env.dayFactor);
  vec3 sunColor = mix(vec3(1.0, 0.46, 0.24), vec3(1.0, 0.88, 0.72),
                      smoothstep(-0.10, 0.65, env.dayFactor));
  waterRefl += sunColor * (1.20 * sunDisk + 0.12 * sunHalo) * daylight;

  float moonAlignment = max(dot(skyReflectionDir, normalize(env.moonDir)), 0.0);
  float moonGlint = pow(moonAlignment, 120.0) * smoothstep(0.05, 0.55, -env.dayFactor);
  waterRefl += vec3(0.43, 0.56, 0.82) * moonGlint * (1.0 - env.rainFactor);

  float torchSpark = pow(max(0.0, dot(skyReflectionDir, normalize(vec3(viewDir.x, 0.35, viewDir.z)))), 24.0);
  waterRefl += torchColor * NL_TORCHLIGHT_INTENSITY * lit.x * torchSpark * 0.07;

  if (!env.end) waterRefl *= 0.05 + lit.y * 1.14;
  waterRefl *= 1.0 - 0.68 * env.rainFactor;

  float fresnel = calculateFresnel(cosR, 0.055);
  float opacity = 1.0 - abs(cosR);
  color.rgb *= 0.22 * NL_WATER_TINT * (1.0 - 0.78 * fresnel);
  color.a = mix(COLOR.a * NL_WATER_TRANSPARENCY, 1.0, opacity * opacity);
  return vec4(waterRefl, fresnel);
}

#endif
