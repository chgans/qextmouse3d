// Built-in variables:
// mediump vec4 gl_FragCoord;
// bool gl_FrontFacing;
// mediump vec4 gl_FragColor;
// mediump vec4 gl_FragData[gl_MaxDrawBuffers]
// mediump vec2 gl_PointCoord; // Not defined unless drawing points.
uniform mediump vec4 scli;
uniform mediump vec4 scm;
uniform mediump vec4 color;
uniform float shininess;
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
    vec4 specularComponent = vec4( 0.0, 0.0, 0.0, 0.0 );
    qNormal = normalize(qNormal);
    float intensity =  max(dot(qNormal, qLightDirection), 0.0);

    if(intensity > 0.0)
    {
        float specularIntensity = max( dot(qNormal, qHalfVector), 0.0 );
        if(specularIntensity > 0.0)
            specularComponent = scm * scli * pow(specularIntensity, shininess);
    }
    gl_FragColor = qAmbient + qDiffuse * intensity + specularComponent;
};
