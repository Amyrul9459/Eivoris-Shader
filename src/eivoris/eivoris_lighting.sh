#ifndef EIVORIS_LIGHTING_SH
#define EIVORIS_LIGHTING_SH

// EIVORIS physically-inspired direct surface response.
// Minecraft's lightmap remains authoritative for block/sky illumination;
// this material layer adds a restrained Cook-Torrance/GGX specular lobe.

float eivorisDistributionGGX(float ndh, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float d = ndh * ndh * (a2 - 1.0) + 1.0;
    return a2 / max(3.14159265 * d * d, 0.0001);
}

float eivorisGeometrySchlick(float ndv, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) * 0.125;
    return ndv / max(ndv * (1.0 - k) + k, 0.0001);
}

vec3 eivorisFresnelSchlick(float cosTheta, vec3 f0) {
    float f = pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
    return f0 + (1.0 - f0) * f;
}

vec3 eivorisWorldLighting(
    vec3 albedo,
    vec3 normal,
    vec3 viewDirection,
    vec3 lightmap,
    vec3 sunDirection,
    vec3 sunColor,
    float dayFactor,
    float materialAO
) {
    vec3 n = normalize(normal);
    vec3 v = normalize(viewDirection);
    vec3 l = normalize(sunDirection);
    vec3 h = normalize(v + l);

    float ndl = max(dot(n, l), 0.0);
    float ndv = max(dot(n, v), 0.001);
    float ndh = max(dot(n, h), 0.0);
    float vdh = max(dot(v, h), 0.0);

    float sky = clamp(lightmap.g, 0.0, 1.0);
    float block = clamp(lightmap.r, 0.0, 1.0);

    vec3 ambientSky = mix(vec3(0.022, 0.030, 0.045), vec3(0.27, 0.36, 0.48), sky);
    vec3 ambientBlock = vec3(1.0, 0.48, 0.22) * block * 0.62;
    vec3 diffuse = albedo * (ambientSky + ambientBlock);

    float roughness = 0.78;
    vec3 f0 = vec3(0.035);
    vec3 f = eivorisFresnelSchlick(vdh, f0);
    float d = eivorisDistributionGGX(ndh, roughness);
    float g = eivorisGeometrySchlick(ndv, roughness) * eivorisGeometrySchlick(max(ndl, 0.001), roughness);
    vec3 specular = (d * g * f) / max(4.0 * ndv * max(ndl, 0.001), 0.001);

    vec3 directDiffuse = albedo * sunColor * (ndl * dayFactor);
    vec3 directSpecular = specular * sunColor * (ndl * dayFactor) * 0.65;

    float ao = mix(0.52, 1.0, clamp(materialAO, 0.0, 1.0));
    vec3 result = (diffuse + directDiffuse) * ao + directSpecular;
    return max(result, vec3(0.018));
}

#endif
