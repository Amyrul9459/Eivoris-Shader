# EIVORIS Water — Java Shaderpack Prototype

A small Java-style GLSL shaderpack prototype intended for testing with Magic Loader.

## Current scope

- Animated UV distortion for a moving-water impression.
- Subtle cool tint while retaining vanilla water texture detail.
- Original water alpha is preserved.
- No screen-space reflections, environment reflections, cubemaps, or Fresnel reflection.

## Files

- `shaders/gbuffers_water.vsh`
- `shaders/gbuffers_water.fsh`
- `shaders.properties`

## Important limitations

This is a water-pass prototype, not a complete standalone visual overhaul. It does not yet implement true scene refraction, depth-based absorption, or a custom underwater post-process. Those require confirmed Magic Loader support for the relevant depth/color buffers and shader uniforms.

The first test should verify that Magic Loader compiles and applies the water pass without breaking other render passes. If the loader reports missing uniforms or unsupported GLSL syntax, capture its exact log before changing the shader interface.
