from pathlib import Path
import re

path = Path("upstream/src/newb/config.h")
text = path.read_text()

# EIVORIS PHOTON lighting profile: neutral daylight, controlled contrast,
# cooler ambient shadows and warm practical light sources.
values = {
    "NL_TONEMAP_TYPE": "4",
    "NL_GAMMA": "1.12",
    "NL_SUNLIGHT_INTENSITY": "2.95",
    "NL_TORCHLIGHT_INTENSITY": "1.18",
    "NL_SHADOW_INTENSITY": "0.64",
    "NL_MIN_LIGHTING_BOOST": "1.48",
    "NL_DAWN_SUNLIGHT_COL": "vec3(1.0,0.43,0.22)",
    "NL_NOON_SUNLIGHT_COL": "vec3(1.0,0.96,0.88)",
    "NL_NIGHT_MOONLIGHT_COL": "vec3(0.24,0.34,0.58)",
    "NL_OVERWORLD_TORCH_COL": "vec3(1.0,0.48,0.20)",
    "NL_UNDERWATER_TORCH_COL": "vec3(1.0,0.48,0.20)",
    "NL_NETHER_TORCH_COL": "vec3(1.0,0.38,0.12)",
    "NL_END_TORCH_COL": "vec3(1.0,0.58,0.30)",
    "NL_NETHER_AMBIENT": "vec3(2.35,1.42,1.10)",
    "NL_END_AMBIENT": "vec3(1.65,1.20,2.05)",
}
for key, value in values.items():
    pattern = rf"^\s*#define\s+{key}\s+[^\n]+$"
    replacement = f"#define {key} {value}"
    text, count = re.subn(pattern, replacement, text, count=1, flags=re.M)
    if count != 1:
        raise SystemExit(f"Could not patch exactly one definition for {key}")

# Enable subtle cool-shadow / warm-highlight color grading. The EIVORIS
# config overlay is self-contained and does not inherit upstream tint macros.
if "#define NL_TINT_LOW" not in text:
    marker = "#define NL_GAMMA 1.18"
    if marker not in text:
        raise SystemExit("Could not locate patched gamma definition for tint insertion")
    text = text.replace(
        marker,
        marker + "\n#define NL_TINT\n#define NL_TINT_LOW vec3(0.42,0.56,0.92)\n#define NL_TINT_HIGH vec3(1.12,0.94,0.78)",
        1,
    )
else:
    text = re.sub(r"^\s*#define NL_TINT_LOW\s+[^\n]+$", "#define NL_TINT_LOW vec3(0.42,0.56,0.92)", text, count=1, flags=re.M)
    text = re.sub(r"^\s*#define NL_TINT_HIGH\s+[^\n]+$", "#define NL_TINT_HIGH vec3(1.12,0.94,0.78)", text, count=1, flags=re.M)

path.write_text(text)
print("Applied EIVORIS BSL-inspired lighting profile.")
