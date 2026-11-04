#ifndef EIVORIS_LIGHTING_MODULE_H
#define EIVORIS_LIGHTING_MODULE_H

// EIVORIS world-light response. All engine-specific inputs are explicit.

struct EivorisLightingInput {
  vec3 albedo;
  vec3 normal;
  vec3 viewDirection;
  vec3 sunDirection;
  vec3 sunColor;
  vec3 skyIrradiance;
  vec3 blockLightColor;
  float sunIntensity;
  float blockLight;
  float ambientOcclusion;
  float roughness;
};

vec3 eivorisLighting(EivorisLightingInput s) {
  vec3 N = normalize(s.normal);
  vec3 V = normalize(s.viewDirection);
  vec3 L = normalize(s.sunDirection);
  float nDotV = max(dot(N, V), 0.001);

  float wrappedDiffuse = max((dot(N, L) + 0.18) / 1.18, 0.0);
  vec3 direct = s.sunColor * wrappedDiffuse * s.sunIntensity;
  float skyFacing = 0.35 + 0.65 * max(N.y, 0.0);
  vec3 ambient = s.skyIrradiance * skyFacing;
  float ao = mix(1.0, clamp(s.ambientOcclusion, 0.0, 1.0), 0.72);
  vec3 block = s.blockLightColor * max(s.blockLight, 0.0);

  vec3 result = s.albedo * ((direct + ambient) * ao + block);
  result += s.albedo * (1.0 - nDotV) * 0.025;
  return result;
}

#endif
