import Qt 4.7
import Qt3D 1.0

Image {
	id: theButton
	property bool bounce: false	
	source: "../Assets/button.png"	
	width: 40
	height: 40
	
	signal pressed
	
	SequentialAnimation {
		running: theButton.bounce
		NumberAnimation {target: theButton; property: "scale"; duration: 50; to: 0.8;}
		NumberAnimation {target: theButton; property: "scale"; duration: 50; to: 1;}
		onCompleted: {theButton.bounce = false}
	}
	
	MouseArea {					
		anchors.fill: parent
		onClicked: {
			theButton.bounce=true
			theButton.pressed()
		}
	}
}


