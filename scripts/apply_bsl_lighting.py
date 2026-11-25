from pathlib import Path
import re

# EIVORIS lighting values are authored in src/eivoris/config.h and copied
# into NEWB before compilation. This script intentionally validates only:
# it must never overwrite the user's EIVORIS profile with a second profile.
path = Path("upstream/src/newb/config.h")
text = path.read_text()

required = (
    "NL_TONEMAP_TYPE",
    "NL_GAMMA",
    "NL_SUNLIGHT_INTENSITY",
    "NL_TORCHLIGHT_INTENSITY",
    "NL_SHADOW_INTENSITY",
    "NL_MIN_LIGHTING_BOOST",
    "NL_OVERWORLD_TORCH_COL",
    "NL_NIGHT_MOONLIGHT_COL",
    "NL_WATER_WAVE_SPEED",
    "NL_WATER_TINT",
)
for key in required:
    matches = re.findall(rf"^\s*#define\s+{key}\s+[^\n]+$", text, flags=re.M)
    if len(matches) != 1:
        raise SystemExit(f"Expected exactly one EIVORIS definition for {key}; found {len(matches)}")

if not re.search(r"^\s*#define\s+NL_TINT\s*$", text, flags=re.M):
    raise SystemExit("EIVORIS color grading macro NL_TINT is missing")

lighting_path = Path("upstream/src/newb/functions/lighting.h")
if not lighting_path.exists():
    raise SystemExit("EIVORIS lighting source was not copied into NEWB")
lighting = lighting_path.read_text()
for function in ("nlLighting(", "nlEntityLighting(", "nlUnderwaterLighting("):
    if function not in lighting:
        raise SystemExit(f"Required EIVORIS lighting function missing: {function}")

water_path = Path("upstream/src/newb/functions/water.h")
water = water_path.read_text()
for function in ("eivorisWaterField(", "nlWater("):
    if function not in water:
        raise SystemExit(f"Required EIVORIS water function missing: {function}")

print("Validated EIVORIS config, water engine and lighting source.")
