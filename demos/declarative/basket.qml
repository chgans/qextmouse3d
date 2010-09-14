//! [1]
import Qt 4.7
import Qt3D 1.0

Viewport {
    width: 640; height: 480

    camera: Camera {
        eye: Qt.vector3d(0, 4, 10)
    }
//! [1]

//! [2]
    Item3d {
        mesh: Mesh { source: "meshes/basket.bez" }
        effect: Effect { texture: "textures/basket.jpg" }
//! [2]
//! [3]
        transform: [
            Scale3D { scale: 1.5 },
            Rotation3D {
                axis: Qt.vector3d(0, 1, 0)
                NumberAnimation on angle {
                    running: true
                    loops: Animation.Infinite
                    from: 0
                    to: 360
                    duration: 2000
                } 
            }
        ]
    }
}
//! [3]
