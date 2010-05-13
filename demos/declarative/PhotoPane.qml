import Qt 4.7
import Qt.labs.threed 1.0

Pane {
    id: photo

    property real offset
    property real layer
    property variant image

    width: 4
    height: 3
    position: Qt.vector3d(offset, 0, -layer)
    effect: Effect { texture: image }

    onClicked: {
        if (photo.state == "pulled_out")
            photo.state = "returned";
        else
            photo.state = "pulled_out";
    }

    states: [
        State {
            name: "pulled_out"
            PropertyChanges {
                target: photo
                x: 0
            }
            PropertyChanges {
                target: photo
                scale: 1.75
            }
            PropertyChanges {
                target: photo
                z: 2
            }
            PropertyChanges {
                target: viewport
                state: "show_photo"
            }
        },
        State {
            name: "returned"
            PropertyChanges {
                target: photo
                x: offset
            }
            PropertyChanges {
                target: photo
                scale: 1
            }
            PropertyChanges {
                target: photo
                z: -layer
            }
            PropertyChanges {
                target: viewport
                state: "show_group"
            }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            NumberAnimation {
                targets: photo
                properties: "x,scale,z"
                easing.type: "OutBounce"
                duration: 700
            }
        }
    ]
}
