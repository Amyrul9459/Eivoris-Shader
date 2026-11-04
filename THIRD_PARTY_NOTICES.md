# Third-party notices

## EIVORIS-authored shader code

The RenderChunk vertex/fragment programs and the water and lighting functions in `src/materials/RenderChunk` and `src/eivoris` are authored for EIVORIS.

## FirmamentCraft

FirmamentCraft Shader by ChuYinxiao is used as a feature-level and visual reference only. Its public CurseForge project lists its license as **All Rights Reserved**. No FirmamentCraft source code, compiled material binaries, textures, or other assets are copied or redistributed by this repository.

Project page: https://www.curseforge.com/minecraft-bedrock/texture-packs/firmamentcraft-shader-renderdragon

## Build dependencies

The GitHub Actions workflow checks out a pinned Newb X Legacy repository revision only to access its generic build wrapper, BGFX shader include, and Lazurite/shaderc setup path. The workflow deletes the checkout's `src/materials` and `src/newb` shader-source directories before copying in EIVORIS-owned material sources. No Newb X shader source, water function, lighting function, macro set, or visual configuration is packaged.

Newb X Legacy build environment: https://github.com/devendrn/newb-x-mcbe

MTBinLoader2 is the runtime material-bin loader and is separate from the shader implementation.
