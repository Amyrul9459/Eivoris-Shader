#ifndef EIVORIS_RAIN_MODULE_H
#define EIVORIS_RAIN_MODULE_H

// Rain amount is an engine-provided 0..1 input. Particle geometry is
// handled by a future EIVORIS Weather material entry point.

float eivorisRainAmount(float engineRain) {
  return smoothstep(0.02, 0.85, clamp(engineRain, 0.0, 1.0));
}

float eivorisRainWetness(float engineRain, float surfaceUpness) {
  float rain = eivorisRainAmount(engineRain);
  float exposed = smoothstep(0.12, 0.95, clamp(surfaceUpness, 0.0, 1.0));
  return rain * exposed * 0.72;
}

vec3 eivorisRainSurface(vec3 baseColor, vec3 normal, float engineRain, out float roughness) {
  float wet = eivorisRainWetness(engineRain, normalize(normal).y);
  roughness = mix(0.88, 0.24, wet);
  return baseColor * (1.0 - 0.13 * wet);
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
