// Built-in variables:
// mediump vec4 gl_FragCoord;
// bool gl_FrontFacing;
// mediump vec4 gl_FragColor;
// mediump vec4 gl_FragData[gl_MaxDrawBuffers]
// mediump vec2 gl_PointCoord; // Not defined unless drawing points.
uniform mediump vec4 color;

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
vec4 myColor;

float intensity =  dot(normalize(qNormal), qLightDirection);
intensity = max(intensity, 0.0);
if (intensity > 0.95)
        myColor = vec4(1.0,0.5,0.5,1.0);
else if (intensity > 0.5)
        myColor = vec4(0.6,0.3,0.3,1.0);
else if (intensity > 0.25)
        myColor = vec4(0.4,0.2,0.2,1.0);
else
        myColor = vec4(0.2,0.1,0.1,1.0);
gl_FragColor = myColor;
};
