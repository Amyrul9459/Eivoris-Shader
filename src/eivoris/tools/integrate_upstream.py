#!/usr/bin/env python3
"""Apply and verify EIVORIS runtime hooks in the pinned NEWB X Legacy source."""
from pathlib import Path
import re

ROOT = Path("upstream")
lighting = ROOT / "src/newb/functions/lighting.h"
vertex = ROOT / "src/materials/RenderChunk/vertex.sc"
water = ROOT / "src/newb/functions/water.h"
main = ROOT / "src/newb/main.sh"

for path in (lighting, vertex, water, main):
    if not path.is_file():
        raise SystemExit(f"Required RenderDragon source missing: {path}")

water_text = water.read_text()
for marker in ("eivorisWaterWaveField", "env.sunDir", "env.moonDir"):
    if marker not in water_text:
        raise SystemExit(f"EIVORIS water implementation missing marker: {marker}")

main_text = main.read_text()
for include in ('#include "functions/lighting.h"', '#include "functions/water.h"'):
    if include not in main_text:
        raise SystemExit(f"RenderChunk does not include required runtime module: {include}")

src = lighting.read_text()
if "float nlEivorisCaustics(" not in src:
    helper = r"""
// EIVORIS caustics: world-anchored, animated multi-scale light caustics.
// disp() returns a narrow 0.8..1.0 range; remap before shaping so the
// caustic field does not collapse into an almost constant bright value.
float nlEivorisCaustics(vec3 worldPos, float t) {
  vec3 p0 = worldPos * vec3(0.31, 0.23, 0.31);
  vec3 p1 = worldPos * vec3(0.47, 0.19, 0.43) + vec3(13.7, 2.1, 7.3);
  vec3 p2 = worldPos * vec3(0.67, 0.29, 0.59) + vec3(4.3, 17.1, 9.2);

  float a = clamp((disp(p0, t*0.72) - 0.80) * 5.0, 0.0, 1.0);
  float b = clamp((disp(p1, t*0.91) - 0.80) * 5.0, 0.0, 1.0);
  float c = clamp((disp(p2, t*1.13) - 0.80) * 5.0, 0.0, 1.0);

  float network = 0.50*pow(a, 5.0) + 0.32*pow(b, 6.0) + 0.18*pow(c, 7.0);
  return smoothstep(0.12, 0.66, network);
}
"""
    anchor = "void nlUnderwaterLighting("
    if anchor not in src:
        raise SystemExit("Cannot insert caustics helper: nlUnderwaterLighting() not found")
    src = src.replace(anchor, helper + "\n" + anchor, 1)

pattern = r"void nlUnderwaterLighting\(.*?\n\}"
replacement = """void nlUnderwaterLighting(inout vec3 light, inout vec3 pos, vec2 lit, vec2 uv1, vec3 tiledCpos, vec3 cPos, vec3 worldPos, highp float t, vec3 horizonCol) {
  // This function is called only when env.underwater is true. Do not gate
  // caustics on sky-light UV: that incorrectly suppresses them in bright water.
  float caustics = nlEivorisCaustics(worldPos, t);
  float lightMask = 0.24 + 0.76*max(lit.y, lit.x*0.45);
  light += vec3_splat(NL_UNDERWATER_BRIGHTNESS*0.28
                    + NL_CAUSTIC_INTENSITY*0.48*caustics*lightMask);

  vec3 scatterColor = mix(normalize(horizonCol), vec3(0.30, 0.60, 0.74), 0.48);
  light *= mix(scatterColor, vec3_splat(0.62), lit.y*0.42);

  #ifdef NL_UNDERWATER_WAVE
    pos.xy += NL_UNDERWATER_WAVE*min(0.05*pos.z,0.6)*sin(t*1.2 + dot(cPos,vec3_splat(PI_HALF)));
  #endif
}"""
src, count = re.subn(pattern, replacement, src, count=1, flags=re.S)
if count != 1:
    raise SystemExit("Could not replace nlUnderwaterLighting()")

entity_pattern = r"  if \(env\.underwater\) \{\n    vec3 gPos = wPos \+ CAMERA_POS;\n    float caustics = .*?\n    light \*= mix\(normalize\(skycol\.horizon\), vec3_splat\(0\.5\), tileLightCol\.b\*0\.2\);\n  \}"
entity_replacement = """  if (env.underwater) {
    vec3 gPos = wPos + CAMERA_POS;
    float caustics = nlEivorisCaustics(gPos, t);
    light += vec3_splat(0.28*NL_UNDERWATER_BRIGHTNESS
                      + 0.34*NL_CAUSTIC_INTENSITY*caustics*(0.25 + tl));
    vec3 scatterColor = mix(normalize(skycol.horizon), vec3(0.30, 0.60, 0.74), 0.48);
    light *= mix(scatterColor, vec3_splat(0.56), tileLightCol.b*0.28);
  }"""
src, entity_count = re.subn(entity_pattern, entity_replacement, src, count=1, flags=re.S)
if entity_count != 1:
    raise SystemExit("Could not replace entity underwater-lighting branch")
lighting.write_text(src)

vsrc = vertex.read_text()
old_call = "nlUnderwaterLighting(light, pos.xyz, lit, uv1, tiledCpos, cPos, t, skycol.horizon);"
new_call = "nlUnderwaterLighting(light, pos.xyz, lit, uv1, tiledCpos, cPos, gPos, t, skycol.horizon);"
if old_call in vsrc:
    vsrc = vsrc.replace(old_call, new_call, 1)
elif new_call not in vsrc:
    raise SystemExit("Could not locate RenderChunk underwater-lighting call")
vertex.write_text(vsrc)

# Fail closed if the actual material entry point is not wired to the patched code.
checks = {
    "EIVORIS water wave field": "eivorisWaterWaveField" in water_text,
    "EIVORIS sun reflection": "env.sunDir" in water_text,
    "EIVORIS moon reflection": "env.moonDir" in water_text,
    "world-space caustics helper": "float nlEivorisCaustics(" in src,
    "chunk underwater caustics call": "nlEivorisCaustics(worldPos, t)" in src,
    "entity underwater caustics call": "nlEivorisCaustics(gPos, t)" in src,
    "RenderChunk passes world position": new_call in vsrc,
}
failed = [name for name, ok in checks.items() if not ok]
if failed:
    raise SystemExit("EIVORIS runtime integration audit failed: " + ", ".join(failed))

print("EIVORIS runtime integration passed:")
for name in checks:
    print(" - " + name)
