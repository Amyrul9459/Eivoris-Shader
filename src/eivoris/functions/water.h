#ifndef WATER_H
#define WATER_H

#include "utils.h"
#include "detection.h"
#include "sky.h"

// EIVORIS multi-directional water field.
// Small vertex displacement + dense analytic normal ripples keeps the
// surface animated near the camera without exaggerated block-edge waves.
float eivorisWaveHeight(vec2 p, float t) {
  const float TAU = 6.28318530718;
  vec2 d0 = normalize(vec2(0.82, 0.57));
  vec2 d1 = normalize(vec2(-0.36, 0.93));
  vec2 d2 = normalize(vec2(0.25, -0.97));
  vec2 d3 = normalize(vec2(-0.91, -0.41));
  vec2 d4 = normalize(vec2(0.68, -0.73));
  float q0 = TAU * dot(p,d0) / 17.0 - 0.68*t + 0.4;
  float q1 = TAU * dot(p,d1) / 10.0 - 0.86*t + 2.1;
  float q2 = TAU * dot(p,d2) / 6.2 - 1.08*t + 4.0;
  float q3 = TAU * dot(p,d3) / 3.8 - 1.27*t + 1.3;
  float q4 = TAU * dot(p,d4) / 2.5 - 1.43*t + 2.7;
  return 0.112*sin(q0) + 0.054*sin(q1) + 0.026*sin(q2)
       + 0.013*sin(q3) + 0.006*sin(q4);
}

vec2 eivorisWaveSlope(vec2 p, float t) {
  const float TAU = 6.28318530718;
  vec2 d0 = normalize(vec2(0.82,0.57));
  vec2 d1 = normalize(vec2(-0.36,0.93));
  vec2 d2 = normalize(vec2(0.25,-0.97));
  vec2 d3 = normalize(vec2(-0.91,-0.41));
  vec2 d4 = normalize(vec2(0.68,-0.73));
  float k0=TAU/17.0, k1=TAU/10.0, k2=TAU/6.2, k3=TAU/3.8, k4=TAU/2.5;
  float q0=k0*dot(p,d0)-0.68*t+0.4;
  float q1=k1*dot(p,d1)-0.86*t+2.1;
  float q2=k2*dot(p,d2)-1.08*t+4.0;
  float q3=k3*dot(p,d3)-1.27*t+1.3;
  float q4=k4*dot(p,d4)-1.43*t+2.7;
  return 0.112*k0*cos(q0)*d0 + 0.054*k1*cos(q1)*d1
       + 0.026*k2*cos(q2)*d2 + 0.013*k3*cos(q3)*d3
       + 0.006*k4*cos(q4)*d4;
}

// High-frequency crossing ripples alter normals, not vertex position.
vec2 eivorisFineRippleSlope(vec2 p, float t) {
  const float TAU = 6.28318530718;
  vec2 d0=normalize(vec2(0.71,0.70));
  vec2 d1=normalize(vec2(-0.88,0.47));
  vec2 d2=normalize(vec2(0.31,-0.95));
  float k0=TAU/2.15, k1=TAU/1.42, k2=TAU/0.96;
  float q0=k0*dot(p,d0)-1.65*t+0.8;
  float q1=k1*dot(p,d1)-1.92*t+2.4;
  float q2=k2*dot(p,d2)-2.18*t+4.6;
  return 0.010*k0*cos(q0)*d0 + 0.006*k1*cos(q1)*d1
       + 0.003*k2*cos(q2)*d2;
}

float calculateFresnel(float cosR, float r0) {
  float f=1.0-clamp(cosR,0.0,1.0);
  float f2=f*f;
  return r0+(1.0-r0)*f2*f2*f;
}
float eivorisFresnel(float cosR, float r0) {
  return calculateFresnel(cosR,r0);
}

vec4 nlWater(
  inout vec4 color, inout vec3 wPos, nl_skycolor skycol, nl_environment env, vec4 COLOR, vec3 viewDir,
  vec3 cPos, vec3 tiledCpos, vec3 gPos, vec3 CAMERA_POS, vec3 light, vec3 torchColor, vec2 lit,
  float fractCposY, float camDist, highp float t
) {
  vec2 slope=eivorisWaveSlope(gPos.xz,t);
  vec2 fineSlope=eivorisFineRippleSlope(gPos.xz,t);
  float height=eivorisWaveHeight(gPos.xz,t);

  // Keep the full close-up normal detail. Fade displacement only at distance.
  float nearFade=1.0-smoothstep(64.0,112.0,camDist);
  float normalFade=mix(0.42,1.0,nearFade);
  vec3 nrm;
  if (fractCposY > 0.0) {
    #ifndef NO_WAVE
      nrm=normalize(vec3(slope.x+fineSlope.x*normalFade,-1.0,
                         slope.y+fineSlope.y*normalFade));
      wPos.y-=height*nearFade;
    #else
      nrm=vec3(0.0,-1.0,0.0);
    #endif
  } else {
    vec2 side=(slope+fineSlope*0.55)*0.32;
    nrm=normalize(vec3(normalize(viewDir.xz)+side,-0.12));
  }

  float cosR=dot(nrm,viewDir);
  viewDir=viewDir-2.0*cosR*nrm;
  // Existing RenderDragon sky reflection; deliberately no cloud reflection.
  vec3 waterRefl=nlRenderSky(skycol,env,viewDir,t,false);
  float tc=0.5+0.5*sin(16.0*viewDir.x)*sin(16.0*viewDir.z);
  waterRefl+=torchColor*NL_TORCHLIGHT_INTENSITY*lit.x*tc*tc;
  if (!env.end) waterRefl*=0.05+lit.y*1.14;

  cosR=abs(cosR);
  float fresnel=eivorisFresnel(cosR,0.075);
  float opacity=1.0-cosR;
  color.rgb*=0.22*NL_WATER_TINT*(1.0-0.76*fresnel);
  color.a=mix(COLOR.a*NL_WATER_TRANSPARENCY,1.0,opacity*opacity);
  return vec4(waterRefl,fresnel);
}
#endif
