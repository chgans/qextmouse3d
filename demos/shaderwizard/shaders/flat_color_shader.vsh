    attribute highp vec4 vertex;
    uniform mediump mat4 matrix;
    uniform float time;
    void main(void)
    {
        gl_Position = matrix * vec4(vertex);
    };
