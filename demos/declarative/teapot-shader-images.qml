import Qt 4.7
import Qt3D 1.0

Viewport {
    width: 640; height: 480
    id: viewPort

    property bool flag : true

    MouseArea
    {
        anchors.fill: parent
        onClicked: parent.flag = !parent.flag
    }

    // This is a the Qt Logo fetched from wikipedia.
    property string imageUrl: flag ?
            "http://upload.wikimedia.org/wikipedia/en/thumb/4/4e/Qt_Development_Frameworks_logo.svg/220px-Qt_Development_Frameworks_logo.svg.png" :
            "textures/basket.jpg"

    Image {
        id: myChangingImage
        source: imageUrl
    }

    Item3D {
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: shaderEffect

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
    }

    ShaderProgram {
        id: shaderEffect
        property alias myBoolean : viewPort.flag
        property variant textureOffset : Qt.size(textureOffsetX * 5, 0.0)
        property real textureOffsetX : 0.0


        // Default texture that the shaderprogram will get until something else
        // is bound:
        texture: "textures/qtlogo.png"

        // This property is automatically hooked up to the image uniform
        // in the shader program below.
        property string image : imageUrl

        SequentialAnimation on textureOffsetX {
            running: true
            loops: Animation.Infinite
            PauseAnimation { duration: 2500 }
            NumberAnimation { from: 0.0; to: 1.0; duration: 1500; easing.type:"OutBounce" }
            PauseAnimation { duration: 2500 }
            NumberAnimation { from: 1.0; to: 0.0; duration: 1500; easing.type:"OutBounce" }
        }

        vertexShader: "
        attribute highp vec4 qgl_Vertex;
        attribute highp vec4 qgl_TexCoord0;
        uniform mediump mat4 qgl_ModelViewProjectionMatrix;
        varying highp vec4 texCoord;

        uniform mediump vec2 textureOffset;

        void main(void)
        {
            gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
            texCoord = -qgl_TexCoord0 + vec4(textureOffset.x, textureOffset.y, 0.0, 0.0);
        }
        "

        fragmentShader: "
        varying highp vec4 texCoord;
        uniform sampler2D qgl_Texture0;
        uniform bool myBoolean;

        // This image uniform is automatically associated with the image
        // property of the shader program.
        uniform sampler2D image;

        void main(void)
        {
            mediump vec4 imageColor = texture2D(image, texCoord.st);
            gl_FragColor = imageColor;
        }
        "
    }
}
