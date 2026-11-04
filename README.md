# EIVORIS Shader

**Independent RenderDragon material shader for Minecraft Bedrock 1.26.60.4**

EIVORIS is an independently authored shader project. Its current visual direction takes feature-level inspiration from FirmamentCraft's RenderDragon work: expressive water, sky response, surface highlights, and atmospheric material rendering. No FirmamentCraft source code, compiled materials, textures, or assets are copied into this repository.

## Current rendering systems

- **Water surface:** multi-directional animated wave field, low-amplitude vertex displacement, matching analytic normals, Fresnel response, and procedural sky/sun highlights.
- **World lighting:** engine lightmap integration, hemispherical ambient response, warm/cool daylight transition, and a Cook-Torrance-style GGX specular term.
- **Atmosphere:** engine fog is preserved. Synthetic cloud reflections are intentionally excluded.

## Architecture

EIVORIS-owned shader implementation:

- `src/materials/RenderChunk/vertex.sc`
- `src/materials/RenderChunk/fragment.sc`
- `src/eivoris/eivoris_water.sh`
- `src/eivoris/eivoris_lighting.sh`

The build workflow uses a pinned external checkout only as a generic Lazurite/shaderc + BGFX build environment and for vanilla material metadata. Its shader source directories are removed before EIVORIS sources are compiled. This is build infrastructure, not the EIVORIS rendering approach.

## Important technical limits

- RenderChunk can displace only vertices already present in Minecraft's water mesh. It cannot create additional mesh subdivisions; large, smooth wave silhouettes are therefore limited by the engine mesh density.
- Sky/sun response is procedural. This is not screen-space reflection or ray-traced reflection of the surrounding terrain.
- The current lighting pass does not own a shadow-map renderer or a full global-illumination solution.
- Underwater full-screen scattering and depth-based absorption are not yet implemented; those require reliable scene-depth/pipeline inputs beyond the current RenderChunk pass.
- A successful shaderc build confirms material compilation and packaging, not the final visual result. Device testing with Minecraft 1.26.60.4 and MTBinLoader2 is required.

## Build

GitHub Actions targets Android and packages the output as `eivoris-shader.mcpack`.

[Open build workflow](https://github.com/Amyrul9459/Eivoris-Shader/actions)

## Reference and licensing

FirmamentCraft is a visual/feature reference only. Its CurseForge project lists the license as **All Rights Reserved**; EIVORIS does not redistribute or adapt its proprietary code or assets.

[ FirmamentCraft Shader — CurseForge ](https://www.curseforge.com/minecraft-bedrock/texture-packs/firmamentcraft-shader-renderdragon)

EIVORIS development remains focused on water and world lighting until these systems are validated in-game.
