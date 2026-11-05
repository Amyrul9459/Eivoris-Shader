# OSBES Technical Audit for EIVORIS

Audit date: 2026-11-06

## Scope

Reviewed the public `develop` branch of
[jebbyk/OSBES-minecraft-bedrock-edition-shader](https://github.com/jebbyk/OSBES-minecraft-bedrock-edition-shader)
against the current EIVORIS `main` source and build workflow.

This is a source-level compatibility audit, not an in-game compatibility claim.

## Repository and licensing boundary

- OSBES describes itself as a Minecraft PE shader using OpenGL ES 3.0.
- The repository declares GNU GPL-3.0.
- EIVORIS currently declares MIT.
- No OSBES source files, shader functions, assets, or parameter blocks are copied into this branch.
- Reusing OSBES implementation code in a distributed EIVORIS build requires resolving GPL-3.0 compatibility and corresponding-source obligations. This branch instead records ideas and interfaces to reimplement independently.

## OSBES findings

### Water

Relevant source:
- `shaders/glsl/renderchunk.vertex`
- `shaders/glsl/includes/animations.glsl`
- `shaders/glsl/includes/texture_mapping.glsl`
- `shaders/glsl/renderchunk.fragment`

Observed techniques:
- Water is identified in the blended render pass using vertex color heuristics.
- Optional vertex animation displaces the water surface with two directional sine waves.
- Water normal detail is formed by sampling and combining two scrolling normal-map reads.
- Fresnel and specular/reflection composition happen in the fragment stage.

Portability assessment:
- The multi-directional animation and layered-normal concepts are useful.
- The exact water classification heuristic, texture-atlas coordinates, sampler assumptions, varyings, and uniforms are tied to OSBES's shader/material interface.
- They cannot be pasted into EIVORIS's Newb-derived RenderDragon material entry points as-is.
- EIVORIS already has its own multi-band analytic wave field and Fresnel path; the OSBES water implementation is not demonstrably superior to the current EIVORIS field.

### Underwater

Relevant source:
- `shaders/glsl/includes/env_detectors.glsl`
- `shaders/glsl/includes/animations.glsl`
- `shaders/glsl/includes/texture_mapping.glsl`
- `shaders/glsl/renderchunk.vertex`
- `shaders/glsl/renderchunk.fragment`

Observed techniques:
- Underwater state is inferred from `FOG_CONTROL.r`.
- Screen-space vertex-position distortion is animated when underwater.
- Caustics use two oppositely moving coordinates sampled from a noise region of the base texture.
- Underwater pixels change the fake-shadow/ambient-occlusion path and receive the caustics contribution.
- Vanilla fog remains part of the final composition.

Portability assessment:
- The useful ideas are animated distortion, layered moving caustics, depth-dependent scattering, and preserving a distance/fog transition.
- The detector threshold and texture-atlas sampling are engine/material-specific. They must not be assumed valid for the EIVORIS target.
- EIVORIS's current build workflow already patches the upstream underwater-lighting function. That patch is the first integration point to audit and improve, rather than adding an unrelated OSBES shader pass.

### Weather and fog

Relevant source:
- `shaders/glsl/includes/env_detectors.glsl`
- `shaders/glsl/includes/fogs.glsl`
- `shaders/glsl/renderchunk.vertex`
- `shaders/glsl/renderchunk.fragment`

Observed techniques:
- Rain is inferred from a fog-control uniform.
- Distance fog and height-based fog are computed separately.
- Underwater fog is handled through a separate environment condition.

Portability assessment:
- Separating distance fog, height fog, rain haze, and underwater fog is a useful design pattern.
- OSBES's fog-control thresholds and uniform meanings are not portable without validating the exact RenderDragon material interface.

## EIVORIS integration findings

The current EIVORIS GitHub Actions workflow:
1. Clones `devendrn/newb-x-mcbe` at pinned commit `0d4c1536b21ee4a5152dc935fc84aa0aedaec8e6`.
2. Copies EIVORIS config, water, and rain headers into that source tree.
3. Replaces the upstream underwater-lighting function using a Python source transformation.
4. Builds Android material binaries and packages the resulting mcpack.

Therefore:
- The current compiled material pipeline is still Newb-derived; the standalone files under `src/eivoris/modules/` are not yet wired into the material build.
- OSBES's legacy GLES shader files are not a drop-in replacement for the current BGFX/RenderDragon material build.
- MTBinLoader2 loads the compiled material binaries; it does not make an incompatible shader source compatible.

## Immediate correction included in this branch

The build workflow copies `src/eivoris/functions/rain.h`, not `src/eivoris/modules/rain.h`.

The copied function header previously contained:
- a permanent world-space puddle mask,
- reflective puddle composition,
- permanent ground darkening,
- a permanent damp tint.

That contradicted the project's explicit no-wet-surface requirement. This branch removes the puddle mask and all ground wetting/reflection/color modification from `nlRefl()`. The rain-impact ring field remains only as a future input; it is not claimed to be visibly rendered yet.

## Recommended independent port sequence

1. **Underwater first:** validate the actual EIVORIS underwater flag and camera-space inputs in the pinned RenderDragon source; then tune fog/scattering and caustics using existing available uniforms and texture bindings.
2. **Water second:** retain EIVORIS's analytic multi-band wave field; compare its close-range normal detail against OSBES's two-layer scrolling-normal concept. Do not import OSBES texture-atlas coordinates or vertex heuristics.
3. **Fog third:** keep rain haze and underwater fog as separate controls, preserving vanilla clouds.
4. **Rain splashes last:** determine whether the current material pass exposes stable world position and a suitable surface classification. The current ring function alone does not render splashes.
5. **Build validation:** compile all expected material-bin files, inspect package contents, then test on Minecraft Bedrock 1.26.60.4 Android with MTBinLoader2. Device testing is still required.

## Explicit exclusions

- No PBR texture assets or PBR map sampling.
- No puddles, wet-ground coating, or rain-dependent ground gloss.
- No custom clouds.
- No bloom or god rays.
- No claim of OSBES compatibility with Bedrock 1.26.60.4 / MTBinLoader2 until verified in-game.

## Validation status

- Source files inspected: yes.
- EIVORIS source/build workflow inspected: yes.
- OSBES code copied: no.
- Android material build run: no.
- MTBinLoader2 in-game test: no.
