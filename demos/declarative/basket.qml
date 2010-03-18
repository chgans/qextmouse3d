import Qt 4.6

Viewport {
    camera: Camera {
        eye: Qt.vector3d(0, 4, 10)
    }

    Item3d {
        mesh: Mesh { source: "meshes/basket.bez" }
        transform: [
            Scale {
                xScale: 1.5
                yScale: 1.5
                zScale: 1.5
            },
            Rotation3D {
				id: rotation
                NumberAnimation on angle {
                    running: true
                    repeat: true
                    from: 0
                    to: 360
                    duration: 2000
                } 
                axis: Qt.vector3d(0, 1, 0)
            }
        ]

        effect: Effect {
            texture: "textures/basket.jpg"
        }
    }
}
