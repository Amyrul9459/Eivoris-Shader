# EIVORIS Water — Java Shaderpack for Magic Loader

An experimental Java Edition shaderpack module focused on water. The source follows the OptiFine/Iris-style shaderpack interface used by compatible Java shader loaders.

## Water surface
- Multi-directional, multi-frequency animated vertex displacement.
- Analytic wave normal derived from the same height field.
- Subtle crest shading and cool water tint.
- Vanilla texture alpha is preserved.
- No screen-space reflections, cubemaps, environment reflections, or Fresnel reflection.

## Underwater
- Activates only when `isEyeInWater == 1`.
- Depth-based teal/blue absorption using `depthtex0`.
- Distance fog and subtle animated caustic modulation.
- Lava and powder-snow camera states are left untouched.

## Shader files
- `shaders/gbuffers_water.vsh`
- `shaders/gbuffers_water.fsh`
- `shaders/composite.vsh`
- `shaders/composite.fsh`
- `shaders.properties`

## Compatibility notes
This is a Java shaderpack, not a Bedrock RenderDragon pack. It expects the standard OptiFine/Iris-style uniforms and pass names, including `gbufferModelView`, `gbufferModelViewInverse`, `cameraPosition`, `frameTimeCounter`, `isEyeInWater`, and `depthtex0`.

Magic Loader compatibility can vary by version and graphics backend. The pack has been authored against the documented shaderpack interface, but has not yet been compiled or runtime-tested on the user's device.

## Known limitations
- Vertex displacement is constrained by the water mesh tessellation. It cannot create new geometry; waves may look subtle on large flat surfaces if the mesh has few vertices.
- Underwater caustics are a restrained screen-space modulation, not projected light caustics on the world.
- Refraction is intentionally not implemented.
