$input v_color0, v_fog, v_texcoord0, v_lightmapUV, v_worldPos, v_data

#include <bgfx_shader.sh>
#include "eivoris_water.sh"
#include "eivoris_lighting.sh"

uniform vec4 FogColor;
uniform vec4 TimeOfDay;
uniform vec4 SunDirection;
uniform vec4 CameraPosition;
uniform vec4 ViewPositionAndTime;

SAMPLER2D_AUTOREG(s_MatTexture);
SAMPLER2D_AUTOREG(s_LightMapTexture);

void main() {
    #if defined(DEPTH_ONLY_OPAQUE) || defined(DEPTH_ONLY) || defined(INSTANCING)
        gl_FragColor = vec4(1.0);
        return;
    #endif

    vec4 texel = texture2D(s_MatTexture, v_texcoord0);
    #ifdef ALPHA_TEST
        if (texel.a < 0.6) discard;
    #endif

    vec3 lightmap = texture2D(s_LightMapTexture, v_lightmapUV).rgb;
    vec3 viewDir = normalize(CameraPosition.xyz - v_worldPos);
    float dayPhase = TimeOfDay.x;
    float daylight = clamp(0.5 + 0.5 * sin((dayPhase - 0.25) * 6.2831853), 0.0, 1.0);

    if (v_data.x > 0.5) {
        float time = ViewPositionAndTime.w;
        vec3 normal = eivorisWaterNormal(v_worldPos.xz, time);
        float ndv = max(dot(normal, viewDir), 0.0);
        float fresnel = eivorisWaterFresnel(ndv);
        vec3 reflected = reflect(-viewDir, normal);
        vec3 skyReflection = eivorisWaterSky(reflected, FogColor.rgb, daylight);

        vec3 sunDir = normalize(SunDirection.xyz);
        float sunGlint = pow(max(dot(reflected, sunDir), 0.0), 96.0);
        sunGlint += 0.35 * pow(max(dot(reflected, sunDir), 0.0), 24.0);
        vec3 sunColor = mix(vec3(1.0, 0.52, 0.27), vec3(1.0, 0.91, 0.76), daylight);
        skyReflection += sunColor * sunGlint * (0.35 + 0.65 * daylight);

        vec3 deepWater = mix(vec3(0.012, 0.105, 0.135), vec3(0.018, 0.20, 0.24), clamp(lightmap.g, 0.0, 1.0));
        vec3 waterColor = mix(deepWater, skyReflection, fresnel * 0.82);
        waterColor += sunColor * (0.025 + 0.07 * sunGlint);
        float alpha = clamp(texel.a * v_color0.a + 0.12, 0.28, 0.92);
        gl_FragColor = vec4(waterColor, alpha);
        return;
    }

    vec3 geometricNormal = normalize(cross(dFdx(v_worldPos), dFdy(v_worldPos)));
    if (!gl_FrontFacing) geometricNormal = -geometricNormal;

    vec3 albedo = texel.rgb * v_color0.rgb;
    // Preserve engine vertex shading/tint directly; do not mistake albedo tint for AO.
    float materialAO = 1.0;
    vec3 sunDirection = normalize(SunDirection.xyz);
    vec3 sunColor = mix(vec3(1.0, 0.54, 0.32), vec3(1.0, 0.91, 0.78), daylight);
    vec3 litColor = eivorisWorldLighting(albedo, geometricNormal, lightmap, sunDirection, sunColor, daylight, materialAO);

    // Natural distance fog from the engine's fog color; no synthetic cloud reflection.
    litColor = mix(litColor, FogColor.rgb, clamp(v_fog.a, 0.0, 1.0));
    gl_FragColor = vec4(litColor, texel.a * v_color0.a);
}
