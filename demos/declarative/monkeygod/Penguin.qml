import Qt 4.7
import Qt.labs.threed 1.0

Item3d {
	id: penguin
	scale: 0.2
	effect: Effect{}
	transform: [
		Rotation3D {
			id: penguinTilt
			angle: -100
			axis: Qt.vector3d(1,0,0)
		},
		Rotation3D {
			id: penguinFacing
			angle: 180
			axis: Qt.vector3d(0,1,0)
		}
	]
	
	Light {}
	
	mesh: Mesh { source: "meshes/penguin.3ds" }
	
	SequentialAnimation {
		running: true
		loops: 100
		NumberAnimation {target: penguinTilt; property: "angle"; to: -45; duration: 600; easing.type: "OutQuad"}
		NumberAnimation {target: penguinTilt; property: "angle"; to: -100; duration: 600; easing.type: "OutQuad"}
	}
	
	SequentialAnimation on y {
		running: true
		loops: 100
		NumberAnimation {to: 0.5; duration: 200; easing.type: "OutQuad"}
		NumberAnimation {to: 0.0; duration: 200; easing.type: "OutQuad"}
	}
}
