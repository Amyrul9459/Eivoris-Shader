<div align="center">

# ✦ E I V O R I S ✦
### R E N D E R D R A G O N   S H A D E R

<p>
  <img src="https://img.shields.io/badge/MINECRAFT-BEDROCK-35B56A?style=for-the-badge&logo=minecraft&logoColor=white" alt="Minecraft Bedrock">
  <img src="https://img.shields.io/badge/RENDER-DRAGON-7C5CFC?style=for-the-badge" alt="RenderDragon">
  <img src="https://img.shields.io/badge/STATUS-IN%20DEVELOPMENT-F0A43A?style=for-the-badge" alt="In development">
</p>

**A new perspective on your world.**

*Atmosphere · Light · Depth · Immersion*

<br>

[Overview](#-overview) · [Visual Direction](#-visual-direction) · [Roadmap](#-roadmap) · [Compatibility](#-compatibility)

</div>

---

## 🌌 Overview

**EIVORIS Shader** is an independent RenderDragon material-shader project for Minecraft Bedrock Edition. Its visual direction is inspired by the natural cinematic lighting, atmospheric depth, reflective surfaces, and rich material response associated with SEUS and Photon — adapted for Bedrock's RenderDragon pipeline, not copied from those projects.

The goal is not simply to make the colors brighter. EIVORIS aims to reshape how the world feels — from the way sunlight reaches the landscape to the movement of water and the depth of distant horizons.

> **One world. A different perspective.**

## ✨ Visual Direction

| System | Direction |
|:--|:--|
| ☀️ **Lighting** | Natural, cinematic world illumination |
| 🌑 **Shadows** | Dynamic-looking shadow depth and contrast |
| 🧱 **Ambient Occlusion** | Stronger contact and environmental depth |
| 🌊 **Water** | Noticeable natural waves and subtle light response |
| 🌤️ **Sky & Atmosphere** | Richer skies, atmospheric depth, natural color |
| 🌿 **Materials** | More convincing surface response and visual detail |

<details>
<summary><strong>🎨 The EIVORIS look</strong></summary>

- Balanced realism — avoid oversaturated colors and artificial contrast.
- A living atmosphere — the sky, sunlight, and environment should feel connected.
- Water with character — visible surface movement and restrained reflections.
- Cinematic depth — shadows and ambient lighting should add dimension, not obscure the world.
- Performance awareness — visual quality must be evaluated against real device limits.

</details>

## 🧭 Roadmap

> The items below describe the intended direction. They are not claims that these effects are already implemented.

- [x] Repository foundation and EIVORIS shader configuration
- [x] RenderDragon build pipeline and material research
- [x] Initial cinematic lighting and color configuration
- [ ] Shadow and ambient-occlusion work
- [x] Initial animated water profile; in-game tuning still required
- [ ] Sky and atmospheric rendering
- [ ] Device compatibility and performance testing
- [x] Android material-bin resource-pack pipeline for MTBinLoader2
- [ ] In-game validation and visual tuning toward the SEUS / Photon direction

## 📱 Compatibility

| Component | Target |
|:--|:--|
| Edition | Minecraft Bedrock |
| Rendering | RenderDragon |
| Target version | **1.26.60.4** |
| Development status | In development |

Target loader: **MaterialBinLoader2 (MTBinLoader2)**. The Android build packages compiled RenderDragon material binaries under `renderer/materials/` inside an `.mcpack` container. The container is imported and enabled as a global resource pack; MTBinLoader2 hooks the corresponding `renderer/` assets from the active pack. This is not a standalone `.bin` file installer.

MTBinLoader2 documents Android ARM64 and ARM32 support and includes a Minecraft 26.60 signature. Compatibility is still not guaranteed until tested in-game on the exact Minecraft build and device.

## 🧪 Development Principles

1. **Real implementation over visual claims** — every feature must be validated in-game.
2. **Clean project scope** — this repository is dedicated to the EIVORIS shader, not a general texture-pack project.
3. **Reference, don't copy** — external projects may inform research, but EIVORIS development remains independent.
4. **Test before release** — packaging, loading, visual output, and performance all matter.

## 📦 Releases

The automated Android build is published as a GitHub Actions artifact named **EIVORIS-MTBinLoader2-Android**. It is an experimental test build, not a verified public release. Download the `.mcpack`, import it into Minecraft, enable it in Global Resources, and launch Minecraft through MTBinLoader2. Do not install it as a normal pack without the loader.

## ⚖️ Credits & References

EIVORIS is an independent project. Third-party projects and technical resources may be referenced for research and compatibility study. Their assets and code are not assumed to be part of this repository.

---

<div align="center">

### ✦ E I V O R I S ✦

**Crafted for atmosphere. Built for immersion.**

<sub>Independent Minecraft Bedrock shader project · 2026</sub>

</div>
