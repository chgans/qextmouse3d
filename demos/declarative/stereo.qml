import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    width: 640; height: 480
    camera: Camera {
        eye: Qt.vector3d(0, 0, 15)
        eyeSeparation: 0.2
    }

    Light {}

    Item3d {
        id: teapot
        mesh: Mesh { source: "meshes/teapot.bez" }
        effect: Effect {
            material: china
        }

        SequentialAnimation on z {
            running: true
            loops: Animation.Infinite
            PauseAnimation { duration: 500 }
            NumberAnimation { to : 9.0; duration: 300; easing.type:"OutQuad" }
            NumberAnimation { to : 0.0; duration: 300; easing.type:"OutBounce" }
        }
    }

    Material {
        id: china
        ambientColor: "#c09680"
        specularColor: "#3c3c3c"
        shininess: 128
    }
}
