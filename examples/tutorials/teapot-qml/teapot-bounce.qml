import Qt 4.7
import Qt3D 1.0

Viewport {
    width: 640; height: 480

//! [1]
    Item3D {
        id: teapot
        mesh: Mesh { source: "teapot.bez" }

        effect: Effect {
            material: china
        }

        Material {
            id: china
            ambientColor: "#c09680"
            specularColor: "#3c3c3c"
            shininess: 128
        }
//! [1]

//! [3]
        SequentialAnimation on y {
            running: true
            loops: Animation.Infinite
            PauseAnimation { duration: 500 }
            NumberAnimation { to : 1.0; duration: 300; easing.type: "OutQuad" }
            NumberAnimation { to : 0.0; duration: 300; easing.type: "OutBounce" }
        }
//! [3]
//! [2]
    }
//! [2]
}
