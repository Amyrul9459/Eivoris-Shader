#ifndef EIVORIS_LIGHTING_SH
#define EIVORIS_LIGHTING_SH

// Independent surface-lighting model. The engine lightmap remains the source
// of block/sky illumination; EIVORIS shapes its ambient and direct response.
vec3 eivorisWorldLighting(
    vec3 albedo,
    vec3 normal,
    vec3 lightmap,
    vec3 sunDirection,
    vec3 sunColor,
    float dayFactor,
    float materialAO
) {
    vec3 n = normalize(normal);
    vec3 l = normalize(sunDirection);

    float ndl = max(dot(n, l), 0.0);
    float sky = clamp(lightmap.g, 0.0, 1.0);
    float block = clamp(lightmap.r, 0.0, 1.0);

    vec3 ambientSky = mix(vec3(0.035, 0.050, 0.070), vec3(0.30, 0.39, 0.50), sky);
    vec3 ambientBlock = vec3(1.0, 0.42, 0.16) * block * 0.72;
    vec3 direct = sunColor * ndl * dayFactor * (0.28 + 0.72 * sky);

    // Keep vertex AO / baked shading influential; never lift every shadow flat.
    float ao = mix(0.58, 1.0, clamp(materialAO, 0.0, 1.0));
    vec3 illumination = (ambientSky + ambientBlock + direct) * ao;
    illumination = max(illumination, vec3(0.035));
    return albedo * illumination;
}

#endif
