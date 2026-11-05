#version 120

/*
 * EIVORIS Water — Java shaderpack prototype
 * Goals:
 * - subtle multi-directional animated surface distortion
 * - cool, clear water tint
 * - preserve vanilla texture alpha
 * - deliberately no reflection, cubemap, SSR, or Fresnel reflection
 */

uniform sampler2D texture;
uniform float frameTimeCounter;

varying vec2 vTexCoord;
varying vec4 vVertexColor;
varying vec3 vViewPosition;

const float WAVE_UV_STRENGTH = 0.0017;
const float WATER_TINT_STRENGTH = 0.24;

vec2 waterDistortion(vec2 uv, vec3 viewPos, float time) {
    float waveA = sin(uv.x * 54.0 + time * 1.25 + viewPos.x * 0.025);
    float waveB = cos(uv.y * 47.0 - time * 1.05 + viewPos.z * 0.022);
    float waveC = sin((uv.x + uv.y) * 31.0 + time * 0.72);

    return vec2(
        waveA + waveC * 0.35,
        waveB - waveC * 0.30
    ) * WAVE_UV_STRENGTH;
}

void main() {
    vec2 animatedUV = vTexCoord + waterDistortion(
        vTexCoord,
        vViewPosition,
        frameTimeCounter
    );

    vec4 waterSample = texture2D(texture, animatedUV) * vVertexColor;

    // Slightly cool the water without replacing its original texture detail.
    vec3 clearWaterTint = vec3(0.72, 0.91, 0.96);
    vec3 waterColor = mix(
        waterSample.rgb,
        waterSample.rgb * clearWaterTint,
        WATER_TINT_STRENGTH
    );

    // Preserve the engine's alpha so vanilla water transparency remains intact.
    gl_FragColor = vec4(waterColor, waterSample.a);
}
