import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480
    id: viewPort

    MouseArea {
        anchors.fill: parent
        onClicked: program.scalex = 2.5 - program.scalex
    }

    Item3d {
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: program


        transform: Rotation3D {
            NumberAnimation on angle{
                running: true
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 18000
            }
            axis: Qt.vector3d(1, -0.3, 0)
        }

        ShaderProgram {
            id: program
            property real qgl_Custom : 1.0
            property int customInt : 1
            property color customColor : "#3333ff"
            property real scalex : 1.0
            property real scaley : 1.0
            property real scalez : 1.0
            property real translationX : 0.0
            property real translationY : 0.0
            property real translationZ : 0.0

            property variant matrix4x4 :[
                scalex , 0.0, 0.0, translationX,
                0.0, scaley, 0.0, translationY,
                0.0, 0.0, scalez, translationZ,
                0.0, 0.0, 0.0, 1.0 ]

            SequentialAnimation on translationY {
                running: true
                loops: Animation.Infinite
                PauseAnimation { duration: 2000 }
                NumberAnimation { from: 1.0; to: 0.001; duration: 1500; easing.type:"OutBounce" }
                PauseAnimation { duration: 2000 }
                NumberAnimation { from: 0.001; to: 1.0; duration: 1500; easing.type:"OutBounce" }
            }

            SequentialAnimation on customInt {
                running: true
                loops: Animation.Infinite
                PauseAnimation { duration: 2500 }
                NumberAnimation { from: 1.0; to: 0.001; duration: 1500; easing.type:"OutBounce" }
                PauseAnimation { duration: 2500 }
                NumberAnimation { from: 0.001; to: 1.0; duration: 1500; easing.type:"OutBounce" }
            }

            texture: "textures/qtlogo.png"

            vertexShader: "
            attribute highp vec4 qgl_Vertex;
            attribute highp vec4 qgl_TexCoord0;
            uniform mediump mat4 qgl_ModelViewProjectionMatrix;
            varying highp vec4 texCoord;
            //            uniform highp float qgl_Custom;
            uniform int customInt;
            uniform vec2 vector;

            uniform mat3 matrix3x3;
            uniform mat4 matrix4x4;

            void main(void)
            {
                gl_Position = qgl_ModelViewProjectionMatrix *
                              (matrix4x4 * qgl_Vertex);
                texCoord = -qgl_TexCoord0;

            }
            "
            fragmentShader: "
            varying highp vec4 texCoord;
            uniform sampler2D qgl_Texture0;
            uniform lowp vec4 customColor;
            uniform bool customBoolean;

            void main(void)
            {
                mediump vec4 textureColor =
                        texture2D(qgl_Texture0, texCoord.st);
                gl_FragColor = clamp(vec4(
                        customColor.rgb * (1.0 - textureColor.a) +
                        textureColor.rgb, 1.0), 0.0, 1.0);
            }
            "
        }
    }

}
