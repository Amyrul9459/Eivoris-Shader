# EIVORIS LeviPack — native bootstrap

This directory defines the separate LeviLauncher-native distribution target. It is not a RenderDragon resource pack and does not use MTBinLoader2.

## Current bootstrap state

`EIVORIS-LeviPack-0.1.0-test.levipack` was assembled from the user-supplied `levishaderLITE-1.3.0.levipack` reference. The manifest is branded EIVORIS for a loader smoke-test, but the native payload remains the original `levishaderLITE.so` binary unchanged. Therefore this test package verifies only package/loader compatibility; it does **not** contain EIVORIS-specific rendering behavior.

## Next engineering milestone

Recover or recreate the native source/build interface, then replace the inherited binary with an EIVORIS-owned ARM64 module. Keep the Levi preload contract and OpenGL ES hook lifecycle compatible. Implement water/reflection first, then lighting, underwater caustics, and rain effects.

## Target

- LeviLauncher native preload ecosystem
- `.levipack` archive
- Android ARM64 / AArch64
- Minecraft `1.26.*` (manifest target from supplied reference)
- OpenGL ES 3 runtime (observed imports in supplied native binary)
- MTBinLoader2: not used

## Attribution

The bootstrap native payload is copied unchanged from the user-provided LeviShader LITE 1.3.0 package, whose manifest credits `danielzinn/claude`. This is a compatibility baseline, not original EIVORIS code. Preserve upstream attribution and license obligations when distributing it. No license file was present inside the supplied archive, so redistribution terms still need confirmation.
