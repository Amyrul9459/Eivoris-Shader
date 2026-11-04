# Third-party notices

## Legacy build dependency

The current legacy GitHub Actions workflow fetches Newb X Legacy:
https://github.com/devendrn/newb-x-mcbe

That workflow uses the upstream project as a material/compiler template. Consequently, artifacts produced by that workflow are not yet independent EIVORIS builds. This dependency is being removed as EIVORIS-owned material entry points and a standalone build pipeline are developed.

## Loader

MaterialBinLoader2 (MTBinLoader2) is a separate Android loader project and is not bundled into this shader:
https://github.com/mcbegamerxx954/mtbinloader2

MTBinLoader2 loads compiled material binaries. It does not provide shader source or replace the RenderDragon material/compiler pipeline.

## Artistic references

Bliss Shader and BSL Shader are used only as high-level visual references for water, atmosphere, and lighting direction. EIVORIS does not claim affiliation with these projects and does not intend to include their proprietary code or assets.
