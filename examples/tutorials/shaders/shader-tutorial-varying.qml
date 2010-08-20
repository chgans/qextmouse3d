import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480

    Pane {
        effect: ShaderProgram {
            id: program

            vertexShader: "
            attribute highp vec4 qgl_Vertex;
            uniform mediump mat4 qgl_ModelViewProjectionMatrix;

            attribute highp vec4 qgl_TexCoord0;
            varying highp vec4 texCoord;

            void main(void)
            {
                gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
                texCoord = vec4(qgl_TexCoord0.st, 0, 0);
            }
            "
            fragmentShader: "
            varying highp vec4 texCoord;

            void main(void)
            {
                gl_FragColor = vec4(mod(texCoord.x, 1.0),
                                    mod(texCoord.y, 1.0),
                                    0.0,
                                    1.0);
            }
            "
        }
    }
}
