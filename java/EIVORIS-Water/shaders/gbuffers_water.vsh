#version 120

/*
 * EIVORIS Water — Java shaderpack prototype
 * Water-only pass. No reflection code or reflection samplers.
 */

varying vec2 vTexCoord;
varying vec4 vVertexColor;
varying vec3 vViewPosition;

void main() {
    gl_Position = ftransform();

    vTexCoord = gl_MultiTexCoord0.st;
    vVertexColor = gl_Color;
    vViewPosition = (gl_ModelViewMatrix * gl_Vertex).xyz;
}
