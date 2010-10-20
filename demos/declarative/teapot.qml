import Qt 4.7
import Qt3D 1.0

Viewport {
    width: 640; height: 480
    Item3D {
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: Effect {}
    }
}
