#version 120

// EIVORIS water surface material response.
// Refraction, screen-space reflections and underwater fog are intentionally
// deferred until the loader's depth/color-buffer contract is verified.

uniform sampler2D texture;

varying vec2 vTexCoord;
varying vec4 vColor;
varying vec3 vWaterNormal;
varying vec3 vViewDirection;
varying vec3 vWorldPosition;

void main() {
    vec4 base = texture2D(texture, vTexCoord) * vColor;

    vec3 normal = normalize(vWaterNormal);
    vec3 viewDir = normalize(vViewDirection);

    float nDotV = clamp(dot(normal, viewDir), 0.0, 1.0);
    float fresnel = pow(1.0 - nDotV, 5.0);

    // Restrained deep-to-shallow water palette.
    float textureLuma = dot(base.rgb, vec3(0.299, 0.587, 0.114));
    vec3 deepWater = vec3(0.012, 0.105, 0.155);
    vec3 shallowWater = vec3(0.045, 0.285, 0.335);
    vec3 waterTint = mix(deepWater, shallowWater, clamp(textureLuma * 1.25, 0.0, 1.0));

    vec3 color = mix(base.rgb, waterTint, 0.58);

    // A small grazing-angle lift; not a fake screen-space reflection.
    vec3 skyReflectionTint = vec3(0.48, 0.68, 0.76);
    color = mix(color, skyReflectionTint, fresnel * 0.42);

    // Preserve the source material alpha so the loader's translucent pass
    // remains in control of water compositing.
    gl_FragColor = vec4(color, base.a);
}
