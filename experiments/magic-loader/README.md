# EIVORIS × Magic Loader — Experimental Lab

> Isolated experiment branch. The existing Bedrock/RenderDragon implementation on `main` is not modified.

## Objective

Test the Java shaderpack route on Minecraft Bedrock through Magic Loader, using the original **Complementary Unbound** pack as the compatibility baseline. This is an experiment, not a claim that Java shaderpacks are natively supported by Bedrock.

## Evidence and constraints

- The Magic Loader compatibility list identifies its tested target as **Magic Loader 0.1.1**.
- Complementary Unbound is marked **generally working** (yellow), with some Iris-specific features broken. It is not marked fully working.
- The compatibility list does not identify the exact Complementary release used in its test. Therefore the current upstream release must be tested separately rather than assumed compatible.
- Complementary's official installation guidance identifies the pack as a Java Edition shaderpack for Iris or OptiFine. Magic Loader is the compatibility layer under investigation.
- Complementary has a custom license. This repository will not mirror its ZIP, assets, or source. EIVORIS will use independently authored algorithms and code.

## Test protocol

1. Install the exact Magic Loader build being tested and record its version/build.
2. Obtain Complementary Unbound from its official distribution. Keep the ZIP intact; do not repackage or commit it here.
3. Record the exact Complementary version and Minecraft Bedrock version.
4. Test a clean world with no other graphics packs, first at default settings.
5. Record each test below with screenshots and the log/error text where available.
6. Only after the baseline loads, investigate individual passes and EIVORIS-owned replacements.

## Baseline matrix

| Test | Expected observation | Result |
|---|---|---|
| Pack discovery | Complementary appears in Magic Loader | Not tested |
| Initial compile/load | World renders without black screen | Not tested |
| Water | Water pass renders; no missing/opaque surfaces | Not tested |
| Daylight | Sun/sky lighting visible | Not tested |
| Shadows | No severe shadow artifacts | Not tested |
| Underwater | Underwater fog/colour pass renders | Not tested |
| Stability | No crash or runaway compile | Not tested |

## EIVORIS port order

1. Water surface response (wave profile, normals, Fresnel, colour).
2. Underwater attenuation and fog.
3. Daylight/sky response.
4. Shadows and ambient occlusion.
5. Rain and permanent puddle response.

The first EIVORIS port target is a **fragment-space water response prototype**. True vertex displacement, screen-space reflections, volumetrics, and Iris-only features are separate capabilities and must not be claimed until verified in Magic Loader.

## Device constraints

The Android target is a vivo Y22-class device with 4 GB physical RAM. Start with low render distance and the lowest available shader profile. Do not begin with volumetrics, high-resolution shadows, or expensive reflection passes.

## Files

- `eivoris_water.glsl`: independently authored GLSL water math prototype, not a complete shaderpack and not yet wired to a Java shaderpack pass.
- `TEST-RESULTS.md`: test log template.

## Important

A successful pack load does not prove all effects work. A shader that compiles but has missing passes, broken water, or unstable frame pacing is a failed baseline. Keep the Bedrock implementation on `main` as a separate deliverable.
