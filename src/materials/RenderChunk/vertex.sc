$input a_color0, a_position, a_texcoord0, a_texcoord1
#ifdef INSTANCING
  $input i_data0, i_data1, i_data2, i_data3
#endif
$output v_color0, v_fog, v_texcoord0, v_lightmapUV, v_worldPos, v_data

#include <bgfx_shader.sh>

uniform vec4 RenderChunkFogAlpha;
uniform vec4 FogAndDistanceControl;
uniform vec4 ViewPositionAndTime;
uniform vec4 FogColor;
uniform vec4 TimeOfDay;
uniform vec4 CameraPosition;

void main() {
    #ifdef INSTANCING
        mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
    #else
        mat4 model = u_model[0];
    #endif

    vec3 worldPos = mul(model, vec4(a_position, 1.0)).xyz;
    vec3 cameraRelative = ViewPositionAndTime.xyz - worldPos;
    float distanceToCamera = length(cameraRelative);
    float safeDistance = max(distanceToCamera, 0.0001);

    vec2 uv = 2.0 * a_texcoord0.xy;
    uv = fract(uv) + ((floor(uv) - 0.5) / 16384.0);

    vec2 packedLight = fract(a_texcoord1.y * vec2(256.0, 4096.0));
    vec2 lightUV = packedLight * packedLight;

    vec3 absoluteWorldPos = worldPos + CameraPosition.xyz;
    float waterMask = 0.0;
    #if defined(TRANSPARENT) && !defined(RENDER_AS_BILLBOARDS) && !defined(SEASONS)
        if (a_color0.b > 0.3 && a_color0.a < 0.95) waterMask = 1.0;
    #endif

    vec4 clipPos = mul(u_viewProj, vec4(worldPos, 1.0));
    float fogDistance = max(FogAndDistanceControl.z, 1.0);
    float fogFactor = clamp(distanceToCamera / fogDistance, 0.0, 1.0);
    fogFactor = smoothstep(FogAndDistanceControl.x, FogAndDistanceControl.y, fogFactor);

    v_color0 = a_color0;
    v_texcoord0 = uv;
    v_lightmapUV = lightUV;
    v_worldPos = absoluteWorldPos;
    v_data = vec4(waterMask, absoluteWorldPos.y, distanceToCamera, ViewPositionAndTime.w);
    v_fog = vec4(FogColor.rgb, fogFactor);
    gl_Position = clipPos;
}
