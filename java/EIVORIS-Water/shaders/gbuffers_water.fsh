#version 120

/*
 * EIVORIS Water — Java shaderpack
 * Animated multi-scale wave normals, clear water tint and soft crest shading.
 * No SSR, cubemap, environment reflection, or Fresnel reflection.
 */

uniform sampler2D texture;
uniform float frameTimeCounter;

varying vec2 vTexCoord;
varying vec4 vVertexColor;
varying vec3 vWorldPosition;

const float WAVE_TINT = 0.30;

float waveHeight(vec2 p, float t) {
    float a = sin(dot(p, normalize(vec2( 1.00,  0.32))) * 0.42 + t * 1.05);
    float b = sin(dot(p, normalize(vec2(-0.38,  1.00))) * 0.68 - t * 0.82);
    float c = sin(dot(p, normalize(vec2( 0.72, -0.69))) * 1.12 + t * 0.63);
    float d = sin(dot(p, normalize(vec2(-0.92, -0.28))) * 1.75 - t * 1.24);
    return a * 0.105 + b * 0.052 + c * 0.021 + d * 0.009;
}

void main() {
    vec4 texel = texture2D(texture, vTexCoord) * vVertexColor;

    // Analytic surface normal from the same height field used by the vertex pass.
    float e = 0.18;
    float hL = waveHeight(vWorldPosition.xz - vec2(e, 0.0), frameTimeCounter);
    float hR = waveHeight(vWorldPosition.xz + vec2(e, 0.0), frameTimeCounter);
    float hD = waveHeight(vWorldPosition.xz - vec2(0.0, e), frameTimeCounter);
    float hU = waveHeight(vWorldPosition.xz + vec2(0.0, e), frameTimeCounter);

    vec3 waveNormal = normalize(vec3(
        -(hR - hL) / (2.0 * e),
        1.0,
        -(hU - hD) / (2.0 * e)
    ));

    float crest = clamp(waveNormal.y, 0.0, 1.0);
    float waveLight = 0.94 + (1.0 - crest) * 0.10;

    vec3 waterTint = vec3(0.76, 0.92, 0.97);
    vec3 waterColor = mix(texel.rgb, texel.rgb * waterTint, WAVE_TINT);
    waterColor *= waveLight;

    // Keep vanilla water alpha unchanged; no reflective term is added.
    gl_FragColor = vec4(waterColor, texel.a);
}
