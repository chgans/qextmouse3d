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

        property real textureOffsetX : 0.0
        property real squashFactor : 0.0
        NumberAnimation on textureOffsetX
        {
            running: true; loops: Animation.Infinite
            from: 0.0; to: 1.0;
            duration: 1000
        }

        SequentialAnimation on squashFactor
        {
            running: true; loops: Animation.Infinite
            NumberAnimation {
                from: 0.0; to: 1.0; duration: 1000
                easing.type: "InQuad"
            }
            PauseAnimation {
                duration: 250
            }
            NumberAnimation {
                from: 1.0; to: 0.0; duration: 1000
                easing.type: "OutQuad"
            }
        }

        vertexShader: "
        attribute highp vec4 qgl_Vertex;
        uniform mediump mat4 qgl_ModelViewProjectionMatrix;

        attribute highp vec4 qgl_TexCoord0;
        varying highp vec4 texCoord;
        uniform mediump float textureOffsetX;
        uniform mediump float squashFactor;

        void main(void)
        {
            const float modelSize = 2.0;
            const float modelBottom = -1.0;
            float newY = max(qgl_Vertex.y - squashFactor * modelSize,
                               qgl_Vertex.y * 0.01 + modelBottom);
            gl_Position = qgl_ModelViewProjectionMatrix *
                                  vec4(qgl_Vertex.x, newY, qgl_Vertex.zw);

            texCoord = vec4(-qgl_TexCoord0.s
                            - textureOffsetX,
                            -qgl_TexCoord0.t,
                            qgl_TexCoord0.pq);
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
