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

    Item3D {
        id: button_mesh
        name: "button_mesh"
        mesh: Mesh {
            source: "meshes/button3d.3ds"
            options: "ShowWarnings ForceSmooth"
        }
        cullFaces: "CullBackFaces"

        Item3D {
            id: top_node
            objectName: "top_node"
            name: "top_node"
            meshNode: "Top"
            effect: Effect { material: face_texture }
        }

        Material {
            id: face_texture
            objectName: "face_texture"
            ambientColor: "#000000"
            specularColor: "#000000"
            shininess: 16
            textureUrl: "textures/button/face.png"
        }

        Item3D {
            id: side_plate_node
            objectName: "side_plate_node"
            name: "side_plate_node"
            meshNode: "SidePlate"
            //effect: Effect { material: address_texture }
        }

        Material {
            id: address_texture
            objectName: "address_texture"
            ambientColor: "#000000"
            specularColor: "#000000"
            shininess: 16
            textureUrl: "textures/button/full-address.png"
        }

        Light {
            id: l1
            position: Qt.vector3d(-1, -10, 10)
        }
    }
}
