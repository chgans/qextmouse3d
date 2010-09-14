import Qt 4.7
import Qt3D 1.0

Item3D {
    id: cube
    property real size: 1
    pretransform: Scale3D {
        scale: size
    }
    mesh: Mesh { source: "cube.obj" }
}
