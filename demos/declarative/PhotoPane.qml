/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

Item3D {
    id: photo

    property real offset
    property real layer
    property variant image
    property bool bounce: false

    mesh: Mesh { source: "meshes/photopane.obj" }

    position: Qt.vector3d(offset, 0, -layer)
    effect: Effect { decal: true; texture: image }

    onClicked: {
        if (photo.state == "pulled_out")
            photo.state = "returned";
        else
            photo.state = "pulled_out";
    }

    onHoverEnter: { photo.state = "pop_up" }
    onHoverLeave: { photo.state = "returned" }

    states: [
        State {
            name: "pop_up"
            PropertyChanges {
                target: photo
                y: 1.0
            }
        },
        State {
            name: "pulled_out"
            PropertyChanges {
                target: photo
                x: 0
            }
            PropertyChanges {
                target: photo
                scale: 1.75
            }
            PropertyChanges {
                target: photo
                z: 2
            }
            PropertyChanges {
                target: viewport
                state: "show_photo"
            }
        },
        State {
            name: "returned"
            PropertyChanges {
                target: photo
                x: offset
            }
            PropertyChanges {
                target: photo
                scale: 1
            }
            PropertyChanges {
                target: photo
                z: -layer
            }
            PropertyChanges {
                target: viewport
                state: "show_group"
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            NumberAnimation {
                targets: photo
                properties: "x,scale,z,y"
                easing.type: "OutBounce"
                duration: 700
            }
        }
    ]
}
