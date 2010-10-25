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
