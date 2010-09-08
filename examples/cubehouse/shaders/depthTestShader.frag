// This is equivilent to GL_REPLACE
varying highp vec4 qTexCoord0;

uniform mediump sampler2D texture;
uniform mediump mat4 textureMatrix;
varying highp vec4 onScreenPosition;
varying highp vec4 lightScreenPosition;
varying highp vec4 worldSpaceVertex;
varying float lightDepth;
varying float actualDepth;

void main(void)
{

    // I should be able to build a matrix to do this:
//    vec4 normalizedOnScreenPosition = (onScreenPosition + 5.0) / 10.0;
vec4 normalizedLightScreenPosition = (((lightScreenPosition
//+ lightScreenPosition.w
) / lightScreenPosition.w ) + 1.0) / 2.0;

//    float lightMapDepth = texture2D(texture, normalizedLightScreenPosition.st).z;
    float lightMapDepth = texture2D(texture, normalizedLightScreenPosition.st ).z;
//        gl_FragColor = vec4(lightMapDepth, lightMapDepth, lightMapDepth, 1.0);

//    gl_FragColor = vec4(normalizedLightScreenPosition.st,0.0,1.0);

//    float lightActualDepth = normalizedLightScreenPosition.z;
    float lightActualDepth = lightDepth;
//        gl_FragColor = vec4(lightActualDepth, lightActualDepth, lightActualDepth, 1.0);
    float lightActualDepthWithStupidMaths = (lightActualDepth + 10.0) / 20.0;
    float lightDepthWithStupidMaths = (lightDepth + 10.0) / 20.0;
//    gl_FragColor = vec4(vec3(lightActualDepthWithStupidMaths), 1.0);


//    if(lightActualDepthWithStupidMaths > lightMapDepth )
//    gl_FragColor = vec4(0.2, 0.2, 0.2, 1.0);
//    else
    gl_FragColor = vec4(1.0, 1.0, 0.5, 1.0);


//    gl_FragColor = vec4(lightDepthWithStupidMaths, lightDepthWithStupidMaths, lightDepthWithStupidMaths, 1.0);


//float stupidMathslightDepth = (lightDepth + 10.0) / 20;
//gl_FragColor = vec4(stupidMathslightDepth, stupidMathslightDepth, stupidMathslightDepth, 1.0);

//float stupidMathsActualDepth = (actualDepth + 10.0) / 20.0;
//gl_FragColor = vec4(stupidMathsActualDepth, stupidMathsActualDepth, stupidMathsActualDepth, 1.0);

};
