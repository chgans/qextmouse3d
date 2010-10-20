import Qt 4.7
import Qt3D 1.0

Item3D {
    mesh: Mesh { source: "meshes/teapot.bez" }
    effect: Effect {}
    property int xRotation : 0.0
    property int yRotation : 0.0
    property int zRotation : 0.0

    transform: [
        Rotation3D {
            angle: xRotation
            axis: Qt.vector3d(1, 0, 0)
        },
        Rotation3D {
            angle: yRotation
            axis: Qt.vector3d(0, 1, 0)
        },
        Rotation3D {
            angle: zRotation
            axis: Qt.vector3d(0, 0, 1)
        }
    ]
}
