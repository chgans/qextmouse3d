import Qt 4.6

Viewport {
    width: 640; height: 480
    Item3d {
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: Effect {}

        Light {}
    }
}
