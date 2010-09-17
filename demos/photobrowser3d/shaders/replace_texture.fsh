// This is equivilent to GL_REPLACE
varying highp vec4 qTexCoord0;

uniform mediump sampler2D texture;

void main(void)
{
    gl_FragColor = texture2D(texture, qTexCoord.st);
};
