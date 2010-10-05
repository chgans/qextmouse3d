// Per-pixel lighting - fragment shader side.

struct qgl_MaterialParameters {
    mediump vec4 emission;
    mediump vec4 ambient;
    mediump vec4 diffuse;
    mediump vec4 specular;
    mediump float shininess;
};
uniform qgl_MaterialParameters qgl_Material;

struct qgl_SingleLightParameters {
    mediump vec4 position;
    mediump vec3 spotDirection;
    mediump float spotExponent;
    mediump float spotCutoff;
    mediump float spotCosCutoff;
    mediump float constantAttenuation;
    mediump float linearAttenuation;
    mediump float quadraticAttenuation;
};
uniform qgl_SingleLightParameters qgl_Light;

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
        component += pow(angle, qgl_Material.shininess) * qgl_Material.specular;
    }

    // Apply the spotlight angle and exponent.
    if (qgl_Light.spotCutoff != 180.0) {
        spot = max(dot(normalize(qVertexToLight),
                       normalize(qgl_Light.spotDirection)), 0.0);
        if (spot < qgl_Light.spotCosCutoff)
            spot = 0.0;
        else
            spot = pow(spot, qgl_Light.spotExponent);
        component *= spot;
    }

    return clamp(color + component, 0.0, 1.0);
}

void main(void)
{
    gl_FragColor = qLightPixel(qAmbient, qDiffuse);
}
