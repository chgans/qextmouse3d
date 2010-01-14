// Per-pixel lighting - vertex shader side.

//highp vec4 gl_Position; // should be written to
attribute highp vec4 vertex;
attribute highp vec4 normal;
attribute highp vec4 texCoords;
uniform highp mat4 matrix;
uniform mediump mat3 gl_NormalMatrix;

uniform mediump vec4 acli;      // Ambient intensity of the light
uniform mediump vec4 dcli;      // Diffuse intensity of the light
uniform mediump vec4 scli;      // Specular intensity of the light
uniform mediump vec3 pli;       // Position of the light
uniform mediump vec4 acm;       // Ambient color of the material
uniform mediump vec4 dcm;       // Diffuse color of the material
uniform mediump vec4 ecm;       // Emissive color of the material
uniform mediump vec4 scm;       // specular color of the material
uniform mediump vec4 acs;       // Light model's ambient color of the scene
uniform bool viewerAtInfinity;  // Light model indicates viewer at infinity

uniform float time;

varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;
varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying highp vec4 qTexCoord0;
varying highp vec4 qTexCoord1;

void qLightVertex(vec4 vertex, vec3 normal)
{
    vec3 toEye;
    qAmbient = ecm + acm * acs + acm * acli;
    qDiffuse = dcm * dcli;
    qLightDirection = normalize(pli);
    if (viewerAtInfinity)
        toEye = vec3(0, 0, 1);
    else
        toEye = normalize(-vertex.xyz);
    qHalfVector = normalize(qLightDirection + toEye);
}

void main(void)
{
    qNormal = normalize(gl_NormalMatrix * vec3(normal));
    qLightVertex(vertex, qNormal);
    qTexCoord0 = texCoords;
    gl_Position = matrix * vertex;
};
