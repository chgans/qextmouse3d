import Qt 4.7

Rectangle { 
	id: button
	anchors.left : parent.left
	anchors.leftMargin: 40
	gradient: Gradient {
		GradientStop { position: 1.0; color: "black" }
		GradientStop { position: 0.0; color: "gray" }
	}
	border.color : "gray"
	border.width : 1
	radius: 10
	width: 610
	height: 51
	property bool bounce: false
	
	MouseArea {
		anchors.fill: parent
		onClicked: { button.bounce = true; console.log("Clicked") }
	}
	
	SequentialAnimation on scale{
		running: button.bounce
		NumberAnimation { to : 0.98; duration: 300; easing.type: "OutQuad" }
		NumberAnimation { to : 1.0; duration: 300; easing.type: "OutBounce" }
		onCompleted: button.bounce = false
    }
}
