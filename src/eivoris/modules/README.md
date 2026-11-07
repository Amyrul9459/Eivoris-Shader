# EIVORIS Rendering Modules

EIVORIS uses NEWB X Legacy as its pinned RenderDragon material/compiler
foundation while its custom runtime functions are integrated at the material
entry points.

| Source | Runtime status |
|---|---|
| `functions/water.h` | Copied into the compiled NEWB function set and called by RenderChunk through `nlWater`; includes the EIVORIS travelling wave field, Fresnel response, and directional sun/moon highlights |
| `functions/rain.h` | Copied into the compiled function set; rain-ground reflection hook intentionally returns zero, so no wet coating or puddles are added |
| `config.h` | Replaces the upstream configuration at build time; controls lighting, torch tint, water, fog, and underwater settings |
| `tools/integrate_upstream.py` | Patches and verifies the RenderChunk underwater-lighting hook and shared entity underwater-lighting branch before compilation |
| `modules/*.h` | Standalone EIVORIS design modules/prototypes; not all are currently wired into RenderDragon material entry points |

## Runtime integration and limits

The Android build fetches a pinned NEWB X Legacy source revision, copies the
EIVORIS configuration and water/rain functions, then runs
`tools/integrate_upstream.py`. That integration script fails the build unless
the EIVORIS water implementation, sun/moon reflection code, world-space
underwater caustics, and RenderChunk call site are present in the actual
sources that are compiled.

The material compiler still uses NEWB X Legacy's 13 RenderDragon material
entry points. This is an incremental integration, not a fully independent
EIVORIS material framework. MTBinLoader2 loads the compiled material binaries;
it does not provide shader source, engine interfaces, or a compiler.

The RenderDragon lightmap exposes a shared block-light channel rather than a
reliable per-block emitter identity/color. EIVORIS can tune the general torch
spectrum and preserve its warm/cool tint, but exact per-emitter colors (for
example, different hues for lanterns, sea lanterns, and other light blocks)
require a separate material/texture identification path and are not claimed
by this build.
