import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480
    Light {}
    Item3d {
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: Effect {}
    }
}
