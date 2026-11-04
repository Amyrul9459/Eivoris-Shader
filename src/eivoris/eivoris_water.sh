#ifndef EIVORIS_WATER_SH
#define EIVORIS_WATER_SH

// EIVORIS water field: dense, low-amplitude directional ripples.
// Geometry is deliberately not displaced: the water surface stays inside
// Minecraft's water-block boundary. All motion is carried by the normal field.

float eivorisWavePhase(vec2 p, vec2 direction, float wavelength, float speed, float time, float phase) {
    const float TAU = 6.28318530718;
    return TAU * dot(p, normalize(direction)) / wavelength - speed * time + phase;
}

vec2 eivorisWaterSlope(vec2 p, float time) {
    const float TAU = 6.28318530718;
    vec2 d0 = normalize(vec2(0.91, 0.41));
    vec2 d1 = normalize(vec2(-0.38, 0.92));
    vec2 d2 = normalize(vec2(0.67, -0.74));
    vec2 d3 = normalize(vec2(-0.96, -0.28));
    vec2 d4 = normalize(vec2(0.22, 0.98));
    vec2 d5 = normalize(vec2(-0.72, 0.69));

    float k0 = TAU / 2.8;
    float k1 = TAU / 2.05;
    float k2 = TAU / 1.48;
    float k3 = TAU / 1.08;
    float k4 = TAU / 0.78;
    float k5 = TAU / 0.56;

    float q0 = k0 * dot(p, d0) - 0.72 * time + 0.2;
    float q1 = k1 * dot(p, d1) - 0.91 * time + 1.7;
    float q2 = k2 * dot(p, d2) - 1.12 * time + 3.1;
    float q3 = k3 * dot(p, d3) - 1.31 * time + 4.4;
    float q4 = k4 * dot(p, d4) - 1.53 * time + 2.3;
    float q5 = k5 * dot(p, d5) - 1.76 * time + 5.1;

    return d0 * (0.030 * k0 * cos(q0))
         + d1 * (0.024 * k1 * cos(q1))
         + d2 * (0.017 * k2 * cos(q2))
         + d3 * (0.011 * k3 * cos(q3))
         + d4 * (0.006 * k4 * cos(q4))
         + d5 * (0.0035 * k5 * cos(q5));
}

vec3 eivorisWaterNormal(vec2 worldXZ, float time) {
    vec2 slope = eivorisWaterSlope(worldXZ, time);
    // A stable, low-slope normal keeps the water calm while retaining
    // clearly visible close-range highlights and fine ripple movement.
    return normalize(vec3(-slope.x, 1.0, -slope.y));
}

float eivorisWaterFresnel(float ndv) {
    float f = 1.0 - clamp(ndv, 0.0, 1.0);
    float f2 = f * f;
    return 0.025 + 0.975 * f2 * f2 * f;
}

vec3 eivorisWaterSky(vec3 reflectedDir, vec3 fogColor, float daylight) {
    float up = clamp(reflectedDir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 zenith = mix(vec3(0.018, 0.055, 0.085), vec3(0.16, 0.34, 0.55), daylight);
    vec3 horizon = mix(vec3(0.035, 0.075, 0.095), max(fogColor, vec3(0.0)), 0.68);
    return mix(horizon, zenith, pow(up, 0.72));
}

#endif
