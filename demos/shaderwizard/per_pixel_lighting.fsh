// Built-in variables:
// mediump vec4 gl_FragCoord;
// bool gl_FrontFacing;
// mediump vec4 gl_FragColor;
// mediump vec4 gl_FragData[gl_MaxDrawBuffers]
// mediump vec2 gl_PointCoord; // Not defined unless drawing points.
uniform mediump vec4 color;
varying float intensity;

varying mediump vec4 qAmbient;
varying mediump vec4 qDiffuse;
varying mediump vec3 qNormal;
varying mediump vec3 qLightDirection;
varying mediump vec3 qHalfVector;
varying mediump vec3 qVertexToLight;
varying highp vec4 qTexCoord0;
varying highp vec4 qTexCoord1;


void main(void)
{
    gl_FragColor = vec4(color[0] * intensity, color[1] * intensity, color[2] * intensity, color[3]);
};
