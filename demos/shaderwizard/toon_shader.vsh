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
varying float intensity;
uniform vec3 lightDir;

varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;
varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;
varying highp vec4 qTexCoord0;
varying highp vec4 qTexCoord1;


void qLightVertex(vec4 vertex, vec4 normal)
{
    vec3 toEye;
    qNormal = vec3(normal);
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
    vec4 v = vertex;
    vec3 toEye;
    qLightVertex(vertex, normal);
   intensity = dot(pli, vec3(normal));
    gl_Position = matrix * v;
};
