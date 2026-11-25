# EIVORIS Shader

EIVORIS is being rebuilt around NEWB X Legacy as its RenderDragon material and build foundation. The first development milestone is water; lighting, sky, rain and other effects will be tuned only after water is validated in-game.

## Water milestone 0.2

The water implementation targets actual vertex displacement in the RenderChunk vertex stage, not normal-map animation alone:

- Three directional wave bands with different wavelengths and speeds.
- World-space height displacement applied to water-surface vertices before projection.
- Analytic derivatives of the same height field for reflection normals.
- Fresnel reflection, NEWB sky reflection and animated fine-scale surface detail.
- Distance fade to reduce far-field vertex shimmer.

**Geometry limitation:** RenderChunk can only displace vertices Minecraft supplies. It cannot subdivide the water mesh. If the vanilla water surface has only a few vertices per block, displacement may appear as coarse planar facets rather than a continuously tessellated ocean. The Android build must be tested in-game before amplitude, wavelength or mesh limitations can be judged.

## Build

GitHub Actions builds the Android .mcpack against NEWB X Legacy and uploads eivoris-shader.mcpack as an artifact. MTBinLoader2 is the intended Android material loader. The pack is not considered tested until it has been imported and inspected in Minecraft on-device.

## Project structure

- src/eivoris/config.h — EIVORIS configuration overlay.
- src/eivoris/functions/water.h — EIVORIS water displacement and shading.
- .github/workflows/build-shader.yml — Android build and material-bin validation.

## Upstream and licensing

NEWB X Legacy source: https://github.com/devendrn/newb-x-mcbe

The upstream source is MIT-licensed. EIVORIS retains its own project identity and water implementation; upstream source and notices remain attributed.
