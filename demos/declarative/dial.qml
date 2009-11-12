import Qt 4.6

Rectangle {
    id: Dial
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
        anchors.horizontalCenter: Dial.horizontalCenter
        y: Dial.height/4
        font.family: fontFamily
        font.italic: fontItalic
        font.pointSize: fontSize
        color: "white"
    }

    Repeater {
        dataSource: Dial.max/step+1
        Item {
            id: Tick
            width: Dial.width
            height: Dial.height
            rotation: (index*step-(Dial.max+1)/2)*(degrees/(Dial.max+1))+orientation
            transformOrigin: "Center"
            Rectangle {
                x: index*step%bigstep == 0 ? 0 : tickBigLengthOver
                y: parent.height/2
                width: index*step%bigstep == 0 ? tickBigLength : tickLength
                height: tickWidth
                color: index*step >= redstart ? redColor : tickColor
                Text {
                    id: Label
                    x: tickBigLength+textGutter
                    y: -height/2
                    rotation: -Tick.rotation
                    transformOrigin: "Center"
                    font: Other.font
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
