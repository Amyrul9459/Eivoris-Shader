#ifndef EIVORIS_WATER_SH
#define EIVORIS_WATER_SH

// EIVORIS water field.
// Multi-directional Gerstner-style height field with a matching analytic
// slope. Geometry displacement is applied in RenderChunk vertex.sc;
// the fragment stage reuses the same field for stable moving highlights.

#define EIVORIS_WATER_PI 3.14159265359
#define EIVORIS_WATER_TAU 6.28318530718

float eivorisWave(vec2 p, vec2 direction, float wavelength, float amplitude, float speed, float time, float phase) {
    vec2 d = normalize(direction);
    float k = EIVORIS_WATER_TAU / wavelength;
    float q = k * dot(p, d) - speed * time + phase;
    return amplitude * sin(q);
}

vec2 eivorisWaveSlope(vec2 p, vec2 direction, float wavelength, float amplitude, float speed, float time, float phase) {
    vec2 d = normalize(direction);
    float k = EIVORIS_WATER_TAU / wavelength;
    float q = k * dot(p, d) - speed * time + phase;
    return d * (amplitude * k * cos(q));
}

float eivorisWaterHeight(vec2 p, float time) {
    float h = 0.0;
    h += eivorisWave(p, vec2( 1.00,  0.34), 4.8, 0.040, 0.72, time, 0.4);
    h += eivorisWave(p, vec2(-0.42,  1.00), 3.1, 0.030, 0.91, time, 2.1);
    h += eivorisWave(p, vec2( 0.76, -0.65), 2.1, 0.020, 1.12, time, 4.0);
    h += eivorisWave(p, vec2(-0.91, -0.28), 1.35, 0.010, 1.38, time, 5.2);
    return h;
}

vec2 eivorisWaterSlope(vec2 p, float time) {
    vec2 s = vec2(0.0);
    s += eivorisWaveSlope(p, vec2( 1.00,  0.34), 4.8, 0.040, 0.72, time, 0.4);
    s += eivorisWaveSlope(p, vec2(-0.42,  1.00), 3.1, 0.030, 0.91, time, 2.1);
    s += eivorisWaveSlope(p, vec2( 0.76, -0.65), 2.1, 0.020, 1.12, time, 4.0);
    s += eivorisWaveSlope(p, vec2(-0.91, -0.28), 1.35, 0.010, 1.38, time, 5.2);

    // Fine-scale normal ripples remain visible even where the block mesh
    // has too few vertices to describe the full wave silhouette.
    float r0 = 2.0 * dot(p, normalize(vec2(0.8, 0.6))) - 1.65 * time;
    float r1 = 3.1 * dot(p, normalize(vec2(-0.5, 0.86))) + 1.9 * time;
    s += normalize(vec2(0.8, 0.6)) * (0.010 * cos(r0));
    s += normalize(vec2(-0.5, 0.86)) * (0.006 * cos(r1));
    return s;
}

vec3 eivorisWaterNormal(vec2 worldXZ, float time) {
    vec2 slope = eivorisWaterSlope(worldXZ, time);
    return normalize(vec3(-slope.x, 1.0, -slope.y));
}

float eivorisWaterFresnel(float ndv) {
    float f = 1.0 - clamp(ndv, 0.0, 1.0);
    return 0.025 + 0.975 * f * f * f * f * f;
}

vec3 eivorisWaterSky(vec3 reflectedDir, vec3 fogColor, float daylight) {
    float up = clamp(reflectedDir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 zenith = mix(vec3(0.012, 0.035, 0.060), vec3(0.12, 0.29, 0.50), daylight);
    vec3 horizon = mix(vec3(0.025, 0.060, 0.075), max(fogColor, vec3(0.0)), 0.72);
    return mix(horizon, zenith, pow(up, 0.78));
}

#endif
