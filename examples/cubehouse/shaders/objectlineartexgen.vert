attribute highp vec4 vertex;

uniform highp mat4 objectLinearTexgenMatrix;

uniform highp mat4 matrix;

varying highp vec4 qTexCoord0;


void main(void)
{
    qTexCoord0 = objectLinearTexgenMatrix * vertex;
    gl_Position = matrix * vertex;
};
