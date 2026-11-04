#ifndef EIVORIS_WATER_H
#define EIVORIS_WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"

// EIVORIS independent water field.
// Five overlapping directional wave bands create a tighter, smoother
// surface pattern. Frequencies, amplitudes and phase speeds are authored
// here; no Newb water macros or wave parameters are used.
float eivorisWaveHeight(vec2 p, float t) {
  float p0 = dot(p, vec2(0.78, 0.62)) * 1.45 + t * 1.25;
  float p1 = dot(p, vec2(-0.42, 0.91)) * 2.35 - t * 1.05 + 1.7;
  float p2 = dot(p, vec2(0.96, -0.28)) * 3.80 + t * 1.65 + 2.4;
  float p3 = dot(p, vec2(-0.18, -0.98)) * 5.60 - t * 1.95 + 0.6;
  float p4 = dot(p, vec2(0.64, -0.77)) * 7.10 + t * 2.25 + 3.1;

  return 0.105 * sin(p0)
       + 0.064 * sin(p1)
       + 0.034 * sin(p2)
       + 0.018 * sin(p3)
       + 0.009 * sin(p4);
}

// Analytic derivative of the same wave field. Keeping the normal derived
// from the exact height function avoids mismatched, jittery surface shading.
vec2 eivorisWaveSlope(vec2 p, float t) {
  float p0 = dot(p, vec2(0.78, 0.62)) * 1.45 + t * 1.25;
  float p1 = dot(p, vec2(-0.42, 0.91)) * 2.35 - t * 1.05 + 1.7;
  float p2 = dot(p, vec2(0.96, -0.28)) * 3.80 + t * 1.65 + 2.4;
  float p3 = dot(p, vec2(-0.18, -0.98)) * 5.60 - t * 1.95 + 0.6;
  float p4 = dot(p, vec2(0.64, -0.77)) * 7.10 + t * 2.25 + 3.1;

  float c0 = 0.105 * 1.45 * cos(p0);
  float c1 = 0.064 * 2.35 * cos(p1);
  float c2 = 0.034 * 3.80 * cos(p2);
  float c3 = 0.018 * 5.60 * cos(p3);
  float c4 = 0.009 * 7.10 * cos(p4);

  return c0 * vec2(0.78, 0.62)
       + c1 * vec2(-0.42, 0.91)
       + c2 * vec2(0.96, -0.28)
       + c3 * vec2(-0.18, -0.98)
       + c4 * vec2(0.64, -0.77);
}

// Shared compatibility helper used by other RenderChunk material functions.
float calculateFresnel(float cosR, float r0) {
  float f = 1.0 - clamp(cosR, 0.0, 1.0);
  float f2 = f * f;
  return r0 + (1.0 - r0) * f2 * f2 * f;
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

  // Keep the full wave detail close to the player and fade displacement
  // gradually at distance to avoid a hard transition at the horizon.
  float nearFade = 1.0 - smoothstep(18.0, 42.0, camDist);

  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      nrm = normalize(vec3(slope.x, -1.0, slope.y));
      wPos.y -= waveHeight * nearFade;
    #else
      nrm = vec3(0.0, -1.0, 0.0);
    #endif

    // Fine, smoothly blended ripples add small-scale normal variation.
    float rippleA = sin(dot(gPos.xz, vec2(3.2, -2.7)) + t * 2.8);
    float rippleB = cos(dot(gPos.xz, vec2(2.4, 3.6)) - t * 2.3);
    float ripple = rippleA * rippleB;
    nrm.xz += vec2(ripple, -ripple) * (0.045 * nearFade);
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
