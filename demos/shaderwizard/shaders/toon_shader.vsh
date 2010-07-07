//highp vec4 gl_Position; // should be written to
//mediump float gl_PointSize; // may be written to
attribute highp vec4 vertex;
attribute highp vec4 normal;
uniform mediump mat4 matrix;

uniform mediump vec4 acli;      // Ambient intensity of the light
uniform mediump vec4 dcli;      // Diffuse intensity of the light
uniform mediump vec3 pli;       // Position of the light
uniform mediump vec4 acm;       // Ambient color of the material
uniform mediump vec4 dcm;       // Diffuse color of the material
uniform mediump vec4 ecm;       // Emissive color of the material
uniform mediump vec4 acs;       // Light model's ambient color of the scene
uniform bool viewerAtInfinity;  // Light model indicates viewer at infinity

uniform float time;
uniform vec3 lightDir;
uniform highp mat3 qgl_NormalMatrix;

varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;
varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;
varying highp vec4 qTexCoord0;
varying highp vec4 qTexCoord1;


void qLightVertex(vec4 vertex, vec3 normal)
{
    vec3 toEye;
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
    vec4 v = vertex;
    vec3 toEye;
    qNormal = normalize(qgl_NormalMatrix * vec3(normal));
    qLightVertex(vertex, qNormal);
    gl_Position = matrix * v;
};
