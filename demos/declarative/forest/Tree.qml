//![1]
import Qt 4.7
import Qt.labs.threed 1.0

Item3d {
    id: tree
    mesh: Mesh { source: "tree.obj" }
    effect: Effect {
        blending: true
        texture: "tree.png"
    }
//![1]
//![2]
    transform: BillboardTransform {
        preserveUpVector: true
    }
//![2]
//![3]
}
//![3]
