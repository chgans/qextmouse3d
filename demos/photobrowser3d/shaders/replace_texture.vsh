attribute highp vec4 vertex;
attribute highp vec4 texcoord;
attribute highp vec4 thumbcoord;

uniform highp mat4 matrix;
uniform bool thumb;

varying highp vec4 qTexCoord;

void main(void)
{
    if (thumb)
        qTexCoord = thumbcoord;
    else
        qTexCoord = texcoord;
    gl_Position = matrix * vertex;
};
