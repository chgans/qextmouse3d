//! [1]
import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480

    Item3d {
        id: pyramid
        mesh: pyramidMesh
        effect: Effect {
            texture: "brick.png"
        }
        transform: Rotation3D {
            axis: Qt.vector3d(0, 1, 0)
            NumberAnimation on angle {
                running: true
                loops: Animation.Infinite
                from: 0
                to: 360
                duration: 3000
            }
        }
    }
//! [1]

//! [2]
    InlineMesh {
        id: pyramidMesh
        primitiveType: InlineMesh.Triangles
        vertices: [
            -1, -1, 1,
            1, -1, 1,
            0, 1, 0,
//! [2]
            1, -1, 1,
            1, -1, -1,
            0, 1, 0,
            1, -1, -1,
            -1, -1, -1,
            0, 1, 0,
            -1, -1, -1,
            -1, -1, 1,
            0, 1, 0,
            1, -1, 1,
            -1, -1, 1,
            1, -1, -1,
            -1, -1, -1,
            1, -1, -1,
            -1, -1, 1
//! [3]
        ]
        textureCoords: [
            0, 0,
            1, 0,
            0.5, 1,
//! [3]
            0, 0,
            1, 0,
            0.5, 1,
            0, 0,
            1, 0,
            0.5, 1,
            0, 0,
            1, 0,
            0.5, 1,
            0, 0,
            1, 0,
            0, 1,
            1, 1,
            0, 1,
            1, 0
//! [4]
        ]
    }
//! [4]
//! [5]
}
//! [5]
