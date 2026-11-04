#ifndef EIVORIS_WATER_H
#define EIVORIS_WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"

// EIVORIS water model.
// The wave field below is authored for EIVORIS and does not use Newb water macros.
float eivorisWaveHeight(vec2 p, float t) {
  float p0 = dot(p, vec2(0.78, 0.62)) * 0.72 + t * 1.05;
  float p1 = dot(p, vec2(-0.42, 0.91)) * 1.15 - t * 0.82 + 1.7;
  float p2 = dot(p, vec2(0.96, -0.28)) * 2.00 + t * 1.35 + 2.4;
  float p3 = dot(p, vec2(-0.18, -0.98)) * 2.65 - t * 1.62 + 0.6;

  return 0.140 * sin(p0)
       + 0.075 * sin(p1)
       + 0.035 * sin(p2)
       + 0.018 * sin(p3);
}

vec2 eivorisWaveSlope(vec2 p, float t) {
  float p0 = dot(p, vec2(0.78, 0.62)) * 0.72 + t * 1.05;
  float p1 = dot(p, vec2(-0.42, 0.91)) * 1.15 - t * 0.82 + 1.7;
  float p2 = dot(p, vec2(0.96, -0.28)) * 2.00 + t * 1.35 + 2.4;
  float p3 = dot(p, vec2(-0.18, -0.98)) * 2.65 - t * 1.62 + 0.6;

  float c0 = 0.140 * 0.72 * cos(p0);
  float c1 = 0.075 * 1.15 * cos(p1);
  float c2 = 0.035 * 2.00 * cos(p2);
  float c3 = 0.018 * 2.65 * cos(p3);

  return c0 * vec2(0.78, 0.62)
       + c1 * vec2(-0.42, 0.91)
       + c2 * vec2(0.96, -0.28)
       + c3 * vec2(-0.18, -0.98);
}

float eivorisFresnel(float cosR, float r0) {
  float f = 1.0 - clamp(cosR, 0.0, 1.0);
  float f2 = f * f;
  return r0 + (1.0 - r0) * f2 * f2 * f;
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  vec2 slope = eivorisWaveSlope(gPos.xz, t);
  float waveHeight = eivorisWaveHeight(gPos.xz, t);

  // Close-range wave displacement is strongest around the player and fades smoothly.
  float nearFade = 1.0 - smoothstep(12.0, 30.0, camDist);

  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      nrm = normalize(vec3(slope.x, -1.0, slope.y));
      wPos.y -= waveHeight * nearFade;
    #else
      nrm = vec3(0.0, -1.0, 0.0);
    #endif

    // A restrained high-frequency ripple layer keeps highlights alive at close range.
    float ripple = sin(dot(gPos.xz, vec2(2.1, -1.7)) + t * 2.2)
                 * cos(dot(gPos.xz, vec2(1.4, 2.3)) - t * 1.7);
    nrm.xz += vec2(ripple, -ripple) * (0.055 * nearFade);
  } else {
    vec2 sideSlope = slope * 0.45;
    nrm = normalize(vec3(normalize(viewDir.xz) + sideSlope, -0.12));
  }

  nrm = normalize(nrm);
  float cosR = dot(nrm, viewDir);
  viewDir = viewDir - 2.0 * cosR * nrm;

  vec3 waterRefl = nlRenderSky(skycol, env, viewDir, t, false);

  // Torch-light glints only; no cloud or aurora reflection pass.
  float tc = 0.5 + 0.5 * sin(16.0 * viewDir.x) * sin(16.0 * viewDir.z);
  waterRefl += torchColor * NL_TORCHLIGHT_INTENSITY * lit.x * tc * tc;

  if (!env.end) {
    waterRefl *= 0.05 + lit.y * 1.14;
  }

  cosR = abs(cosR);
  float fresnel = eivorisFresnel(cosR, 0.07);
  float opacity = 1.0 - cosR;

  color.rgb *= 0.22 * NL_WATER_TINT * (1.0 - 0.8 * fresnel);
  color.a = mix(COLOR.a * NL_WATER_TRANSPARENCY, 1.0, opacity * opacity);

  return vec4(waterRefl, fresnel);
}

#endif
