import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480
    Item3d {
        id: teapot
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: Effect {
            material: china
        }

        SequentialAnimation on y{
            running: true
            loops: Animation.Infinite
            PauseAnimation { duration: 500 }
            NumberAnimation { to : 1.0; duration: 300; easing.type:"OutQuad" }
            NumberAnimation { to : 0.0; duration: 300; easing.type:"OutBounce" }
        }

        Light {}

        Material {
            id: china
            ambientColor: "#c09680"
            specularColor: "#3c3c3c"
            shininess: 128
        }
    }
}
