// Per-pixel lighting - fragment shader side.

uniform mediump vec4 scli;              // Specular intensity of the light
uniform mediump vec3 sdli;              // Direction of the light
uniform mediump float srli;             // Spotlight exponent for the light
uniform mediump float crli;             // Spotlight cutoff for the light
uniform mediump float ccrli;            // Cosine of spotlight cutoff for the light
uniform mediump vec4 scm;               // Specular color of the material
uniform mediump float srm;              // Specular exponent of the material

varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;
varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;

vec4 qLightPixel(vec4 ambient, vec4 diffuse)
{
    float angle, spot;
    vec4 color;
    vec4 component;
    vec3 normal = normalize(qNormal);

    // Start with the ambient color.
    color = ambient;

    // Determine the cosine of the angle between the normal and the
    // vector from the vertex to the light.
    angle = max(dot(normal, qLightDirection), 0.0);

    // Calculate the diffuse light components.
    component = angle * diffuse;

    // Calculate the specular light components.
    if (angle != 0.0) {
        angle = max(dot(normal, qHalfVector), 0.0);
        component += pow(angle, srm) * scm * scli;
    }

    // Apply the spotlight angle and exponent.
    if (crli != 180.0) {
        spot = max(dot(normalize(qVertexToLight), normalize(sdli)), 0.0);
        if (spot < ccrli)
            spot = 0.0;
        else
            spot = pow(spot, srli);
        component *= spot;
    }

    return clamp(color + component, 0.0, 1.0);
}

void main(void)
{
    gl_FragColor = qLightPixel(qAmbient, qDiffuse);
}
