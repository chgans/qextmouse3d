import Qt 4.6

Item3d {
    id: "Top"
    property real spoonY
    property bool bounce: false

    Item3d {
        mesh: teacup_mesh
        effect: Effect {
            material: china
        }

        onHoverEnter: { effect.material = china_highlight }
        onHoverLeave: { effect.material = china }
        onClicked: { Top.bounce = true }

        y: Top.spoonY
    }

    spoonY: SequentialAnimation {
        running: Top.bounce
        NumberAnimation { to : 1; duration: 300; easing:"easeOutQuad" }
        NumberAnimation { to : 0; duration: 300; easing:"easeOutBounce" }
        onCompleted: Top.bounce = false
    }
}
