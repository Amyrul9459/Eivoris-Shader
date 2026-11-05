# EIVORIS × Bliss — Technical Foundation

This document records an independent implementation plan based on studying the public structure and rendering concepts of X0nk/Bliss-Shader. It is not a source-code fork and contains no copied Bliss shader implementation or assets.

## Reference examined

- Upstream: https://github.com/X0nk/Bliss-Shader
- Branch examined: `Stable`
- Repository revision inspected: `81e403ed308141039a09d792a36f8eb328898a60`
- Relevant upstream paths inspected:
  - `shaders/dimensions/all_translucent.vsh`
  - `shaders/dimensions/all_translucent.fsh`
  - `shaders/lib/waterBump.glsl`
  - `shaders/dimensions/composite3.fsh`
  - `shaders/dimensions/deferred.fsh`
  - `shaders/lib/settings.glsl`
  - `shaders/shaders.properties`

Bliss describes itself as an extensive edit of Chocapic13 V9.0–V9.1 and credits upstream contributors. Its repository license file states that Chocapic13 retains all rights. Treat Bliss and its upstream components as all-rights-reserved unless the relevant rights holder grants permission. Do not copy source, textures, or package assets into EIVORIS.

## What EIVORIS should learn from Bliss

### 1. Water is a pipeline, not a single wave formula

The inspected implementation separates several jobs:

1. **Geometry stage:** classify water and optionally displace the water surface before projection.
2. **Surface data:** carry tangent frame, normal, material mask, and view direction to the fragment stage.
3. **Wave field:** derive a broad height field from world-space coordinates and animated noise.
4. **Normal reconstruction:** sample the height field at nearby coordinates and derive a stable normal from its gradients.
5. **Material response:** use the normal for Fresnel/specular response and water shading.
6. **Composite/refraction:** distort the background sampling coordinate and use scene depth to avoid arbitrary distortion through unrelated geometry.
7. **Underwater response:** apply distance-dependent absorption/scattering as a distinct view condition.

EIVORIS must preserve these boundaries. A fragment-only normal animation is not geometric wave displacement. A Fresnel term is not a reflection system. Refraction needs a valid background/depth input from the loader.

### 2. Water surface target for EIVORIS

Use Bliss as a visual/technical reference, while keeping the actual implementation independently authored:

- World-anchored motion; waves must not swim with the camera.
- Multiple directional scales, with compact ripples over broad low-amplitude movement.
- Keep vertical displacement conservative so the surface does not visibly rise through adjacent block boundaries.
- Reconstruct normals from the same height field used by displacement; do not animate unrelated normal noise.
- Increase normal sampling footprint with camera distance to suppress distant shimmer.
- Keep near-player wave detail perceptible without making the entire surface choppy.
- Fresnel should strengthen grazing-angle reflections, not turn the whole water surface into a mirror.
- Water tint should vary with depth and view distance.
- Caustics, SSR, and refraction are optional later stages, each gated by confirmed loader inputs and performance.

### 3. Lighting architecture

Bliss demonstrates a multi-pass deferred architecture rather than trying to calculate every effect in one fragment shader. EIVORIS should follow the same architectural principle:

- **G-buffer/material stage:** preserve material identity, normals, depth, and lightmap inputs where the loader exposes them.
- **Lighting stage:** calculate direct sun/moon response separately from ambient/sky contribution.
- **Shadow stage:** consume a real shadow map only if Magic Loader exposes a supported shadow pass and matrices.
- **Composite stage:** combine lighting, fog, water/refraction, and post-processing in a deliberate order.
- **Dimension variants:** isolate Overworld, Nether, and End conditions instead of scattering dimension checks through every formula.

Do not invent uniforms or assume that Iris/OptiFine inputs exist in Magic Loader. First map the loader's actual supported uniforms, buffers, formats, and pass names.

### 4. Effects to defer

Bliss includes advanced systems such as voxel/floodfill light propagation, volumetric clouds/fog, screen-space reflections, temporal AA, and variable-penumbra shadows. These are not first milestones for EIVORIS.

For the Android target (vivo Y22, 4 GB RAM), defer:
- Voxel floodfill / LPV.
- Volumetric clouds and volumetric lighting.
- High-sample PCF / variable-penumbra shadows.
- Long-step SSR and expensive ray marching.
- High-resolution temporal accumulation.

Start with one-pass or low-sample effects. Add a costly feature only after profiling shows it is viable.

### 5. Suggested EIVORIS implementation sequence

| Stage | Deliverable | Gate |
|---|---|---|
| 0 | Magic Loader capability map and known-good pack baseline | Loader accepts and renders a shaderpack |
| 1 | Basic deferred/material pass with correct depth and normals | No missing geometry or black surfaces |
| 2 | Independent water normals and restrained wave motion | Stable close-up and distant water |
| 3 | Water tint, Fresnel and underwater absorption | No abrupt colour discontinuity |
| 4 | Screen-space refraction using verified depth/background buffers | No severe edge smearing |
| 5 | Basic shadows and sky/direct-light separation | No invented or unsupported inputs |
| 6 | Rain response, impact rings and persistent puddles | No dependence on Bedrock-only APIs |
| 7 | Optional SSR, caustics and advanced atmosphere | Only after performance profiling |

## Magic Loader compatibility notes

The compatibility list previously supplied for this project marks Bliss as fully working on its tested Magic Loader setup. This is a useful baseline signal, not proof that every Bliss release or every Bliss feature works on the user's Bedrock build/GPU. Record the exact Magic Loader build, Minecraft version, Bliss package revision, and device before treating it as reproducible.

The current EIVORIS experiment branch still needs an on-device baseline test. Repository inspection alone cannot validate runtime compatibility.

## Licensing and attribution

Bliss is a reference for studying architecture and visual goals. Do not redistribute its source, textures, or compiled pack as EIVORIS. Implement original code, retain a record of technical inspiration, and obtain permission before incorporating any upstream code. Bliss README credits Chocapic13 and other contributors; the upstream repository's license notice is restrictive.
