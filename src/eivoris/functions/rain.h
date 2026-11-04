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
// Animated, world-anchored circular rain marks on block surfaces.
// This is a surface shader effect, not an engine particle emitter.
float eivorisRainImpact(vec2 worldXZ, float t) {
  vec2 cell = floor(worldXZ * 0.82);
  vec2 local = fract(worldXZ * 0.82) - 0.5;
  float seed = fastRand(cell);
  vec2 center = (vec2(fract(seed * 17.13), fract(seed * 31.71)) - 0.5) * 0.66;
  float phase = fract(t * (0.82 + seed * 0.34) + seed);
  float radius = length(local - center);

  float radiusA = 0.04 + phase * 0.43;
  float radiusB = 0.04 + fract(phase + 0.48) * 0.43;
  float ringA = exp(-abs(radius - radiusA) * 92.0);
  float ringB = exp(-abs(radius - radiusB) * 82.0);
  float lifeA = smoothstep(0.02, 0.12, phase) * (1.0 - smoothstep(0.82, 1.0, phase));
  float lifeB = smoothstep(0.02, 0.12, fract(phase + 0.48))
              * (1.0 - smoothstep(0.82, 1.0, fract(phase + 0.48)));
  float centerDimple = exp(-radius * radius * 150.0)
                     * (1.0 - smoothstep(0.0, 0.18, phase));
  return clamp(ringA * lifeA * 0.62 + ringB * lifeB * 0.34
             + centerDimple * 0.22, 0.0, 1.0);
}

vec4 nlRefl(
  inout vec4 color, nl_skycolor skycol, nl_environment env, vec3 viewDir, vec3 wPos, vec3 tiledCpos,
  vec3 CAMERA_POS, vec3 torchColor, vec2 lit, float camDist, float renderDist, highp float t
) {
  vec4 wetRefl = vec4(0.0,0.0,0.0,0.0);

  // Permanent subtle wet finish: independent of the current weather state.
  float wetness = lit.y * lit.y;
  float endDist = renderDist * 0.6;
  if (camDist < endDist && wPos.y < 0.0) {
    float cosR = max(viewDir.y, 0.0);
    float puddles = max(1.0 - NL_GROUND_RAIN_PUDDLES * fastRand(tiledCpos.xz), 0.0);
    float reflective = wetness * puddles * 0.22;

    // Rain circles animate only while raining, while the wet material stays permanent.
    float impact = eivorisRainImpact((wPos + CAMERA_POS).xz, t) * env.rainFactor;
    if (impact > 0.001) {
      color.rgb += vec3(0.24, 0.34, 0.39) * impact * 0.22;
      vec2 rippleDir = normalize((wPos + CAMERA_POS).xz + vec2(0.001));
      viewDir.xz += rippleDir * impact * 0.014;
      viewDir = normalize(viewDir);
    }

    viewDir.y = -viewDir.y;
    wetRefl.rgb = nlRenderSky(skycol, env, viewDir, t, false);

    #ifdef NL_CLOUD_AURORA_REFLECTION
      vec4 cloudRefl = nlCloudAuroraReflection(skycol, env, viewDir, wPos, CAMERA_POS, t);
      wetRefl.rgb = mix(wetRefl.rgb, cloudRefl.rgb, cloudRefl.a);
    #endif

    wetRefl.rgb += torchColor * lit.x * NL_TORCHLIGHT_INTENSITY;
    float glint = pow(max(0.0, 1.0 - abs(impact - 0.48) * 5.0), 5.0);
    wetRefl.rgb += vec3(0.62, 0.76, 0.82) * glint * env.rainFactor * 0.18;

    wetRefl.a = calculateFresnel(cosR, 0.03) * reflective;
    wetRefl.a *= clamp(2.0 - 2.0 * camDist / endDist, 0.0, 1.0);
    wetRefl.a *= 1.0 + impact * 0.18;
  }

  // Slight permanent darkening gives stone, soil and other blocks a damp finish.
  color.rgb *= 1.0 - 0.13 * wetness;
  return wetRefl;
}

#endif
