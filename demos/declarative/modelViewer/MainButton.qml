import Qt 4.7
import Qt3D 1.0

Image { 
	id: mainbutton	
		
	source: imageSource
		
	width: 100 
	height: 100
	property bool bounce: false
	property string imageSource: "images/cog.svg"
	
	anchors.right: parent.right
	anchors.rightMargin: 10
	anchors.topMargin: 20

	
	SequentialAnimation on scale{
		running: mainbutton.bounce
		NumberAnimation { to : 0.8; duration: 50; easing.type: "OutQuad" }
		NumberAnimation { to : 1.0; duration: 50; easing.type: "OutBounce" }
		
		onCompleted: mainbutton.bounce = false
	}
}
