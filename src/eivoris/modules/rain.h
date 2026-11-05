#ifndef EIVORIS_RAIN_MODULE_H
#define EIVORIS_RAIN_MODULE_H

// EIVORIS rain response.
// Rain affects atmospheric haze only. It deliberately does not modify
// block albedo, roughness, normals, or add wet-surface/puddle overlays.

float eivorisRainAmount(float engineRain) {
  return smoothstep(0.02, 0.85, clamp(engineRain, 0.0, 1.0));
}

float eivorisRainHaze(float engineRain, float distanceToCamera) {
  float rain = eivorisRainAmount(engineRain);
  float distanceFade = 1.0 - exp(-max(distanceToCamera, 0.0) * 0.012);
  return clamp(rain * distanceFade * 0.32, 0.0, 0.32);
}

vec3 eivorisRainAtmosphere(vec3 sceneColor, vec3 overcastColor, float haze) {
  return mix(sceneColor, overcastColor, clamp(haze, 0.0, 1.0));
}

#endif
