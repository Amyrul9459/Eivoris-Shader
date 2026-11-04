# EIVORIS Shader

**Independent RenderDragon shader for Minecraft Bedrock 1.26.60.4**

EIVORIS uses the proven Newb X RenderChunk pipeline as its technical base, with EIVORIS-authored water and lighting overrides. The goal is expressive, visibly moving water and natural cinematic world lighting without pretending that RenderChunk alone provides ray tracing or full screen-space global illumination.

## Current implementation

### Water
- Four travelling wave directions with different wavelengths and speeds.
- Matching procedural gradient normals for close-range wave detail.
- Vertex displacement on the existing Minecraft water mesh.
- Fresnel reflection and procedural sky/sun response.
- Blue/teal water tint with tuned transparency and specular response.
- Cloud and aurora reflections disabled.

### World lighting
- Newb environment detection for day/night, rain, Nether, End and underwater.
- Sun/moon lighting, sky ambient, torch contribution and lightmap integration.
- Tuned shadow response and increased sky ambient contribution for softer, more natural illumination.
- Existing Newb fog/sky pipeline retained instead of replacing it with a flat fog-color gradient.

### Underwater
- Retains Newb underwater lighting and caustics, with EIVORIS tuning for brightness and caustic intensity.
- Full-screen depth absorption and volumetric scattering are not claimed: those require additional pipeline inputs/passes.

## Source layout

- `src/materials/RenderChunk/` — Newb-compatible RenderChunk vertex/fragment/varying interface.
- `src/eivoris/newb_overrides/water.h` — EIVORIS multi-directional water implementation.
- `src/eivoris/apply_newb_overrides.py` — reproducibly applies EIVORIS water and lighting settings to the pinned Newb build tree.
- `src/eivoris/pack_config.toml` — EIVORIS package metadata.

The build pins the Newb X repository to a specific commit. Newb X source is MIT-licensed; its copyright/license notices are retained in the build source. EIVORIS does not use proprietary FirmamentCraft or Bliss source/assets.

## Known technical limits

- Water displacement can only move vertices already present in Minecraft's water mesh. It cannot add subdivisions, so the visible silhouette wave is constrained by mesh density. The moving normal field provides additional detail close to the player.
- Reflections are procedural sky/sun reflections, not screen-space reflections of terrain or ray tracing.
- Shadowing remains constrained by the RenderChunk/lightmap data available in this pipeline.
- A successful shader compiler build confirms material compilation and package creation, not final in-game visual quality.

## Build and test

GitHub Actions compiles the Android material package for MTBinLoader2 and names it `eivoris-shader.mcpack`.

[Build workflow](https://github.com/Amyrul9459/Eivoris-Shader/actions)

**Target:** Minecraft Bedrock 1.26.60.4 + MTBinLoader2. Device testing is still required to judge the final water motion, lighting balance and performance.
