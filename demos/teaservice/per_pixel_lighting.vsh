// Per-pixel lighting - vertex shader side.

attribute highp vec4 vertexAttr;
attribute mediump vec3 normalAttr;
uniform mediump mat4 modelView;
uniform mediump mat4 matrix;
uniform mediump mat3 normalMatrix;

uniform mediump vec4 acli;      // Ambient intensity of the light
uniform mediump vec4 dcli;      // Diffuse intensity of the light
uniform mediump vec3 pli;       // Position of the light
uniform mediump vec4 acm;       // Ambient color of the material
uniform mediump vec4 dcm;       // Diffuse color of the material
uniform mediump vec4 ecm;       // Emissive color of the material
uniform mediump vec4 acs;       // Light model's ambient color of the scene
uniform bool viewerAtInfinity;  // Light model indicates viewer at infinity

varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;
varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;

void qLightVertex(vec4 vertex, vec3 normal)
{
    vec3 toEye;
    qNormal = normal;
    qAmbient = ecm + acm * acs + acm * acli;
    qDiffuse = dcm * dcli;
    qLightDirection = normalize(pli);
    if (viewerAtInfinity)
        toEye = vec3(0, 0, 1);
    else
        toEye = normalize(-vertex.xyz);
    qHalfVector = normalize(qLightDirection + toEye);
    qVertexToLight = vertex.xyz - pli;
}

void main(void)
{
    gl_Position = matrix * vertexAttr;
    vec4 vertex = modelView * vertexAttr;
    vec3 normal = normalize(normalMatrix * normalAttr);
    qLightVertex(vertex, normal);
}
