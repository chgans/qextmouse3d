uniform sampler2D texture;
varying highp vec4 texCoord;
varying float facingProjector;

void main(void)
{
    vec2 clampedCoords = texCoord.st;
    clampedCoords =  clamp(texCoord.st, vec2(0.0), vec2(1.0));
    vec4 textureColor = texture2D(texture, clampedCoords);
    gl_FragColor =  vec4(textureColor.rgb * facingProjector, 1.0);
};
