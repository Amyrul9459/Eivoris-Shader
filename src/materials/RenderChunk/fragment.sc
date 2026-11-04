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
        float time = v_data.w;
        vec3 normal = eivorisWaterNormal(v_worldPos.xz, time);
        float ndv = max(dot(normal, viewDir), 0.0);
        float fresnel = eivorisWaterFresnel(ndv);
        vec3 reflected = reflect(-viewDir, normal);
        vec3 skyReflection = eivorisWaterSky(reflected, FogColor.rgb, daylight);

        vec3 sunDir = normalize(SunDirection.xyz);
        float sunAlignment = max(dot(reflected, sunDir), 0.0);
        float sunGlint = pow(sunAlignment, 112.0) + 0.30 * pow(sunAlignment, 28.0);
        vec3 sunColor = mix(vec3(1.0, 0.48, 0.27), vec3(1.0, 0.91, 0.76), daylight);
        skyReflection += sunColor * sunGlint * (0.42 + 0.58 * daylight);

        // Shallow/deep tint is driven by available engine lightmap, not fake
        // screen-space depth. No cloud reflection is synthesized.
        float skyLight = clamp(lightmap.g, 0.0, 1.0);
        vec3 deepTint = mix(vec3(0.006, 0.045, 0.075), vec3(0.012, 0.14, 0.19), skyLight);
        vec3 waterColor = mix(deepTint, skyReflection, clamp(fresnel * 0.92, 0.0, 1.0));
        waterColor += sunColor * (0.018 + 0.055 * sunGlint);
        float alpha = clamp(texel.a * v_color0.a + 0.10, 0.30, 0.94);
        gl_FragColor = vec4(waterColor, alpha);
        return;
    }

    vec3 geometricNormal = normalize(cross(dFdx(v_worldPos), dFdy(v_worldPos)));
    if (!gl_FrontFacing) geometricNormal = -geometricNormal;

    vec3 albedo = texel.rgb * v_color0.rgb;
    vec3 sunDirection = normalize(SunDirection.xyz);
    vec3 sunColor = mix(vec3(1.0, 0.52, 0.32), vec3(1.0, 0.91, 0.78), daylight);
    vec3 litColor = eivorisWorldLighting(
        albedo, geometricNormal, viewDir, lightmap,
        sunDirection, sunColor, daylight, 1.0
    );

    // Preserve engine fog behavior and avoid artificial cloud reflections.
    litColor = mix(litColor, FogColor.rgb, clamp(v_fog.a, 0.0, 1.0));
    gl_FragColor = vec4(litColor, texel.a * v_color0.a);
}
