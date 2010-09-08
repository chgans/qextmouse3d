varying highp vec4 qTexCoord0;
uniform sampler2D texture;
void main(void)
{
    vec2 clampedCoords = qTexCoord0.st;
    clampedCoords =  clamp(qTexCoord0.st, vec2(0.0), vec2(1.0));
    gl_FragColor = texture2D(texture, clampedCoords);
//    gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
};
