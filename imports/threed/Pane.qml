import Qt 4.7
import Qt.labs.threed 1.0

Item3d {
    id: pane
    property real width: 1
    property real height: 1
    pretransform: Scale3D {
        xScale: width
        yScale: height
    }
    mesh: InlineMesh {
        primitiveType: InlineMesh.Quads
        smooth: false
        vertices: [
            -0.5, -0.5, 0.0,
             0.5, -0.5, 0.0,
             0.5,  0.5, 0.0,
            -0.5,  0.5, 0.0
        ]
        textureCoords: [
            0.0, 0.0,
            1.0, 0.0,
            1.0, 1.0,
            0.0, 1.0
        ]
    }
}
