attribute highp vec4 vertex;
attribute highp vec4 texCoords;

uniform highp mat4 matrix;

varying highp vec4 qTexCoord0;

void main(void)
{
    qTexCoord0 = texCoords;
    gl_Position = matrix * vertex;
};
