import Qt 4.6

Item3d {
    id: "top"
    property real saucerY
    Item3d {
        id: teaspoon1
        property real y_lift
        y: Math.max(y_lift,top.saucerY)
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

        SequentialAnimation {
            running: teaspoon1.bounce
            NumberAnimation {target: teaspoon1; property: "y_lift"; to : 1; duration: 300; easing.type:"OutQuad" }
            NumberAnimation {target: teaspoon1; property: "y_lift"; to : 0; duration: 300; easing.type:"OutBounce" }
            onCompleted: teaspoon1.bounce = false
        }
    }
}
