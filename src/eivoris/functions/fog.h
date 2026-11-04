#ifndef FOG_H
#define FOG_H

float nlRenderFogFade(float relativeDist, vec3 FOG_COLOR, vec2 FOG_CONTROL) {
  #ifdef NL_FOG
    float fade = smoothstep(FOG_CONTROL.x, FOG_CONTROL.y, relativeDist);
    float density = NL_MIST_DENSITY*(19.0 - 18.0*FOG_COLOR.g);
    fade += (1.0-fade)*(0.3-0.3*exp(-relativeDist*relativeDist*density));
    return NL_FOG * fade;
  #else
    return 0.0;
  #endif
}

// EIVORIS volumetric-style sun shafts.
// Eight integration samples approximate light scattering through the fog volume.
// This is evaluated per vertex by RenderChunk, so it is intentionally lightweight.
float nlRenderGodRayIntensity(vec3 cPos, vec3 worldPos, float t, vec2 uv1, float relativeDist, vec3 FOG_COLOR) {
  vec3 rayDir = normalize(worldPos);
  float rayLength = min(length(worldPos), 96.0);
  float dawnMask = clamp(2.8*(FOG_COLOR.r-FOG_COLOR.b), 0.0, 1.0);
  float daylightMask = smoothstep(0.04, 0.34, FOG_COLOR.r + FOG_COLOR.g - FOG_COLOR.b);
  float weatherMask = clamp(1.0 - 1.4*FOG_COLOR.g, 0.25, 1.0);

  // Approximate sun-facing phase term. The animated axis keeps the shafts
  // moving with the day rather than behaving like a static screen overlay.
  vec3 sunAxis = normalize(vec3(0.28 + 0.10*sin(0.025*t), 0.88, 0.34));
  float phase = pow(max(dot(rayDir, sunAxis), 0.0), 5.0);
  phase = max(phase, 0.10*pow(max(dot(rayDir, -sunAxis), 0.0), 2.0));

  float stepLength = rayLength * 0.125;
  float scattering = 0.0;
  for (int i = 0; i < 8; ++i) {
    float fi = (float(i) + 0.5) * 0.125;
    vec3 p = rayDir * (rayLength * fi) + cPos * 0.03125;
    float nA = sin(dot(p, vec3(0.071, 0.113, 0.097)) + t*0.11);
    float nB = sin(dot(p, vec3(-0.137, 0.053, 0.163)) - t*0.075);
    float bands = 0.5 + 0.5*sin(p.x*0.19 + p.z*0.13 + 2.2*nA + t*0.035);
    float cloudDensity = smoothstep(0.12, 0.88, 0.5 + 0.25*nA + 0.25*nB);
    float shaftMask = mix(0.22, 1.0, bands*bands);
    float heightFalloff = exp(-max(p.y, 0.0)*0.0025);
    scattering += cloudDensity * shaftMask * heightFalloff;
  }

  scattering *= stepLength / 12.0;
  scattering *= phase * (0.35 + 0.65*uv1.y);
  scattering *= smoothstep(0.08, 0.75, relativeDist);
  scattering *= mix(0.30, 1.0, dawnMask);
  scattering *= daylightMask * weatherMask;
  return clamp(scattering, 0.0, 1.0);
}

#endif
