# Third-party notices

## EIVORIS-authored shader code

The active RenderChunk vertex/fragment programs and the water and lighting functions in src/materials/RenderChunk and src/eivoris are authored for EIVORIS.

## Build dependencies

The GitHub Actions workflow checks out a pinned Newb X Legacy repository revision only to access its generic build wrapper, BGFX shader include, and the Lazurite/shaderc setup path. The workflow deletes the checkout's src/materials and src/newb shader-source directories before copying in EIVORIS-owned material sources. No Newb X shader source, water function, lighting function, macro set, or visual configuration is packaged.

Newb X Legacy: https://github.com/devendrn/newb-x-mcbe

MTBinLoader2 is the runtime material-bin loader and is separate from the shader implementation.
