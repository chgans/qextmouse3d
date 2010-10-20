//![1]
//![2]
import Qt 4.7
import Qt3D 1.0
//![2]

//![3]
Viewport {
    width: 640; height: 480
//![3]
//![4]
    Item3D {
        id: teapot
        mesh: Mesh { source: "teapot.bez" }
        effect: Effect {}
    }
//![4]
}
//![1]
