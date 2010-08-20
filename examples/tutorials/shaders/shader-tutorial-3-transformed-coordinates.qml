import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480


    Item3d {
        z: -8.0
        transform: [
            Rotation3D {
                NumberAnimation on angle {
                    running: true; loops: Animation.Infinite
                    from: 0; to: 360; duration: 5000
                }
                axis: Qt.vector3d(0, 0, 1.0)
            }
        ]

        TutorialTeapot {id: teapot1; effect: program; y:2.0; x:0.0}
        TutorialTeapot {id: teapot2; effect: program; y:-1.0; x:-1.732}
        TutorialTeapot {id: teapot3; effect: program; y:-1.0; x:1.732}
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

        attribute mediump vec4 qgl_TexCoord0;
        varying mediump vec4 texCoord;

        void main(void)
        {
            const float modelBottom = -4.0;

            vec4 workingPosition = qgl_ModelViewProjectionMatrix * qgl_Vertex;
            float newY = max(workingPosition.y,
                               workingPosition.y * 0.15 + modelBottom);
            workingPosition.y = newY;
            gl_Position = workingPosition;

            texCoord = -qgl_TexCoord0;
        }
        "
        fragmentShader: "
        varying mediump vec4 texCoord;
        uniform sampler2D qgl_Texture0;

        void main(void)
        {
            mediump vec4 textureColor = texture2D(qgl_Texture0, texCoord.st);
            gl_FragColor = textureColor;
        }
        "
    }
}
