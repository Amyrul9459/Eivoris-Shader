#ifndef WATER_H
#define WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"
#include "clouds.h"
#include "noise.h"

// EIVORIS WATER ENGINE 0.3.0 — analytic, low-cost vertex displacement
// Independent world-space wave model. NEWB is used only for engine integration.
// The surface field is continuous across adjacent water blocks.

// Four-band directional wave field. Height and analytic normal are evaluated
// together so every vertex computes each wave phase only once.
float eivorisWave(vec2 p, vec2 direction, float wavelength, float speed,
                  float amplitude, float phase, highp float t, inout vec2 gradient) {
  float k = 6.2831853 / wavelength;
  float theta = k * dot(p, direction) - speed * t + phase;
  float fundamental = sin(theta);
  float harmonic = sin(2.0 * theta + 0.7);
  float derivative = amplitude * k *
    (cos(theta) + 0.36 * cos(2.0 * theta + 0.7));
  gradient += direction * derivative;
  return amplitude * (fundamental + 0.18 * harmonic);
}

vec3 eivorisWaterField(vec2 p, highp float t) {
  highp float waveTime = t * 1.35;
  vec2 gradient = vec2(0.0);
  float height = 0.0;

  height += eivorisWave(p, normalize(vec2(0.91, 0.41)), 8.4, 0.52, 0.078, 0.2, waveTime, gradient);
  height += eivorisWave(p, normalize(vec2(-0.36, 0.93)), 5.9, 0.73, 0.052, 2.1, waveTime, gradient);
  height += eivorisWave(p, normalize(vec2(0.57, -0.82)), 4.15, 0.96, 0.034, 4.3, waveTime, gradient);
  height += eivorisWave(p, normalize(vec2(-0.96, -0.27)), 3.35, 1.18, 0.020, 1.4, waveTime, gradient);

  // Long swells break up repeated wave trains without texture sampling.
  vec2 swellA = vec2(0.19, 0.11);
  vec2 swellB = vec2(-0.13, 0.21);
  float phaseA = dot(p, swellA) - 0.22 * waveTime;
  float phaseB = dot(p, swellB) + 0.31 * waveTime;
  height += 0.020 * sin(phaseA) + 0.012 * sin(phaseB);
  gradient += 0.020 * cos(phaseA) * swellA + 0.012 * cos(phaseB) * swellB;

  // Smooth saturation bounds displacement and applies its derivative to the normal.
  float inv = inversesqrt(1.0 + height * height * 10.0);
  height *= inv;
  gradient *= inv * inv * inv;
  return vec3(height, gradient);
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

  // Height and slope share one phase evaluation for each wave band.
  vec3 waterField = vec3(0.0);
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      waterField = eivorisWaterField(worldXZ, t);
      wPos.y += waterField.x * distanceFade;
    #endif
  }

  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      vec2 slope = waterField.yz * distanceFade;
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
  // Broad, low-energy specular lobe: soft sun reflection, not a hard white dot.
  float sunDisk = pow(sunAlignment, 30.0);
  float sunHalo = pow(sunAlignment, 10.0);
  float daylight = (1.0 - env.rainFactor) * smoothstep(-0.20, 0.08, env.dayFactor);
  vec3 sunColor = mix(vec3(1.0, 0.48, 0.30), vec3(1.0, 0.90, 0.78),
                      smoothstep(-0.10, 0.65, env.dayFactor));
  waterRefl += sunColor * (0.34 * sunDisk + 0.075 * sunHalo) * daylight;

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
