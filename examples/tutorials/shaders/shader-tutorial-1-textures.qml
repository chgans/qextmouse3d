import Qt 4.7
import Qt3D 1.0

Viewport {
    width: 640; height: 480

    TutorialTeapot {
        effect: program
    }

    ShaderProgram {
        id: program
        texture: "textures/qtlogo.png"
        property real textureOffsetX : 1.0
        NumberAnimation on textureOffsetX
        {
            running: true; loops: Animation.Infinite
            from: 0.0; to: 1.0;
            duration: 1000
        }

        property string texture2: "textures/basket.jpg"
        property real interpolationFactor : 1.0
        SequentialAnimation on interpolationFactor
        {
            running: true; loops: Animation.Infinite
            NumberAnimation { from: 0.0; to: 1.0; duration: 2000 }
            PauseAnimation { duration: 500 }
            NumberAnimation { from: 1.0; to: 0.0; duration: 2000 }
            PauseAnimation { duration: 500 }
        }

        vertexShader: "
        attribute highp vec4 qgl_Vertex;
        uniform mediump mat4 qgl_ModelViewProjectionMatrix;

        attribute highp vec4 qgl_TexCoord0;
        uniform mediump float textureOffsetX;
        varying mediump vec4 texCoord;

        void main(void)
        {
            gl_Position = qgl_ModelViewProjectionMatrix * qgl_Vertex;
            texCoord.st = vec2(-qgl_TexCoord0.s - textureOffsetX,
                            -qgl_TexCoord0.t);
            texCoord.pq = vec2(-qgl_TexCoord0.s + textureOffsetX,
                            -qgl_TexCoord0.t);
        }
        "
        fragmentShader: "
        varying highp vec4 texCoord;
        uniform sampler2D qgl_Texture0;
        uniform sampler2D texture2;
        uniform mediump float interpolationFactor;

        void main(void)
        {
            mediump vec4 texture1Color = texture2D(qgl_Texture0, texCoord.st);
            mediump vec4 texture2Color = texture2D(texture2, texCoord.pq);
            mediump vec4 textureColor = mix(texture1Color, texture2Color, interpolationFactor);
            gl_FragColor = textureColor;
        }
        "
    }
}
