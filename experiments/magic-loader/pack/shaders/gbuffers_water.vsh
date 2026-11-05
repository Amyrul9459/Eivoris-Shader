#version 120

// EIVORIS water prototype.
// Independent implementation; Bliss is a pipeline/visual reference only.

attribute vec4 mc_Entity;

uniform mat4 gbufferModelView;
uniform mat4 gbufferModelViewInverse;
uniform vec3 cameraPosition;
uniform float frameTimeCounter;

varying vec2 vTexCoord;
varying vec4 vColor;
varying vec3 vWaterNormal;
varying vec3 vViewDirection;
varying vec3 vWorldPosition;

const float EIVORIS_PI = 3.14159265359;

float eivorisWaveHeight(vec2 p, float t) {
    float a = sin(dot(p, normalize(vec2( 1.00,  0.22)) * 0.34) + t * 0.72) * 0.030;
    float b = sin(dot(p, normalize(vec2(-0.31,  1.00)) * 0.52) - t * 0.91) * 0.016;
    float c = sin(dot(p, normalize(vec2( 0.72, -0.69)) * 0.91) + t * 1.17) * 0.006;
    return a + b + c;
}

vec2 eivorisWaveGradient(vec2 p, float t) {
    vec2 d1 = normalize(vec2( 1.00,  0.22)) * 0.34;
    vec2 d2 = normalize(vec2(-0.31,  1.00)) * 0.52;
    vec2 d3 = normalize(vec2( 0.72, -0.69)) * 0.91;

    float a = cos(dot(p, d1) + t * 0.72) * 0.030;
    float b = cos(dot(p, d2) - t * 0.91) * 0.016;
    float c = cos(dot(p, d3) + t * 1.17) * 0.006;

    return a * d1 + b * d2 + c * d3;
}

void main() {
    vec4 viewPosition = gl_ModelViewMatrix * gl_Vertex;
    vec3 worldPosition = (gbufferModelViewInverse * viewPosition).xyz + cameraPosition;

    // OptiFine/Iris water block ID convention used by the Bliss reference.
    if (mc_Entity.x == 8.0) {
        float timeValue = frameTimeCounter;
        worldPosition.y += eivorisWaveHeight(worldPosition.xz, timeValue);

        vec2 gradient = eivorisWaveGradient(worldPosition.xz, timeValue);
        vec3 worldNormal = normalize(vec3(-gradient.x, 1.0, -gradient.y));
        vWaterNormal = normalize(mat3(gbufferModelView) * worldNormal);

        viewPosition = gbufferModelView * vec4(worldPosition - cameraPosition, 1.0);
    } else {
        vWaterNormal = normalize(gl_NormalMatrix * gl_Normal);
    }

    gl_Position = gl_ProjectionMatrix * viewPosition;

    vTexCoord = gl_MultiTexCoord0.xy;
    vColor = gl_Color;
    vViewDirection = normalize(-viewPosition.xyz);
    vWorldPosition = worldPosition;
}
