import Qt 4.7
import Qt.labs.threed 1.0

Item3d {
    id: cube
    property real size: 1
    pretransform: Scale3D {
        scale: size
    }
    mesh: Mesh { source: "cube.obj" }
}
