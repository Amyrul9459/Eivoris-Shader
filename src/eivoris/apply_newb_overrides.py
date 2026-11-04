from pathlib import Path
import re

root = Path("toolchain/src/newb")
config = root / "config.h"
lighting = root / "functions" / "lighting.h"
water = root / "functions" / "water.h"

# EIVORIS keeps Newb's environment detection, sky, fog, material and lightmap
# integration, while overriding only the water implementation and selected
# lighting constants.
text = config.read_text()
replacements = {
    r"#define NL_SUNLIGHT_INTENSITY\s+3\.3": "#define NL_SUNLIGHT_INTENSITY   3.2",
    r"#define NL_SHADOW_INTENSITY\s+0\.7": "#define NL_SHADOW_INTENSITY     0.62",
    r"#define NL_MIN_LIGHTING_BOOST\s+1\.5": "#define NL_MIN_LIGHTING_BOOST   1.35",
    r"#define NL_WATER_TRANSPARENCY\s+0\.9": "#define NL_WATER_TRANSPARENCY 0.88",
    r"#define NL_WATER_BUMP\s+0\.09": "#define NL_WATER_BUMP 0.16",
    r"#define NL_WATER_WAVE_SPEED\s+0\.8": "#define NL_WATER_WAVE_SPEED 1.15",
    r"#define NL_WATER_TEX_OPACITY\s+0\.3": "#define NL_WATER_TEX_OPACITY 0.24",
    r"#define NL_WATER_TINT\s+vec3\([^)]*\)": "#define NL_WATER_TINT vec3(0.48,0.82,0.96)",
    r"#define NL_UNDERWATER_BRIGHTNESS\s+0\.8": "#define NL_UNDERWATER_BRIGHTNESS 0.65",
    r"#define NL_CAUSTIC_INTENSITY\s+1\.9": "#define NL_CAUSTIC_INTENSITY 2.2",
}
for pattern, replacement in replacements.items():
    text, count = re.subn(pattern, replacement, text, count=1)
    if count != 1:
        raise SystemExit(f"Config patch failed: {pattern}")
# Do not synthesize clouds or aurora in water reflections.
text = re.sub(r"^#define NL_CLOUD_AURORA_REFLECTION.*\n", "", text, flags=re.M)
config.write_text(text)

lighting_text = lighting.read_text()
old = "light += (skycol.horizon + skycol.zenith)*(uv1.y/(1.0+lum));"
new = "light += 1.16*(skycol.horizon + skycol.zenith)*(uv1.y/(1.0+lum));"
if old not in lighting_text:
    raise SystemExit("Lighting ambient insertion point not found")
lighting.write_text(lighting_text.replace(old, new, 1))

override = Path("src/eivoris/newb_overrides/water.h")
if not override.exists():
    raise SystemExit("EIVORIS water override missing")
(root / "functions" / "water.h").write_text(override.read_text())
print("Applied EIVORIS water + lighting overrides to pinned Newb RenderDragon pipeline.")
