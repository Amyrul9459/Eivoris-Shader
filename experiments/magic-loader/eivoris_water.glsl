/*
 * EIVORIS — Java shader water math prototype
 * Experimental source for the Magic Loader lab.
 *
 * This file contains only portable GLSL-style math. It is NOT a complete
 * OptiFine/Iris shaderpack pass: it still needs the target pass's uniforms,
 * coordinate transforms, varyings, and integration code.
 *
 * Original EIVORIS-authored wave model; no Complementary source or assets.
 */

#ifndef EIVORIS_JAVA_WATER_GLSL
#define EIVORIS_JAVA_WATER_GLSL

const float EIVORIS_PI = 3.14159265358979323846;

float eivorisWaveHeight(vec2 worldXZ, float timeSeconds) {
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

    float q0 = k0 * dot(d0, p) - 0.72 * timeSeconds + 0.4;
    float q1 = k1 * dot(d1, p) - 0.91 * timeSeconds + 2.1;
    float q2 = k2 * dot(d2, p) - 1.17 * timeSeconds + 4.0;

    return a0 * sin(q0) + a1 * sin(q1) + a2 * sin(q2);
}

vec3 eivorisWaveNormal(vec2 worldXZ, float timeSeconds) {
    vec2 p = worldXZ * 0.34;

    vec2 d0 = normalize(vec2(0.83, 0.55));
    vec2 d1 = normalize(vec2(-0.38, 0.92));
    vec2 d2 = normalize(vec2(0.24, -0.97));

    float k0 = 6.28318530718 / 18.0;
    float k1 = 6.28318530718 / 10.5;
    float k2 = 6.28318530718 / 6.8;

    float q0 = k0 * dot(d0, p) - 0.72 * timeSeconds + 0.4;
    float q1 = k1 * dot(d1, p) - 0.91 * timeSeconds + 2.1;
    float q2 = k2 * dot(d2, p) - 1.17 * timeSeconds + 4.0;

    vec2 gradient =
          0.115 * k0 * cos(q0) * d0
        + 0.052 * k1 * cos(q1) * d1
        + 0.018 * k2 * cos(q2) * d2;

    // High-frequency detail changes the normal only, never the height.
    float r0 = sin(dot(p, vec2(1.7, 2.3)) - 1.8 * timeSeconds);
    float r1 = cos(dot(p, vec2(-2.6, 1.2)) - 1.35 * timeSeconds);
    vec2 fineGradient = vec2(r0 * r1, r1 - r0) * 0.018;

    vec2 totalGradient = gradient + fineGradient;
    return normalize(vec3(-totalGradient.x, 1.0, -totalGradient.y));
}

float eivorisWaterFresnel(float nDotV, float f0) {
    float f = 1.0 - clamp(nDotV, 0.0, 1.0);
    float f2 = f * f;
    return f0 + (1.0 - f0) * f2 * f2 * f;
}

vec3 eivorisWaterTint(vec3 deepColor, vec3 shallowColor, float depthFactor) {
    return mix(deepColor, shallowColor, smoothstep(0.05, 0.92, depthFactor));
}

#endif
