#version 120

/*
 * EIVORIS Water — Java shaderpack
 * Multi-scale Gerstner-style height field.
 * Geometry displacement only; no reflection calculations.
 */

uniform mat4 gbufferModelView;
uniform mat4 gbufferModelViewInverse;
uniform vec3 cameraPosition;
uniform float frameTimeCounter;

varying vec2 vTexCoord;
varying vec4 vVertexColor;
varying vec3 vWorldPosition;

float waveHeight(vec2 p, float t) {
    float a = sin(dot(p, normalize(vec2( 1.00,  0.32))) * 0.42 + t * 1.05);
    float b = sin(dot(p, normalize(vec2(-0.38,  1.00))) * 0.68 - t * 0.82);
    float c = sin(dot(p, normalize(vec2( 0.72, -0.69))) * 1.12 + t * 0.63);
    float d = sin(dot(p, normalize(vec2(-0.92, -0.28))) * 1.75 - t * 1.24);

    return a * 0.105 + b * 0.052 + c * 0.021 + d * 0.009;
}

void main() {
    vTexCoord = gl_MultiTexCoord0.st;
    vVertexColor = gl_Color;

    vec4 viewPosition = gl_ModelViewMatrix * gl_Vertex;
    vec3 cameraRelativeWorld = (gbufferModelViewInverse * viewPosition).xyz;
    vec3 worldPosition = cameraRelativeWorld + cameraPosition;

    float t = frameTimeCounter;
    float displacement = waveHeight(worldPosition.xz, t);

    // Displace the water mesh along world-up, then return to view space.
    cameraRelativeWorld.y += displacement;
    vec4 displacedView = gbufferModelView * vec4(cameraRelativeWorld, 1.0);

    gl_Position = gl_ProjectionMatrix * displacedView;
    vWorldPosition = worldPosition;
}
