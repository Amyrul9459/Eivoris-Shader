#ifndef WATER_H
#define WATER_H
#include "utils.h"
#include "detection.h"
#include "sky.h"
#include "clouds.h"
#include "noise.h"

// EIVORIS WATER 0.2: world-space geometric displacement + analytic normals.
float eivorisWaterHeight(vec2 p, highp float t) {
  vec2 d0 = normalize(vec2(0.82, 0.57));
  vec2 d1 = normalize(vec2(-0.48, 0.88));
  vec2 d2 = normalize(vec2(0.96, -0.28));
  float k0 = 6.28318530718 / 8.0;
  float k1 = 6.28318530718 / 4.7;
  float k2 = 6.28318530718 / 2.9;
  float q0 = k0 * dot(p, d0) - 0.72 * t + 0.4;
  float q1 = k1 * dot(p, d1) - 1.02 * t + 2.1;
  float q2 = k2 * dot(p, d2) - 1.38 * t + 4.0;
  return 0.105 * sin(q0) + 0.055 * sin(q1) + 0.022 * sin(q2);
}
vec2 eivorisWaterGradient(vec2 p, highp float t) {
  vec2 d0 = normalize(vec2(0.82, 0.57));
  vec2 d1 = normalize(vec2(-0.48, 0.88));
  vec2 d2 = normalize(vec2(0.96, -0.28));
  float k0 = 6.28318530718 / 8.0;
  float k1 = 6.28318530718 / 4.7;
  float k2 = 6.28318530718 / 2.9;
  float q0 = k0 * dot(p, d0) - 0.72 * t + 0.4;
  float q1 = k1 * dot(p, d1) - 1.02 * t + 2.1;
  float q2 = k2 * dot(p, d2) - 1.38 * t + 4.0;
  return d0 * (0.105 * k0 * cos(q0))
       + d1 * (0.055 * k1 * cos(q1))
       + d2 * (0.022 * k2 * cos(q2));
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
  // Move the actual RenderChunk vertex before it is transformed to clip space.
  // Maximum displacement is +/-0.182 blocks (about 0.36 blocks crest-to-trough).
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
  // Small-scale animated detail complements, but does not replace, geometry waves.
  vec2 fine = vec2(
    movingNoise2D(gPos.xz * 1.35 + gPos.yy, NL_WATER_WAVE_SPEED * t, 0.6),
    movingNoise2D(gPos.zx * 1.55 + gPos.yy, NL_WATER_WAVE_SPEED * t + 2.4, 0.6)
  );
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
