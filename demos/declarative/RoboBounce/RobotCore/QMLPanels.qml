import Qt 4.7
import Qt3D 1.0

Item {
	Rectangle {
		id: buttonBox1
		x: 30
		y: 110
		width: 60
		height: 160
		opacity: 0.5
		color: "black"
		border.color: "#2D918D"
		border.width: 2
		radius: 5
	}		
	PushButton {
		x: buttonBox1.x+10
		y: buttonBox1.y+10
	}
	PushButton {
		x: buttonBox1.x+10
		y: buttonBox1.y+60
	}
	PushButton {
		x: buttonBox1.x+10
		y: buttonBox1.y+110
	}
	
	Rectangle {
		id: buttonBox2
		x: 550
		y: 110
		width: 60
		height: 160
		opacity: 0.5
		color: "black"
		border.color: "#2D918D"
		border.width: 2
		radius: 5
	}
	PushButton {
		x: buttonBox2.x+10
		y: buttonBox2.y+10
	}
	PushButton {
		x: buttonBox2.x+10
		y: buttonBox2.y+60
	}
	PushButton {
		x: buttonBox2.x+10
		y: buttonBox2.y+110
	}	
	
	states: [
		State {},
		State {}
		
	]
}


