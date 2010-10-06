// Per-pixel lighting - vertex shader side.

attribute highp vec4 qgl_Vertex;
attribute mediump vec3 qgl_Normal;
uniform mediump mat4 qgl_ModelViewMatrix;
uniform mediump mat4 qgl_ModelViewProjectionMatrix;
uniform mediump mat3 qgl_NormalMatrix;

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
    qAmbient = qgl_Material.emission + qgl_Material.ambient;
    qDiffuse = qgl_Material.diffuse;
    qLightDirection = normalize(qgl_Light.position.xyz);
    toEye = vec3(0, 0, 1);
    qHalfVector = normalize(qLightDirection + toEye);
    qVertexToLight = vertex.xyz - qgl_Light.position.xyz;
}

void main(void)
{
    gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
    vec4 vertex = qgl_ModelViewMatrix * qgl_Vertex;
    vec3 normal = normalize(qgl_NormalMatrix * qgl_Normal);
    qLightVertex(vertex, normal);
}
