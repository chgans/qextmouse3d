// Built-in variables:
// mediump vec4 gl_FragCoord;
// bool gl_FrontFacing;
// mediump vec4 gl_FragColor;
// mediump vec4 gl_FragData[gl_MaxDrawBuffers]
// mediump vec2 gl_PointCoord; // Not defined unless drawing points.
uniform mediump vec4 scli;
uniform mediump vec4 scm;
uniform mediump vec4 color;
uniform mediump sampler2D texture;
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

mediump vec3 perPixelNormal;

void main(void)
{
    vec4 specularComponent = vec4( 0.0, 0.0, 0.0, 0.0 );
    perPixelNormal = normalize(qNormal);
    float intensity =  max(dot(perPixelNormal, qLightDirection), 0.0);

    if(intensity > 0.0)
    {
        float specularIntensity = max( dot(perPixelNormal, qHalfVector), 0.0 );
        if(specularIntensity > 0.0)
            specularComponent = scm * scli * pow(specularIntensity, shininess);
    }
    vec4 textureColor = texture2D(texture, qTexCoord0.st);
    vec4 lightingColor= qAmbient + qDiffuse * intensity ;

// The two fixed function lighting models are GL_MODULATE and GL_DECAL

// In GL_MODULATE the lighting and texture are combined, so the material
// and texture are combined.  Uncomment this line to see this effect:
//gl_FragColor = vec4(textureColor.xyz * (lightingColor.xyz + specularComponent.xyz), textureColor.a);

// In GL_DECAL the Texture is applied over the top of the lighting, like a
// sticker (or decal, hence the name).
gl_FragColor = vec4(lightingColor.xyz * (1.0 - textureColor.a) + textureColor.xyz * textureColor.a, lightingColor.a) + specularComponent;

};
