#ifndef RAIN_H
#define RAIN_H

#include "clouds.h"
#include "detection.h"
#include "noise.h"
#include "sky.h"
#include "water.h"

float nlWindblow(vec3 pos, float t){
  vec2 p = pos.xy/(1.0+pos.z);
  float val = sin(4.0*p.x + 2.0*p.y + 2.0*t + 3.0*p.y*p.x)*sin(p.y*2.0 + 0.2*t);
  val += sin(p.y - p.x + 0.2*t);
  return 0.25*val*val;
}

// Stable world-space rain impact field. It creates small expanding rings
// without adding geometry or depending on texture UVs.
float eivorisRainImpact(vec2 worldXZ, float t) {
  vec2 cell = floor(worldXZ * 0.72);
  vec2 local = fract(worldXZ * 0.72) - 0.5;
  float seed = fastRand(cell);
  vec2 center = vec2(fract(seed * 17.13), fract(seed * 31.71)) - 0.5;
  float phase = fract(t * (1.15 + seed * 0.55) + seed);
  float radius = length(local - center);
  float ringRadius = phase * 0.62;
  float ring = exp(-abs(radius - ringRadius) * 72.0);
  float life = smoothstep(0.02, 0.16, phase) * (1.0 - smoothstep(0.78, 1.0, phase));
  float core = exp(-radius * radius * 95.0) * (1.0 - smoothstep(0.0, 0.12, phase));
  return clamp((ring * 0.72 + core * 0.28) * life, 0.0, 1.0);
}

vec4 nlRefl(
  inout vec4 color, nl_skycolor skycol, nl_environment env, vec3 viewDir, vec3 wPos, vec3 tiledCpos,
  vec3 CAMERA_POS, vec3 torchColor, vec2 lit, float camDist, float renderDist, highp float t
) {
  vec4 wetRefl = vec4(0.0,0.0,0.0,0.0);

  #ifndef NL_GROUND_REFL
  if (env.rainFactor > 0.0) {
  #endif

    float wetness = lit.y*lit.y;
    float endDist = renderDist*0.6;
    if (camDist < endDist) {
      float cosR = max(viewDir.y, 0.0);
      float puddles = max(1.0 - NL_GROUND_RAIN_PUDDLES*fastRand(tiledCpos.xz), 0.0);

      #ifndef NL_GROUND_REFL
        wetness *= puddles;
        float reflective = wetness*env.rainFactor*NL_GROUND_RAIN_WETNESS;
      #else
        float reflective = NL_GROUND_REFL;
        if (!env.end && !env.nether) reflective *= wetness;
        wetness *= puddles;
        reflective = mix(reflective, wetness, env.rainFactor);
      #endif

      if (wPos.y < 0.0) {
        // Slightly perturb the reflected direction with fine rain ripples.
        // The impact rings are world-anchored, so they do not swim with UVs.
        float impact = eivorisRainImpact((wPos + CAMERA_POS).xz, t);
        vec2 rippleDir = normalize((wPos + CAMERA_POS).xz + vec2(0.001));
        viewDir.xz += rippleDir * impact * 0.018 * env.rainFactor;
        viewDir = normalize(viewDir);
        viewDir.y = -viewDir.y;
        wetRefl.rgb = nlRenderSky(skycol, env, viewDir, t, false);

        #ifdef NL_CLOUD_AURORA_REFLECTION
          vec4 cloudRefl = nlCloudAuroraReflection(skycol, env, viewDir, wPos, CAMERA_POS, t);
          wetRefl.rgb = mix(wetRefl.rgb, cloudRefl.rgb, cloudRefl.a);
        #endif

        wetRefl.rgb += torchColor*lit.x*NL_TORCHLIGHT_INTENSITY;
        // Moving narrow glints make individual impact rings readable.
        float glint = pow(max(0.0, 1.0 - abs(impact - 0.52) * 5.0), 5.0);
        wetRefl.rgb += vec3(0.62, 0.76, 0.82) * glint * env.rainFactor * 0.16;

        wetRefl.a = calculateFresnel(cosR, 0.03)*reflective;
        wetRefl.a *= clamp(2.0-2.0*camDist/endDist, 0.0, 1.0);
        wetRefl.a *= 1.0 + impact * 0.22 * env.rainFactor;
      }
    }

    // Wet surfaces darken and become more reflective as rain accumulates.
    color.rgb *= 1.0 - 0.4*wetness*env.rainFactor;

  #ifndef NL_GROUND_REFL
  }
  #endif

  return wetRefl;
}

#endif
