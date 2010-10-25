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
    id: dial
    property int max: 220
    property int redstart: 101
    property int step: 2
    property int bigstep: 10
    property int textstep: 20
    property int degrees: 270
    property color tickColor: "#e0e0e0"
    property color textColor: "white"
    property color redColor: "#ff4040"
    property int tickWidth: 4
    property int tickLength: 25
    property int tickBigLength: 45
    property int tickBigLengthOver: 3
    property int orientation: 90
    property int textGutter: 10
    property string fontFamily: "andale sans"
    property bool fontItalic: true
    property int fontSize: 50

    width: 1000
    height: 1000
    color: "black"

    Text {
        text: "km/h"
        anchors.horizontalCenter: dial.horizontalCenter
        y: dial.height/4
        font.family: fontFamily
        font.italic: fontItalic
        font.pointSize: fontSize
        color: "white"
    }

    Repeater {
        model: dial.max/step+1
        Item {
            id: tick
            width: dial.width
            height: dial.height
            rotation: (index*step-(dial.max+1)/2)*(degrees/(dial.max+1))+orientation
            transformOrigin: "Center"
            Rectangle {
                x: index*step%bigstep == 0 ? 0 : tickBigLengthOver
                y: parent.height/2
                width: index*step%bigstep == 0 ? tickBigLength : tickLength
                height: tickWidth
                color: index*step >= redstart ? redColor : tickColor
                Text {
                    id: label
                    x: tickBigLength+textGutter
                    y: -height/2
                    rotation: -tick.rotation
                    transformOrigin: "Center"
                    //font: Other.font
                    font.family: fontFamily
                    font.italic: fontItalic
                    font.pointSize: fontSize
                    opacity: index*step%textstep == 0 ? 1 : 0
                    text: index*step
                    color: textColor
                }
            }
        }
    }
}
