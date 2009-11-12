import Qt 4.6

Item3d {
    id: "Top"
    property real saucerY
    Item3d {
        id: teaspoon1
        property real y_lift
        y: Math.max(y_lift,Top.saucerY)
        transform: Rotation3D {
            angle: 275
            axis: Qt.vector3d(1, 0, 0)
        }
        mesh: teaspoon_mesh
        effect: Effect {
            material: metal
        }
        property bool bounce: false

        onHoverEnter: { effect.material = metal_highlight }
        onHoverLeave: { effect.material = metal }
        onClicked: { teaspoon1.bounce = true }

        y_lift: SequentialAnimation {
            running: teaspoon1.bounce
            NumberAnimation { to : 1; duration: 300; easing:"easeOutQuad" }
            NumberAnimation { to : 0; duration: 300; easing:"easeOutBounce" }
            onCompleted: teaspoon1.bounce = false
        }
    }
}
