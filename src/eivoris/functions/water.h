#ifndef WATER_H
#define WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"

// EIVORIS WATER — smooth multi-scale directional surface.
// The wave field is authored here; it does not use Newb wave/noise functions.
float eivorisWaveHeight(vec2 p, float t) {
  const float TAU = 6.28318530718;

  // Long rolling swell, medium crossing swell, then restrained surface detail.
  float q0 = TAU * dot(p, normalize(vec2(0.82, 0.57))) / 18.0 - 0.72 * t + 0.4;
  float q1 = TAU * dot(p, normalize(vec2(-0.36, 0.93))) / 10.5 - 0.91 * t + 2.1;
  float q2 = TAU * dot(p, normalize(vec2(0.25, -0.97))) / 5.8 - 1.13 * t + 4.0;
  float q3 = TAU * dot(p, normalize(vec2(-0.91, -0.41))) / 3.6 - 1.36 * t + 1.3;

  return 0.160 * sin(q0)
       + 0.075 * sin(q1)
       + 0.028 * sin(q2)
       + 0.009 * sin(q3);
}

// Exact gradient of eivorisWaveHeight: surface normal follows displacement.
vec2 eivorisWaveSlope(vec2 p, float t) {
  const float TAU = 6.28318530718;
  vec2 d0 = normalize(vec2(0.82, 0.57));
  vec2 d1 = normalize(vec2(-0.36, 0.93));
  vec2 d2 = normalize(vec2(0.25, -0.97));
  vec2 d3 = normalize(vec2(-0.91, -0.41));

  float k0 = TAU / 18.0;
  float k1 = TAU / 10.5;
  float k2 = TAU / 5.8;
  float k3 = TAU / 3.6;

  float q0 = k0 * dot(p, d0) - 0.72 * t + 0.4;
  float q1 = k1 * dot(p, d1) - 0.91 * t + 2.1;
  float q2 = k2 * dot(p, d2) - 1.13 * t + 4.0;
  float q3 = k3 * dot(p, d3) - 1.36 * t + 1.3;

  return (0.160 * k0 * cos(q0)) * d0
       + (0.075 * k1 * cos(q1)) * d1
       + (0.028 * k2 * cos(q2)) * d2
       + (0.009 * k3 * cos(q3)) * d3;
}

// Schlick Fresnel.
float calculateFresnel(float cosR, float r0) {
  float f = 1.0 - clamp(cosR, 0.0, 1.0);
  float f2 = f * f;
  return r0 + (1.0 - r0) * f2 * f2 * f;
}

float eivorisFresnel(float cosR, float r0) {
  return calculateFresnel(cosR, r0);
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  vec2 slope = eivorisWaveSlope(gPos.xz, t);
  float waveHeight = eivorisWaveHeight(gPos.xz, t);

  // Keep full displacement around the player; fade only at long distance.
  float nearFade = 1.0 - smoothstep(48.0, 88.0, camDist);

  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      // Minecraft water top faces use the downward-facing convention here.
      nrm = normalize(vec3(slope.x, -1.0, slope.y));
      wPos.y -= waveHeight * nearFade;
    #else
      nrm = vec3(0.0, -1.0, 0.0);
    #endif

    // Restrained high-frequency normal detail; no vertex displacement,
    // so close-range ripples stay smooth instead of producing broken geometry.
    float r0 = sin(dot(gPos.xz, vec2(0.74, 1.08)) - 1.55 * t);
    float r1 = cos(dot(gPos.xz, vec2(-1.12, 0.63)) - 1.22 * t);
    nrm.xz += vec2(r0, r1) * (0.018 * nearFade);
  } else {
    vec2 sideSlope = slope * 0.35;
    nrm = normalize(vec3(normalize(viewDir.xz) + sideSlope, -0.12));
  }

  nrm = normalize(nrm);
  float cosR = dot(nrm, viewDir);
  viewDir = viewDir - 2.0 * cosR * nrm;

  vec3 waterRefl = nlRenderSky(skycol, env, viewDir, t, false);

  // Small point-light glints only. No cloud/aurora reflection pass.
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
