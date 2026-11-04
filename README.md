<div align="center">

# ✦ E I V O R I S ✦
### R E N D E R D R A G O N   S H A D E R

<p>
  <img src="https://img.shields.io/badge/MINECRAFT-BEDROCK-35B56A?style=for-the-badge&logo=minecraft&logoColor=white" alt="Minecraft Bedrock">
  <img src="https://img.shields.io/badge/RENDER-DRAGON-7C5CFC?style=for-the-badge" alt="RenderDragon">
  <img src="https://img.shields.io/badge/STATUS-REBUILDING-F0A43A?style=for-the-badge" alt="Rebuilding">
</p>

**A new perspective on your world.**

*Atmosphere · Light · Depth · Immersion*

</div>

---

## 🌌 Project Direction

EIVORIS is being rebuilt as an independently authored RenderDragon shader for Minecraft Bedrock. Its visual direction takes broad artistic inspiration from the lush water and atmosphere associated with Bliss Shader and the cinematic daylight associated with BSL Shader. Those projects are visual references only: EIVORIS does not intend to copy their source code, assets, or parameter sets.

The goal is to develop EIVORIS-owned rendering modules for water, lighting, rain, reflections, and image effects, then connect them to EIVORIS-owned RenderDragon material entry points.

> **One world. A different perspective.**

## 🧩 EIVORIS Modules

| Module | Responsibility | Status |
|:--|:--|:--|
| 🌊 Water | Multi-scale displacement, analytic normals, water tint and Fresnel | Source module drafted |
| ☀️ Lighting | Direct sun, sky fill, block light and ambient response | Source module drafted |
| 🌧️ Rain | Surface wetness, roughness and rain haze | Source module drafted |
| 🪞 Reflection | Fresnel and environment reflection response | Source module drafted |
| ✨ Effects | Tone mapping, color grading and atmospheric fog | Source module drafted |
| 🌤️ Sky | Sky dome, sun/moon and atmospheric scattering | Not started |
| 🌑 Shadows / AO | Contact shading and shadow integration | Not started |

The current modules are located in `src/eivoris/modules/`. They are EIVORIS-authored GLSL-style source, but are not yet connected to the Minecraft material pipeline.

## ⚠️ Build Status — Read This

The existing GitHub Actions workflow still fetches Newb X Legacy as its material/compiler template. This is a legacy build path and means the current .mcpack is **not yet independent**, even though the new EIVORIS module sources do not depend on Newb functions.

The next milestone is to author EIVORIS-owned material entry points (starting with RenderChunk, Sky and Weather), then replace the legacy workflow. The BGFX shader compiler may be retained as a compiler toolchain; it is not the same thing as retaining Newb's shader implementation.

MTBinLoader2 remains the intended Android loader. It loads compiled material binaries; it does not provide shader source, engine material interfaces, or a compiler.

## 🎨 Visual Direction

- **Water — Bliss-inspired:** broad, smooth, layered movement, natural color depth and restrained sky reflections.
- **Lighting — BSL-inspired:** warm direct daylight, balanced ambient fill, soft visual contrast and atmospheric depth.
- **Rain:** wet material response, subtle haze and weather-aware lighting.
- **Reflections:** physically motivated Fresnel response using environment information available to the material.
- **Effects:** controlled tone mapping, color separation and distance atmosphere.

These are design targets, not claims of completed in-game effects.

## 📱 Compatibility Target

| Component | Target |
|:--|:--|
| Edition | Minecraft Bedrock |
| Renderer | RenderDragon |
| Minecraft version | **1.26.60.4** |
| Loader | MTBinLoader2 |
| Platform | Android |

Compatibility and performance must be verified on the exact Minecraft build and device before a release is called stable.

## 🧭 Roadmap

- [x] Repository foundation and MTBinLoader2 packaging research
- [x] Initial EIVORIS module source drafts
- [ ] EIVORIS-owned RenderChunk material
- [ ] EIVORIS-owned Sky material
- [ ] EIVORIS-owned Weather material
- [ ] Standalone BGFX build workflow (no Newb source fetch)
- [ ] Compile and validate material-bin output
- [ ] In-game water, lighting and rain tuning
- [ ] Android performance and compatibility testing

## 📦 Builds

Existing GitHub Actions artifacts are legacy experimental builds. They still use the Newb X Legacy material template and must not be described as an independent EIVORIS release. A new independent .mcpack will only be published after the standalone material pipeline compiles and passes validation.

---

<div align="center">

### ✦ E I V O R I S ✦

**Crafted for atmosphere. Built for immersion.**

<sub>Independent shader implementation in development · 2026</sub>

</div>
