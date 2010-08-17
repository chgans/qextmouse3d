import Qt 4.7

Item {
	
	anchors.left : parent.left
	anchors.leftMargin: 40
	width: 750 
	height: 51
	
	
	ShortButton {
		id: button1
		//anchors.left: 
		//anchors.verticalCenter: parent.verticalCenter
	}
	ShortButton {
		id:  button2
		anchors.left: button1.right
		//anchors.leftMargin: 3
		//anchors.verticalCenter: parent.verticalCenter
	}
	ShortButton {
		id: button3
		anchors.left: button2.right
		//anchors.leftMargin: 3
		//anchors.verticalCenter: parent.verticalCenter
	}
	ShortButton {
		id: button4
		anchors.left: button3.right
		//anchors.leftMargin: 3
		//anchors.verticalCenter: parent.verticalCenter
	}
}