#ifndef EIVORIS_EFFECTS_MODULE_H
#define EIVORIS_EFFECTS_MODULE_H

// EIVORIS tone response and atmospheric helpers.

vec3 eivorisACES(vec3 x) {
  const float a = 2.51;
  const float b = 0.03;
  const float c = 2.43;
  const float d = 0.59;
  const float e = 0.14;
  return clamp((x*(a*x+b))/(x*(c*x+d)+e), 0.0, 1.0);
}

vec3 eivorisColorGrade(vec3 color, float exposure, float saturation) {
  vec3 exposed = max(color * max(exposure, 0.0), vec3(0.0));
  float luma = dot(exposed, vec3(0.2126, 0.7152, 0.0722));
  return eivorisACES(mix(vec3(luma), exposed, max(saturation, 0.0)));
}

vec3 eivorisAtmosphericFog(vec3 sceneColor, vec3 fogColor,
                           float distanceToCamera, float density) {
  float transmittance = exp(-max(density, 0.0) * max(distanceToCamera, 0.0));
  return mix(fogColor, sceneColor, clamp(transmittance, 0.0, 1.0));
}

#endif
