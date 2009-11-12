import Qt 4.6

Viewport {
    Light {}

    Cube {
        size: 1.5

        transform: Rotation3D {
            angle: 45
            axis: Qt.vector3d(1, 1, 1)
        }

        effect: Effect {
            color: "#aaca00"
            texture: "textures/qtlogo.png"
        }
    }
}
