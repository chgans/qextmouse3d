import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480

Item3d {
    mesh: Mesh { source: "meshes/teapot.bez" }
    effect: program

    transform: Rotation3D {
        NumberAnimation on angle{
            running: true
            loops: Animation.Infinite
            from: 0
            to: 360
            duration: 5000
        }
        axis: Qt.vector3d(1, -0.3, 0)
    }

    ShaderProgram {
        id: program
        texture: "textures/basket.jpg"
        property variant texture2 : "textures/qtlogo.png"
        property real interpolationFactor : 1.0

        SequentialAnimation on interpolationFactor {
            running: true
            loops: Animation.Infinite
            NumberAnimation { to : 1.0; duration: 750; }


            PauseAnimation { duration: 550 }
            NumberAnimation { to : 0.0; duration: 750; }
            PauseAnimation { duration: 550 }
        }

        SequentialAnimation on color{
            running: true
            loops: Animation.Infinite
            ColorAnimation {
                from: "#aaca00"
                to: "#0033ca"
                duration: 500
            }
            ColorAnimation {
                from: "#0033ca"
                to: "#aaca00"
                duration: 500
            }
        }

        vertexShader: "
            attribute highp vec4 qgl_Vertex;
            attribute highp vec4 qgl_TexCoord0;
            uniform mediump mat4 qgl_ModelViewProjectionMatrix;
            varying highp vec4 texCoord;

            void main(void)
            {
                gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
                texCoord = qgl_TexCoord0;
            }
        "
        fragmentShader: "
            varying highp vec4 texCoord;
            uniform sampler2D qgl_Texture0;
            uniform sampler2D texture2;
            uniform mediump vec4 qgl_Color;
            uniform float interpolationFactor;

            void main(void)
            {
                mediump vec4 col1 = texture2D(qgl_Texture0, texCoord.st);
                mediump vec4 col2 = texture2D(texture2, texCoord.st);
                gl_FragColor = mix(col1, col2, interpolationFactor);
            }
        "
    }
}

}
