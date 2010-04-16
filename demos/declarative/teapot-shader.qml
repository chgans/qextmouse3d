import Qt 4.6
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
            duration: 2000
        }
        axis: Qt.vector3d(1, -0.3, 0)
    }

    SequentialAnimation on y{
        running: true
        loops: Animation.Infinite
        PauseAnimation { duration: 500 }
        NumberAnimation { to : 1.0; duration: 300; easing.type:"OutQuad" }
        NumberAnimation { to : 0.0; duration: 300; easing.type:"OutBounce" }
    }

    ShaderProgram {
        id: program
        texture: "textures/qtlogo.png"
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
            uniform mediump vec4 qgl_Color;

            void main(void)
            {
                mediump vec4 col = texture2D(qgl_Texture0, texCoord.st);
                gl_FragColor = vec4(clamp(qgl_Color.rgb * (1.0 - col.a) +
                                          col.rgb, 0.0, 1.0), 1.0);
            }
        "
    }
}

}
