# Third-party notices

EIVORIS uses the Newb X Legacy RenderDragon shader source as its upstream build foundation:
https://github.com/devendrn/newb-x-mcbe

The upstream source code is licensed under the MIT License. Its source is fetched at a pinned commit by the GitHub Actions workflow and is not copied wholesale into this repository. The EIVORIS configuration and integration files in this repository are project-specific modifications.

MaterialBinLoader2 (MTBinLoader2) is a separate Android loader project and is not bundled into this shader. EIVORIS packages the compiled material binaries in an Android resource-pack container so MTBinLoader2 can intercept the active pack's `renderer/` assets. The loader's source is MIT-licensed:
https://github.com/mcbegamerxx954/mtbinloader2

Visual references: SEUS and Photon are used only as high-level artistic references. EIVORIS does not include their proprietary shader code or assets.
