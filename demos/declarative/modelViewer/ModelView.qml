import Qt 4.7
import Qt.labs.threed 1.0

Rectangle {
	width: 800
	height: 500
Rectangle {
	id: mainwindow
    width: 1600	; height: 500

	gradient: Gradient {
         GradientStop { position: 0.0; color: "#300000" }
         GradientStop { position: 0.3; color: "darkred" }
         GradientStop { position: 1.0; color: "#300000" }
	}
	
	states: [
		State {
			name: "Open"
			
			PropertyChanges { target: mainwindow; x: -800}
			
		},
		State {
			name: "NotImplemented"
			
			//PropertyChanges { target: container; y: 0 }	
		}
	]

	transitions: Transition {
		NumberAnimation { properties: "x"; easing.type: Easing.OutQuad; duration: 600 }
	}
	
	
	Viewport {
		x: 800
		id: viewport1
		width: 800; height: 500
		picking: false
		backgroundColor: "transparent"

		camera: Camera {
			eye: "0, 0, 350"
			farPlane: 2000
		}

		
	Text { 
		id: imgSrc
		text: "Image Source"; font.family: "Helvetica"; font.pointSize: 22; color: "white" 
		anchors.top: parent.top
		z: 10
	}
	
	LongButton {
		//width: 800
		id: imgSrcButton
		anchors.top: imgSrc.bottom
		anchors.topMargin: 2
		z: 10
	}
	Text { 
		id: displayOptions
		anchors.top: imgSrcButton.bottom
		anchors.topMargin: 2
		text: "Display Options"; font.family: "Helvetica"; font.pointSize: 22; color: "white" 
		z: 10
	}
	FourButtons {
		id: displayOptionsButton
		anchors.top: displayOptions.bottom
		anchors.topMargin: 2
		z: 10
	}
	Text { 
		id: texture
		anchors.top: displayOptionsButton.bottom
		anchors.topMargin: 2
		text: "Texture"; font.family: "Helvetica"; font.pointSize:22; color: "white" 
		z: 10
	}
	LongButton {
		id: textureButton
		anchors.top: texture.bottom
		anchors.topMargin: 2
		z: 10
	}
	Text { 
		id: navigation
		anchors.top: textureButton.bottom
		anchors.topMargin: 2
		text: "Navigation"; font.family: "Helvetica"; font.pointSize: 22; color: "white" 
		z: 10
	}
	FourButtons {
		id: navigationButton
		anchors.top: navigation.bottom
		anchors.topMargin: 2		
		z: 10
	}
	Text { 
		id: background
		anchors.top: navigationButton.bottom
		anchors.topMargin: 2			
		text: "Background"; font.family: "Helvetica"; font.pointSize: 22; color: "white" 
		z: 10
	}
	LongButton {
		id: backgroundButton
		anchors.top: background.bottom
		anchors.topMargin: 2		
		z: 10
	}
		
		//Button 1
		MainButton {
			id: button1c
			anchors.top: parent.top
			imageSource: "images/cog.svg"
				
			
			MouseArea {
				//property bool on: false
				anchors.fill: parent
				onClicked: {
					//source_mesh.options =  "ForceSmooth";
					//source_mesh.source = textInput.text;
					parent.bounce = true;
					
					if (mainwindow.state == "")mainwindow.state = "Open"; else mainwindow.state = "";
				}
			}
		}
		
	}	
	
	Viewport {
		id: viewport
		width: 800; height: 500
		picking: false
		blending: true
		backgroundColor: "transparent"
		
		camera: Camera {
			eye: "0, 0, 350"
			farPlane: 2000
		}

		Item3d {
				id: mainItem
				scale: 50
				//y: -20
				
				transform: [
					Rotation3D {
						id: rotate1
						angle: -90
						axis: Qt.vector3d(1, 0, 0)
					},
					Rotation3D {
						id: rotate2
						angle: 0
						axis: Qt.vector3d(0, 1, 0)
					},
					Rotation3D {
						id: rotate3
						angle: 0
						axis: Qt.vector3d(1, 0, 0)
					}
				]
				mesh: source_mesh
				effect: Effect {}
				cullFaces: "CullBackFaces"
			}

		Mesh {
			id: source_mesh
			source: "meshes/monkey.3ds"
		}

		MouseArea {
			id: mouseArea
			property bool rotating: false
			property int startX: 0
			property int startY: 0
			property variant startEye
			property variant startCenter
			property variant startUpVector
			anchors.fill: parent
			onPressed: {
				if (mouse.button == Qt.LeftButton) {
					rotating = true;
					startX = mouse.x;
					startY = mouse.y;
					startEye = viewport.camera.eye;
					startCenter = viewport.camera.center;
					startUpVector = viewport.camera.upVector;
				}
			}
			onReleased: {
				if (mouse.button == Qt.LeftButton)
					rotating = false;
			}
			onPositionChanged: {
				if (rotating) {
					var deltaX = mouse.x - startX;
					var deltaY = mouse.y - startY;
					var angleAroundY = deltaX * 90 / mouseArea.width;
					var angleAroundX = deltaY * 90 / mouseArea.height;

					viewport.camera.eye = startEye;
					viewport.camera.center = startCenter;
					viewport.camera.upVector = startUpVector;
					viewport.camera.tiltCenter(-angleAroundX);
					viewport.camera.panCenter(-angleAroundY);
				}
			}
		}
			
		Rectangle
		{
			anchors.horizontalCenter: parent.horizontalCenter
			anchors.bottomMargin: 15
			anchors.bottom: parent.bottom
			radius: 10
			border.width: 1
			border.color: "black"
			color: "white"
			width: 500
			height: 25
			//Enter text here
			TextInput {
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				id: textInput
				text: source_mesh.source
				activeFocusOnPress: false
				MouseArea {
					anchors.fill: parent
					onClicked: {
						if (!textInput.activeFocus) {
							textInput.forceActiveFocus();
							textInput.openSoftwareInputPanel();
						} else {
							textInput.focus = false;
						}
					}
					onPressAndHold: textInput.closeSoftwareInputPanel();
				}
			}
		}
				
		
		//Button 1
		MainButton {
			id: button1
			anchors.top: parent.top
			imageSource: "images/cog.svg"
				
			
			MouseArea {
				//property bool on: false
				anchors.fill: parent
				onClicked: {
					//source_mesh.options =  "ForceSmooth";
					//source_mesh.source = textInput.text;
					parent.bounce = true;
					
					if (mainwindow.state == "")mainwindow.state = "Open"; else mainwindow.state = "";
				}
			}
		}
		
		//Button 2
		MainButton {
			id: button2
			anchors.top: button1.bottom
			imageSource: "images/zoomin.svg";
			property bool zoom: false
			property real changeScale: 1.1
			
			MouseArea {			
				anchors.fill: parent
				onClicked: {				
					button2.changeScale = 1.1
					button2.zoom = true
					parent.bounce = true;
				}
				onPressAndHold: {
					button2.changeScale = 2
					button2.zoom = true
					parent.bounce = true;
				}
				
			}
			
			SequentialAnimation {
				running: button2.zoom
				loops: 1
				NumberAnimation { target: mainItem; property: "scale"; to: mainItem.scale*button2.changeScale; duration: 50;}
				onCompleted: button2.zoom = false
			}
		}
		
		//Buton 3
		MainButton { 
			id: button3
			anchors.top: button2.bottom
			imageSource: "images/zoomout.svg"		
			property bool zoom: false			
			property real changeScale: 1.1
			
			MouseArea {
				anchors.fill: parent
				onClicked: {				
					button3.changeScale = 1.1
					button3.zoom = true
					parent.bounce = true;
				}
				onPressAndHold: {
					button3.changeScale = 2
					button3.zoom = true
					parent.bounce = true;
				}
			}
										
			SequentialAnimation {
				running: button3.zoom
				loops: 1
				NumberAnimation { target: mainItem; property: "scale"; to: mainItem.scale/button3.changeScale; duration: 50;}
				onCompleted: button3.zoom = false
			}
		}
		
		//Buton 4
		MainButton { 
			id: button4
			anchors.top: button3.bottom
			imageSource: "images/arrow.svg"
			
			MouseArea {
				anchors.fill: parent
				onClicked: {
					
					parent.bounce = true;
					
					source_mesh.source = textInput.text;
				}
			}
		}
		
		
	}	
}
}