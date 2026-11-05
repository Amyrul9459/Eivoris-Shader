# EIVORIS × Magic Loader — Experimental Lab

> Isolated experiment branch. The existing Bedrock/RenderDragon implementation on `main` is not modified.

## Objective

Test the Java shaderpack route on Minecraft Bedrock through Magic Loader, using **Bliss Shader** as the primary architectural reference and compatibility baseline. This is an experiment, not a claim that Java shaderpacks are natively supported by Bedrock.

## Evidence and constraints

- The Magic Loader compatibility list identifies its tested target as **Magic Loader 0.1.1**.
- Bliss is marked fully working in the supplied Magic Loader compatibility list. This is a baseline signal, not proof that every Bliss release works on this Bedrock version/device.
- Bliss uses a multi-pass Java shader architecture. EIVORIS will study the pipeline and reimplement its own code; no Bliss source, textures, or compiled pack will be copied.
- Bliss's repository license notice is restrictive and credits Chocapic13 and other contributors. Treat upstream material as all-rights-reserved unless permission is granted.
- Complementary Unbound is now a secondary comparison only.

## Test protocol

1. Install the exact Magic Loader build being tested and record its version/build.
2. Obtain Bliss from its official distribution. Keep the ZIP intact; do not repackage or commit it here.
3. Record the exact Bliss revision/release and Minecraft Bedrock version.
4. Test a clean world with no other graphics packs, first at default settings.
5. Record each test below with screenshots and the log/error text where available.
6. Only after the baseline loads, investigate individual passes and EIVORIS-owned replacements.

## Baseline matrix

| Test | Expected observation | Result |
|---|---|---|
| Pack discovery | Bliss appears in Magic Loader | Not tested |
| Initial compile/load | World renders without black screen | Not tested |
| Water | Water pass renders; no missing/opaque surfaces | Not tested |
| Daylight | Sun/sky lighting visible | Not tested |
| Shadows | No severe shadow artifacts | Not tested |
| Underwater | Underwater fog/colour pass renders | Not tested |
| Stability | No crash or runaway compile | Not tested |

## EIVORIS port order

1. Map Magic Loader's supported passes, uniforms, depth buffers, and texture formats.
2. Reproduce a minimal known-good Bliss baseline on-device.
3. Water surface response (wave profile, normals, Fresnel, colour).
4. Underwater attenuation and fog.
5. Daylight/sky response.
6. Shadows and ambient occlusion.
7. Rain and permanent puddle response.

The first EIVORIS port target is a **fragment-space water response prototype**. True vertex displacement, screen-space reflections, volumetrics, and Iris-only features are separate capabilities and must not be claimed until verified in Magic Loader.

## Device constraints

The Android target is a vivo Y22-class device with 4 GB physical RAM. Start with low render distance and the lowest available shader profile. Do not begin with volumetrics, high-resolution shadows, or expensive reflection passes.

## Files

- `BLISS-REFERENCE-ARCHITECTURE.md`: analysis of Bliss's render pipeline and the independent EIVORIS implementation sequence.
- `eivoris_water.glsl`: independently authored GLSL water math prototype, not a complete shaderpack and not yet wired to a Java shaderpack pass.
- `TEST-RESULTS.md`: test log template.

## Important

A successful pack load does not prove all effects work. A shader that compiles but has missing passes, broken water, or unstable frame pacing is a failed baseline. Keep the Bedrock implementation on `main` as a separate deliverable.
