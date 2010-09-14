import Qt 4.7
import Qt3D 1.0

Viewport {
    width: 640; height: 480


    TutorialTeapot {
        id: teapot
        effect: program
        xRotation: 30
    }

    ShaderProgram {
        id: program
        texture: "textures/qtlogo.png"

        vertexShader: "
        attribute highp vec4 qgl_Vertex;
        uniform highp mat4 qgl_ModelViewProjectionMatrix;

        attribute highp vec4 qgl_TexCoord0;
        varying highp vec4 texCoord;

        void main(void)
        {
            texCoord = qgl_TexCoord0;
            gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
        }
        "
        fragmentShader: "
        varying highp vec4 texCoord;
        uniform sampler2D qgl_Texture0;

        void main(void)
        {
            mediump vec4 textureColor = texture2D(qgl_Texture0, texCoord.st);
            gl_FragColor = textureColor;
        }
        "
    }
}
