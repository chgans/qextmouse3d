// Algorithm from section 2.14.1 of OpenGL 2.1 specification.

uniform mediump vec3 sdli;      // Direction of the light (must be normalized).
uniform mediump vec3 pli;       // Position of the light
uniform mediump float pliw;     // 0 for directional, 1 for positional.
uniform mediump float srli;     // Spotlight exponent for the light
uniform mediump float crli;     // Spotlight cutoff for the light
uniform mediump float ccrli;    // Cosine of spotlight cutoff for the light
uniform mediump vec4 acm;       // Ambient color of the material and light
uniform mediump vec4 dcm;       // Diffuse color of the material and light
uniform mediump vec4 scm;       // Specular color of the material and light
uniform mediump vec4 ecm;       // Emissive color and ambient scene color
uniform mediump float srm;      // Specular exponent of the material
uniform bool viewerAtInfinity;  // Light model indicates viewer at infinity

varying mediump vec4 qColor;
varying mediump vec4 qSecondaryColor;

void qLightVertex(vec4 vertex, vec3 normal)
{
    vec3 toEye, toLight, h;
    float angle, spot;
    vec4 color, scolor;
    vec4 adcomponent, scomponent;

    // Start with the material's emissive color and the ambient scene color,
    // which have been combined into the ecm parameter by the C++ code.
    color = ecm;
    scolor = vec4(0, 0, 0, 0);

    // Vector from the vertex to the eye position (i.e. the origin).
    if (viewerAtInfinity)
        toEye = vec3(0, 0, 1);
    else
        toEye = normalize(-vertex.xyz);

    // Determine the cosine of the angle between the normal and the
    // vector from the vertex to the light.
    if (pliw == 0.0)
        toLight = normalize(pli);
    else
        toLight = normalize(pli - vertex.xyz);
    angle = max(dot(normal, toLight), 0.0);

    // Calculate the ambient and diffuse light components.
    adcomponent = acm + angle * dcm;

    // Calculate the specular light components.
    if (angle != 0.0) {
        h = normalize(toLight + toEye);
        angle = max(dot(normal, h), 0.0);
        scomponent = pow(angle, srm) * scm;
    } else {
        scomponent = vec4(0, 0, 0, 0);
    }

    // Apply the spotlight angle and exponent.
    if (crli != 180.0) {
        spot = max(dot(normalize(vertex.xyz - pli), sdli), 0.0);
        if (spot < ccrli)
            spot = 0.0;
        else
            spot = pow(spot, srli);
    } else {
        spot = 1.0;
    }
    adcomponent *= spot;
    scomponent *= spot;

    // Generate the final output colors.
    color += adcomponent;
    scolor += scomponent;
    float alpha = dcm.a;
    qColor = vec4(clamp(color.rgb, 0.0, 1.0), alpha);
    qSecondaryColor = clamp(scolor, 0.0, 1.0);
}
