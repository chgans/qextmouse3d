import Qt 4.7
import Qt3D 1.0
import Qt3D.Test 1.0

Viewport {
    id: viewport

    Item3D {
        id: "other"
        scale: 12
        position: Qt.vector3d(1.0, 3.0, 5.0);
        mesh: Mesh {

        }
        pretransform: [
            Rotation3D { id: other_item_pretransform1 ; angle: 30 ; axis: Qt.vector3d(0, 1, 0)},
            Rotation3D { id: other_item_pretransform2; angle: 90 ; axis: Qt.vector3d(1, 0, 0)}
        ]

        transform: [
            Rotation3D { id: other_teapot_rotate1 ; angle: 0 ; axis: Qt.vector3d(0, 1, 0)},
            Rotation3D { id: other_teapot_rotate2; angle: 0 ; axis: Qt.vector3d(0, 0, 1)}
        ]
    }

    Effect {
        id: "testEffect"
    }

    Light {
        id: "testLight"
    }

    Item3D {
        id: item
        property int positionHasBeenChanged: 0
        onPositionChanged : positionHasBeenChanged = positionHasBeenChanged + 1;

        property bool onScaleChangedSignalTriggered: false
        onScaleChanged: onScaleChangedSignalTriggered = true

        property bool onMeshChangedSignalTriggered: false
        onMeshChanged: onMeshChangedSignalTriggered = true

        property bool onEnabledChangedSignalTriggered:false
        onEnabledChanged: onEnabledChangedSignalTriggered = true

        property bool onEffectChangedSignalTriggered: false
        onEffectChanged: onEffectChangedSignalTriggered = true

        property bool onLightChangedSignalTriggered: false
        onLightChanged: onLightChangedSignalTriggered = true

        property int stateChangeTestVariable: 0

        property bool onStateChangedSignalTriggered: false
        onStateChanged: onStateChangedSignalTriggered = true

        Item3D {
            id: child
        }

        Item3D {
            id: child2
        }

        states: [
            State {
                name: "testState1"
                PropertyChanges { target: item; stateChangeTestVariable: 1}
            },
            State {
                name: "testState2"
                PropertyChanges { target: item; stateChangeTestVariable: 2}
            }
        ]

        TestCase {
            name: "Item3D"

            function test_position()
            {
                verify(item.position.x == 0, "Default position x");
                verify(item.position.y == 0, "Default position y");
                verify(item.position.z == 0, "Default position z");

                verify(item.position.x == 0, "Default x property");
                verify(item.position.y == 0, "Default y property");
                verify(item.position.z == 0, "Default z property");

                verify(!item.positionHasBeenChanged, "pre signal test");
                item.position = Qt.vector3d(1,-6,1234567);
                compare(item.positionHasBeenChanged, 1, "onPositionChanged signal");
                verify(item.position.x == 1, "Set position x");
                verify(item.position.y == -6, "Set position y negative");
                verify(item.position.z == 1234567, "seven significant digits");

                item.position.x = -1;
                item.position.y = 12;
                item.position.z = 83461;
                verify(item.x == -1, "x property");
                verify(item.y == 12, "y property");
                verify(item.z == 83461, "z property");
                compare(item.positionHasBeenChanged, 4, "onPositionChanged signal from x,y,z properties");
            }

            function test_scale()
            {
                verify(!item.onScaleChangedSignalTriggered);
                compare(item.scale, 1, "default value");
                item.scale = 2.5;
                compare(item.scale, 2.5, "setScale");
                verify(item.onScaleChangedSignalTriggered, "scaleChanged signal");
                item.scale = other.scale;
                compare(item.scale, 12, "setScale from other item");
            }

            function test_transform()
            {
                compare(item.transform.length, 0, "Default transformations are empty");
                compare(other.transform.length, 2, "non-empty transform");
                // no public set function
            }

            function test_pretransform()
            {
                compare(item.pretransform.length, 0, "Default pretransformations are empty");
                compare(other.pretransform.length, 2, "non-empty pretransform");
                // no public set function
            }

            function test_inheritEvents()
            {
                verify(!item.inheritEvents, "default inheritEvents is false");
                item.inheritEvents = true;
                verify(item.inheritEvents, "setInheritEvents()");
            }

            function test_mesh()
            {
                verify(!item.mesh, "default mesh value");
                verify(other.mesh != null, "non-null mesh");
                item.mesh = other.mesh;
                verify(item.onMeshChangedSignalTriggered, "MeshChanged signal");
            }

            function test_effect()
            {
                verify(!item.effect, "default effect is null");
                item.effect = testEffect;
                verify(item.onEffectChangedSignalTriggered, "onEffectChanged signal");
                verify(item.effect == testEffect, "setEffect()");
            }

            function test_light()
            {
                verify(!item.light, "default lightparamters is null");
                item.light = testLight;
                verify(item.onLightChangedSignalTriggered, "onLightChangedSignal");
                verify(item.light == testLight);
            }

            function test_state()

            {
                verify(!item.state, "Default state is null");
                verify(item.stateChangeTestVariable == 0, "property check before state change");
                item.state = "testState1";
                verify(item.onStateChangedSignalTriggered, "onStateChanged signal");
                verify(item.state == "testState1", "setState()");
                verify(item.stateChangeTestVariable == 1, "property changed with new state");
            }

            function test_enabled()
            {
                verify(item.enabled, "default value")
                item.enabled = false;
                verify(!item.enabled, "setEnabled false")
                verify(item.onEnabledChangedSignalTriggered, "enabledChanged signal")
            }

            function test_parent()
            {
                // Test the initial Item3D parent/child relationships.
                verify(item.parent == null, "root item")
                verify(child.parent == item, "child item")
                verify(child2.parent == item, "child2 item")
                compare(item.children.length, 2, "root children")
                compare(child.children.length, 0, "child children")
                verify(item.children[0] == child, "child 0")
                verify(item.children[1] == child2, "child 1")

                // Reparent the second child and re-test.
                child2.parent = child
                verify(item.parent == null, "root item (B)")
                verify(child.parent == item, "child item (B)")
                verify(child2.parent == child, "child2 item (B)")
                compare(item.children.length, 1, "root children (B)")
                compare(child.children.length, 1, "child children (B)")
                verify(item.children[0] == child, "child 0 (B)")
                verify(child.children[0] == child2, "child 1 (B)")
            }
        }
    }
}
