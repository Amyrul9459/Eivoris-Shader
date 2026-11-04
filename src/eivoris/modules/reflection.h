#ifndef EIVORIS_REFLECTION_MODULE_H
#define EIVORIS_REFLECTION_MODULE_H

// Environment color must be supplied by the material/render pipeline.
// This is a reflection response model, not screen-space reflection.

float eivorisSchlickFresnel(float nDotV, float f0) {
  float f = 1.0 - clamp(nDotV, 0.0, 1.0);
  float f2 = f*f;
  return f0 + (1.0-f0)*f2*f2*f;
}

vec3 eivorisReflection(vec3 environmentColor, vec3 surfaceColor,
                       float nDotV, float roughness, float reflectivity) {
  float fresnel = eivorisSchlickFresnel(nDotV, 0.035);
  float gloss = 1.0 - clamp(roughness, 0.0, 1.0);
  float strength = clamp(reflectivity, 0.0, 1.0) * fresnel;
  vec3 softenedEnvironment = mix(surfaceColor, environmentColor, 0.55 + 0.45*gloss);
  return mix(surfaceColor, softenedEnvironment, strength);
}

#endif
