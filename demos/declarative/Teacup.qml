import Qt 4.6

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

    SequentialAnimation {
        running: top.bounce
        NumberAnimation { target: top; property:"spoonY"; to : 1; duration: 300; easing.type:"OutQuad" }
        NumberAnimation { target: top; property:"spoonY"; to : 0; duration: 300; easing.type:"OutBounce" }
        onCompleted: top.bounce = false
    }
}
