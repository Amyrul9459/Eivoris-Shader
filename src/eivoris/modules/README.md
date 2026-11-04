# EIVORIS Rendering Modules

These modules are EIVORIS-authored GLSL-style source. They do not include
Newb headers, call Newb functions, or reuse Newb water/light/rain parameters.

| File | Responsibility |
|---|---|
| water.h | Multi-scale water displacement, analytic normals, Fresnel and tint |
| lighting.h | Direct sun, sky fill, block light and supplied ambient occlusion |
| rain.h | Rain wetness, roughness response and atmospheric haze |
| reflection.h | Fresnel and roughness response for supplied environment color |
| effects.h | ACES tone curve, color grade and distance fog |

## Integration status

SOURCE MODULES ONLY — not yet connected to a standalone RenderDragon material
pipeline. Function inputs are explicit so future material entry points can
bind Minecraft uniforms and varyings without hiding engine dependencies.

The current repository build workflow still fetches Newb X Legacy as its
material/compiler template. Therefore these modules alone do not make the
.mcpack build independent. The next stage is to author EIVORIS-owned material
entry points (starting with RenderChunk, Sky and Weather), then replace the
workflow with one that consumes EIVORIS source and the BGFX shader compiler
toolchain without fetching Newb.

MTBinLoader2 is a loader target, not a shader framework. It loads compiled
material binaries; it does not provide shader source, material interfaces,
or a compiler.
