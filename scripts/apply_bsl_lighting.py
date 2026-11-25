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
)
for key in required:
    matches = re.findall(rf"^\\s*#define\\s+{key}\\s+[^\\n]+$", text, flags=re.M)
    if len(matches) != 1:
        raise SystemExit(f"Expected exactly one EIVORIS definition for {key}; found {len(matches)}")

if not re.search(r"^\\s*#define\\s+NL_TINT\\s*$", text, flags=re.M):
    raise SystemExit("EIVORIS color grading macro NL_TINT is missing")

print("Validated EIVORIS lighting profile; no upstream values were overwritten.")
