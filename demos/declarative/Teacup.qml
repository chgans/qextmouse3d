import Qt 4.7
import Qt.labs.threed 1.0

Item3d {
    id: top
    property real spoonY
    property bool bounce: false

    Item3d {
        mesh: teacup_mesh
        effect: Effect {
            material: china
        }

        onHoverEnter: { effect.material = china_highlight }
        onHoverLeave: { effect.material = china }
        onClicked: { top.bounce = true }

        y: top.spoonY
    }

    SequentialAnimation on spoonY {
        running: top.bounce
        NumberAnimation { to : 1; duration: 300; easing.type:"OutQuad" }
        NumberAnimation { to : 0; duration: 300; easing.type:"OutBounce" }
        onCompleted: top.bounce = false
    }
}
