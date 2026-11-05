#version 120

/*
 * EIVORIS Underwater Post Process
 * Depth absorption + soft screen-space caustic shimmer.
 * Does not generate or sample reflections.
 */

uniform sampler2D colortex0;
uniform sampler2D depthtex0;
uniform int isEyeInWater;
uniform float near;
uniform float far;
uniform float frameTimeCounter;

varying vec2 texCoord;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float causticPattern(vec2 uv, float t) {
    float a = sin(uv.x * 185.0 + sin(uv.y * 91.0 + t * 0.75) * 2.2 + t);
    float b = sin(uv.y * 171.0 - sin(uv.x * 83.0 - t * 0.62) * 2.0 - t * 0.82);
    float c = a * b;
    return pow(clamp(c * 0.5 + 0.5, 0.0, 1.0), 7.0);
}

void main() {
    vec4 scene = texture2D(colortex0, texCoord);

    // Preserve the normal image outside water, lava and powder snow.
    if (isEyeInWater != 1) {
        gl_FragColor = scene;
        return;
    }

    float depthSample = texture2D(depthtex0, texCoord).r;
    float viewDistance = linearizeDepth(depthSample);

    // Absorption builds gradually with distance, not as a flat blue overlay.
    float absorption = 1.0 - exp(-viewDistance * 0.018);
    float fogAmount = 1.0 - exp(-viewDistance * 0.027);

    vec3 absorbedColor = scene.rgb * vec3(0.72, 0.91, 0.94);
    vec3 deepWaterColor = vec3(0.012, 0.19, 0.25);
    vec3 underwaterColor = mix(absorbedColor, deepWaterColor, clamp(fogAmount, 0.0, 0.92));

    // Restrained moving caustic shimmer. This is a screen-space light pattern,
    // not a reflection and not a physically projected caustic map.
    float caustics = causticPattern(texCoord, frameTimeCounter);
    float nearVisibility = 1.0 - clamp(absorption * 0.72, 0.0, 0.72);
    underwaterColor += vec3(0.035, 0.095, 0.085) * caustics * nearVisibility;

    // Slight depth-dependent blue/green absorption.
    underwaterColor *= vec3(1.0 - absorption * 0.12, 1.0, 1.0 + absorption * 0.025);

    gl_FragColor = vec4(underwaterColor, scene.a);
}
