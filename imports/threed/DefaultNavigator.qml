import Qt 4.7
import Qt.labs.threed 1.0

MouseArea {
    // Set this property to the "Viewport" item to be controlled
    // by this navigator.  By default we assume that it is our parent.
    property variant viewport: parent

    // The navigator fills the entire viewport.
    anchors.fill: viewport

    // Use hover if the viewport supports picking.
    hoverEnabled: viewport.picking

    // Private variables.
    QtObject {
        id: d_ptr
        property bool panning: false
        property int startX: 0
        property int startY: 0
        property variant startEye
        property variant startCenter
        property variant startUpVector
        property variant pressedObject: null
        property int pressedButton: 0
    }

    // Handle mouse press events.
    onPressed: {
        // Determine which object was pressed in the viewport.
        var objectUnderMouse;
        if (!d_ptr.panning && viewport.picking)
            objectUnderMouse = null;
        else
            objectUnderMouse = null;
        if (d_ptr.pressedObject) {
            // Send the press event to the pressed object.  Use a position
            // of (0, 0) if the mouse is still within the pressed object,
            // or (-1, -1) if the mouse is no longer within the pressed object.
            // TODO
        } else if (objectUnderMouse) {
            // Mouse press on a new object.
            // TODO
        } else if (viewport.navigation && mouse.button == Qt.LeftButton) {
            // Start panning the view.
            d_ptr.panning = true;
            d_ptr.startX = mouse.x;
            d_ptr.startY = mouse.y;
            d_ptr.startEye = viewport.camera.eye;
            d_ptr.startCenter = viewport.camera.center;
            d_ptr.startUpVector = viewport.camera.upVector;
            mouse.accepted = true;
        }
    }

    // Handle mouse release events.
    onReleased: {
        if (d_ptr.panning && mouse.button == Qt.LeftButton)
            d_ptr.panning = false;
        if (pressedObject) {
            // Deliver the release event to the pressed object.
            // TODO
        }
    }

    // Handle mouse double click events.
    onDoubleClicked: {
        // TODO
    }

    // Handle mouse move events.
    onPositionChanged: {
        if (d_ptr.panning) {
            var deltaX = mouse.x - d_ptr.startX;
            var deltaY = mouse.y - d_ptr.startY;
            var angleAroundY = deltaX * 90 / width;
            var angleAroundX = deltaY * 90 / height;

            viewport.camera.eye = d_ptr.startEye;
            viewport.camera.center = d_ptr.startCenter;
            viewport.camera.upVector = d_ptr.startUpVector;
            viewport.camera.tiltPanRollCenter
                (-angleAroundX, -angleAroundY, 0, "TiltPanRoll");
        } else if (viewport.picking) {
            // Handle object enter/leave events.
            // TODO
        }
    }
}
