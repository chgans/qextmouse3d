//![1]
import Qt 4.7
import Qt3D 1.0

Viewport {
    id: viewport
    width: 640; height: 480
    picking: false

    camera: Camera {
        eye: Qt.vector3d(0, 4, 12)
    }

    Item3d {
        id: ground
        mesh: Mesh { source: "ground.obj" }
        effect: Effect {
            color: "#604000"
            useLighting: false
        }
    }

    Tree { position: Qt.vector3d(-1.5, 0, -1) }
    Tree { position: Qt.vector3d(-0.5, 0, -1) }
    Tree { position: Qt.vector3d(0.5, 0, -1) }
    Tree { position: Qt.vector3d(1.5, 0, -1) }

    Tree { position: Qt.vector3d(0, 0, 0) }
    Tree { position: Qt.vector3d(-1, 0, 0) }
    Tree { position: Qt.vector3d(1, 0, 0) }

    Tree { position: Qt.vector3d(-1.5, 0, 1) }
    Tree { position: Qt.vector3d(-0.5, 0, 1) }
    Tree { position: Qt.vector3d(0.5, 0, 1) }
    Tree { position: Qt.vector3d(1.5, 0, 1) }
}
//![1]
