#ifndef EIVORIS_WATER_MODULE_H
#define EIVORIS_WATER_MODULE_H

// EIVORIS WATER MODEL. Standalone GLSL-style source; no Newb dependencies.

struct EivorisWaterState {
  vec3 displacement;
  vec3 normal;
  float crest;
};

EivorisWaterState eivorisWaterSurface(vec2 worldXZ, float time) {
  vec2 p = worldXZ * 0.34;
  vec2 d0 = normalize(vec2(0.83, 0.55));
  vec2 d1 = normalize(vec2(-0.38, 0.92));
  vec2 d2 = normalize(vec2(0.24, -0.97));

  float k0 = 6.28318530718 / 18.0;
  float k1 = 6.28318530718 / 10.5;
  float k2 = 6.28318530718 / 6.8;
  float a0 = 0.115;
  float a1 = 0.052;
  float a2 = 0.018;

  float q0 = k0 * dot(d0, p) - 0.72 * time + 0.4;
  float q1 = k1 * dot(d1, p) - 0.91 * time + 2.1;
  float q2 = k2 * dot(d2, p) - 1.17 * time + 4.0;
  float h = a0 * sin(q0) + a1 * sin(q1) + a2 * sin(q2);

  // Exact gradient of the height field; normal and displacement agree.
  vec2 grad = a0*k0*cos(q0)*d0
            + a1*k1*cos(q1)*d1
            + a2*k2*cos(q2)*d2;

  // Fine detail affects normals only, never vertex displacement.
  float r0 = sin(dot(p, vec2(1.7, 2.3)) - 1.8 * time);
  float r1 = cos(dot(p, vec2(-2.6, 1.2)) - 1.35 * time);
  vec2 rippleGrad = vec2(r0*r1, r1-r0) * 0.018;
  vec2 totalGrad = grad + rippleGrad;

  EivorisWaterState result;
  result.displacement = vec3(0.0, h, 0.0);
  result.normal = normalize(vec3(-totalGrad.x, 1.0, -totalGrad.y));
  result.crest = clamp(0.5 + 0.5*sin(q0), 0.0, 1.0);
  return result;
}

float eivorisWaterFresnel(float nDotV, float f0) {
  float f = 1.0 - clamp(nDotV, 0.0, 1.0);
  float f2 = f*f;
  return f0 + (1.0-f0)*f2*f2*f;
}

vec3 eivorisWaterColor(vec3 deepColor, vec3 shallowColor, float depthFactor) {
  return mix(deepColor, shallowColor, smoothstep(0.05, 0.92, depthFactor));
}

#endif
