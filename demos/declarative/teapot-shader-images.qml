/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import Qt3D 1.0

Rectangle {
    width: 640; height: 480
    id: top
    color: "black"

    property bool flag : true

    // This is a the Qt Logo fetched from wikipedia.
    property string imageUrl: flag ?
            "http://upload.wikimedia.org/wikipedia/en/thumb/4/4e/Qt_Development_Frameworks_logo.svg/220px-Qt_Development_Frameworks_logo.svg.png" :
            "textures/basket.jpg"

    Image {
        id: myChangingImage
        source: imageUrl
    }

    Viewport {
        anchors.fill: parent

        MouseArea
        {
            anchors.fill: parent
            onClicked: top.flag = !top.flag
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
            property alias myBoolean : top.flag
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
}
