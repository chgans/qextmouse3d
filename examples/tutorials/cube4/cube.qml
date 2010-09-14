//! [1]
import Qt 4.7
import Qt3D 1.0
import Qt3D.Shapes 1.0

Viewport {
    width: 640; height: 480

    Cube {
        size: 1.0
        transform: Rotation3D {
            angle: 45; axis: Qt.vector3d(1, 1, 1)
        }

        effect: Effect {
            color: "#aaca00";
            texture: "qtlogo.png"
            decal: true
        }
    }
}
//! [1]
