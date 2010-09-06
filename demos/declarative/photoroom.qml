import Qt 4.7
import Qt.labs.threed 1.0

Viewport {
    id: viewport
    width: 800; height: 480
    picking: true
    //navigation: false
    //showPicking: true

    camera: Camera {
        id: main_camera
        eye: Qt.vector3d(0, 4, 12)
        center: Qt.vector3d(0, 0, -2.5)
    }

    PhotoPane {
        offset: -2.1
        layer: 0
        image: "textures/button/woman.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 1
        image: "textures/photos/niagara_falls.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 2
        image: "textures/photos/place.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 3
        image: "textures/photos/background.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 4
        image: "textures/basket.jpg"
    }
    PhotoPane {
        offset: -2.1
        layer: 5
        image: "textures/qtlogo.png"
        effect.color: "#006090"
    }

    PhotoPane {
        offset: 2.1
        layer: 0
        image: "textures/photos/niagara_falls.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 1
        image: "textures/photos/place.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 2
        image: "textures/photos/background.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 3
        image: "textures/basket.jpg"
    }
    PhotoPane {
        offset: 2.1
        layer: 4
        image: "textures/qtlogo.png"
        effect.color: "#006090"
    }
    PhotoPane {
        offset: 2.1
        layer: 5
        image: "textures/button/woman.jpg"
    }

    states: [
        State {
            name: "show_photo"
            PropertyChanges {
                target: main_camera
                eye.y: 0
            }
            PropertyChanges {
                target: main_camera
                eye.x: 0
            }
            PropertyChanges {
                target: main_camera
                eye.z: 12
            }
            PropertyChanges {
                target: main_camera
                upVector: Qt.vector3d(0, 1, 0)
            }
        },
        State {
            name: "show_group"
            PropertyChanges {
                target: main_camera
                eye.y: 4
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            NumberAnimation {
                targets: main_camera
                properties: "eye.y"
                easing.type: "OutBounce"
                duration: 700
            }
        }
    ]
}
