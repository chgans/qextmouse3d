// This is equivilent to GL_REPLACE
varying highp vec4 qTexCoord;

uniform sampler2D texture;

void main(void)
{
    gl_FragColor = texture2D(texture, qTexCoord.st);
}
