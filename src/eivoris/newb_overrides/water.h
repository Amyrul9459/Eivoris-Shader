#ifndef WATER_H
#define WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"
#include "clouds.h"
#include "noise.h"

// EIVORIS water field: four travelling wave directions.
// The block mesh limits silhouette detail; the directional gradient below
// supplies the close-range moving normal detail the mesh cannot represent.
float eivorisWaterHeight(vec2 p, float t) {
  float h = 0.0;
  h += 0.045*sin(dot(p, normalize(vec2(1.0, 0.32))*1.45) - 0.72*t);
  h += 0.030*sin(dot(p, normalize(vec2(-0.38, 1.0))*2.05) - 0.91*t + 1.7);
  h += 0.018*sin(dot(p, normalize(vec2(0.72, -0.69))*3.10) - 1.13*t + 3.2);
  h += 0.010*sin(dot(p, normalize(vec2(-0.92, -0.25))*4.65) - 1.42*t + 4.6);
  return h;
}

vec2 eivorisWaterGradient(vec2 p, float t) {
  vec2 g = vec2(0.0);
  vec2 d0 = normalize(vec2(1.0, 0.32));
  vec2 d1 = normalize(vec2(-0.38, 1.0));
  vec2 d2 = normalize(vec2(0.72, -0.69));
  vec2 d3 = normalize(vec2(-0.92, -0.25));
  float q0 = dot(p, d0)*1.45 - 0.72*t;
  float q1 = dot(p, d1)*2.05 - 0.91*t + 1.7;
  float q2 = dot(p, d2)*3.10 - 1.13*t + 3.2;
  float q3 = dot(p, d3)*4.65 - 1.42*t + 4.6;
  g += d0*(0.045*1.45*cos(q0));
  g += d1*(0.030*2.05*cos(q1));
  g += d2*(0.018*3.10*cos(q2));
  g += d3*(0.010*4.65*cos(q3));
  // Small moving interference ripples remain visible close to the player.
  float r0 = dot(p, normalize(vec2(0.8, 0.6)))*8.0 - 2.0*t;
  float r1 = dot(p, normalize(vec2(-0.5, 0.86)))*11.0 + 1.55*t;
  g += normalize(vec2(0.8, 0.6))*(0.012*cos(r0));
  g += normalize(vec2(-0.5, 0.86))*(0.008*cos(r1));
  return g;
}

// Schlick Fresnel.
float calculateFresnel(float cosR, float r0) {
  float a = 1.0-cosR;
  float a2 = a*a;
  return r0 + (1.0-r0)*a2*a2*a;
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  vec2 gradient = eivorisWaterGradient(gPos.xz, t);
  vec2 bump = gradient*2.4;
  // Retain a small stochastic component to break up mathematically perfect waves.
  float micro = movingNoise2D(gPos.xz + gPos.yy, NL_WATER_WAVE_SPEED*t, 0.6)-0.5;
  bump += vec2(micro, -micro*0.72)*0.20;

  vec3 nrm;
  if (fractCposY > 0.0) {
    nrm.xz = bump*NL_WATER_BUMP;
    nrm.y = -1.0;
  } else {
    bump *= 0.5 + 0.5*sin(3.0*t*NL_WATER_WAVE_SPEED + cPos.y*PI_HALF);
    nrm.xz = normalize(viewDir.xz) + bump.y*(1.0-viewDir.xz*viewDir.xz)*NL_WATER_BUMP;
    nrm.y = bump.x*NL_WATER_BUMP;
  }
  nrm = normalize(nrm);

  float cosR = dot(nrm, viewDir);
  viewDir = viewDir - 2.0*cosR*nrm;
  vec3 waterRefl = nlRenderSky(skycol, env, viewDir, t, false);

  // Cloud/aurora reflection is intentionally excluded for EIVORIS.
  float tc = 0.5+0.5*sin(16.0*viewDir.x)*sin(16.0*viewDir.z);
  waterRefl += torchColor*NL_TORCHLIGHT_INTENSITY*lit.x*tc*tc;
  if (!env.end) waterRefl *= 0.05 + lit.y*1.14;

  cosR = abs(cosR);
  float fresnel = calculateFresnel(cosR, 0.035);
  float opacity = 1.0-cosR;
  color.rgb *= 0.22*NL_WATER_TINT*(1.0-0.8*fresnel);
  color.a = mix(COLOR.a*NL_WATER_TRANSPARENCY, 1.0, opacity*opacity);

  // Real vertex displacement on the existing water mesh. The engine's mesh
  // density still limits silhouette smoothness; fragment normals carry detail.
  #ifdef NL_WATER_WAVE
    if (camDist < 20.0) {
      wPos.y += eivorisWaterHeight(gPos.xz, t)*0.72;
    }
  #endif

  return vec4(waterRefl, fresnel);
}
#endif
