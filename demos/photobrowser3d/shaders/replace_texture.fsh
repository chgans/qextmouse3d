// This is equivilent to GL_REPLACE
varying highp vec4 qTexCoord;
uniform bool picking;
uniform mediump vec4 color;
uniform sampler2D texture;

void main(void)
{
    if (picking)
        gl_FragColor = color;
    else
        gl_FragColor = texture2D(texture, qTexCoord.st);
}
