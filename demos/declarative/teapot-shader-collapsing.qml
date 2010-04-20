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
            duration: 8000
        }
        axis: Qt.vector3d(1, -0.3, 0)
    }

    ShaderProgram {
        id: program
        SequentialAnimation on customValue {
            running: true
            loops: Animation.Infinite
            PauseAnimation { duration: 700 }
            NumberAnimation { from: 0.0; to: 1.0; duration: 1500; easing.type:"OutBounce" }
            PauseAnimation { duration: 700 }
            NumberAnimation { from: 1.0; to: 0.0; duration: 1500; easing.type:"OutBounce" }
        }

        texture: "textures/qtlogo.png"

        SequentialAnimation on color{
            running: true
            loops: Animation.Infinite
            ColorAnimation {
                from: "#00008a"
                to: "#003333"
                duration: 5000
            }
            ColorAnimation {
                from: "#003333"
                to: "#00008a"
                duration: 5000
            }
        }

        vertexShader: "
            attribute highp vec4 qgl_Vertex;
            attribute highp vec4 qgl_TexCoord0;
            uniform mediump mat4 qgl_ModelViewProjectionMatrix;
            varying highp vec4 texCoord;
            uniform highp float qgl_Custom;

            void main(void)
            {
                // Interpolate between the actual position of the input vertex
                // and treating the tex-coordinates as vertex positions to
                // create a neat collapsing effect.
                vec4 workingPosition = mix( qgl_Vertex, vec4(-qgl_TexCoord0.xyz, 1.0) , qgl_Custom);
                gl_Position = qgl_ModelViewProjectionMatrix * workingPosition
                ;
                texCoord =  -qgl_TexCoord0;

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
