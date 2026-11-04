# EIVORIS Shader

**Independent RenderDragon material shader for Minecraft Bedrock 1.26.60+**

EIVORIS is being built around two active systems only:

- **Water:** dense, multi-directional micro-ripples with restrained amplitude. The water mesh is not vertically displaced, so waves do not rise through block boundaries. Close-range detail is carried by a time-varying normal field.
- **World lighting:** EIVORIS-authored direct/ambient response using Minecraft's material inputs and lightmap, with natural warm sunlight and preserved shadow contrast.

## Architecture

The active renderer is authored in this repository:

- src/materials/RenderChunk/vertex.sc
- src/materials/RenderChunk/fragment.sc
- src/eivoris/eivoris_water.sh
- src/eivoris/eivoris_lighting.sh

The build workflow compiles the EIVORIS RenderChunk material. It does **not** copy Newb X shader source, functions, macros, or configuration into the pack.

The pinned external checkout is used only for the generic Lazurite/shaderc build wrapper, BGFX shader include, and vanilla material metadata needed to compile the RenderDragon material-bin. MTBinLoader2 remains the runtime material-bin loader; it is not a shader framework.

## Current implementation limits

- Water motion is normal-based, not vertex displacement. This is intentional to keep the water surface inside the Minecraft block boundary.
- Reflection is a procedural sky/sun response, not screen-space reflection.
- Lighting uses the engine lightmap and EIVORIS direct/ambient shading. This pass does not yet implement a custom shadow-map renderer.
- A successful compile validates shader/material packaging, not the visual result on a device. In-game testing with Minecraft + MTBinLoader2 is still required.

## Build

GitHub Actions builds the Android artifact as eivoris-shader.mcpack.

[Open build workflow](https://github.com/Amyrul9459/Eivoris-Shader/actions)

## Scope

Development is intentionally limited to **water and world lighting** until both meet the visual target. Other shader effects are out of scope for now.
