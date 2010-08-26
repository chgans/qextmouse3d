import Qt 4.7
import Qt.labs.threed 1.0

	Viewport  {
		width: 1000
		height: 800
		
		camera: Camera { eye: Qt.vector3d(21,7,19)}
			
		Item3d {
			id: monkey
			scale: 2
			position: Qt.vector3d(0, 1.5, -6)
			
			mesh: Mesh { source: "meshes/monkey.3ds"; options: "ForceSmooth"}
			effect: Effect { material: gold}
		}
		
		Item3d
		{
			Penguin {position: Qt.vector3d(-1.5,0,1)}
			Penguin {position: Qt.vector3d(1.5,0,1)}
			Penguin {position: Qt.vector3d(-0.5,0,2)}
			Penguin {position: Qt.vector3d(0.5,0,2)}
			
			transform: [
				Rotation3D {id: swivel1; angle: -20; axis: Qt.vector3d(0,1,0)}
			]
			
			SequentialAnimation {
				running: true
				loops: 100
				NumberAnimation {target: swivel1; property: "angle"; to: 20; duration: 1200; easing.type: "OutQuad"}
				NumberAnimation {target: swivel1; property: "angle"; to: -20; duration: 1200; easing.type: "OutQuad"}
			}
		}
		
		Item3d {
			scale: 0.012
			y: -6.5
			z: -2
			mesh: Mesh { source: "meshes/lintel.3ds"}
			transform: [Rotation3D {angle: -90; axis: Qt.vector3d(1,0,0)}]
			effect: Effect{}
		}
		
		Item3d
		{
			Penguin {position: Qt.vector3d(-2.5,0,2)}
			Penguin {position: Qt.vector3d(2.5,0,2)}
			Penguin {position: Qt.vector3d(-1.5,0,3)}
			Penguin {position: Qt.vector3d(1.5,0,3)}
			Penguin {position: Qt.vector3d(-0.5,0,3.5)}
			Penguin {position: Qt.vector3d(0.5,0,3.5)}
			
			transform: [
				Rotation3D {id: swivel2; angle: 20; axis: Qt.vector3d(0,1,0)}
			]
			
			SequentialAnimation {
				running: true
				loops: 100
				NumberAnimation {target: swivel2; property: "angle"; to: -20; duration: 1200; easing.type: "OutQuad"}
				NumberAnimation {target: swivel2; property: "angle"; to: 20; duration: 1200; easing.type: "OutQuad"}
			}
		}
		
		Material {
			id: gold
			ambientColor: "#EEDD00"
			specularColor: "#FFFFFF"
			shininess: 200
		}
	}
