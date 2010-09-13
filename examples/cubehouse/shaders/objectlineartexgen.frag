uniform sampler2D texture;
varying highp vec4 qTexCoord0;
varying highp vec4 qNormal;
varying float facingProjector;

void main(void)
{
    vec2 clampedCoords = qTexCoord0.st;
    clampedCoords =  clamp(qTexCoord0.st, vec2(0.0), vec2(1.0));
    vec4 textureColor = texture2D(texture, clampedCoords);
    gl_FragColor =  vec4(textureColor.rgb * facingProjector, 1.0);
};
