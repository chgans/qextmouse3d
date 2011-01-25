/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt 4.7
import Qt3D 1.0
import QtQuickTest 1.0

Viewport {
    id: viewport

    Item3DClicker {
        id: mouseClicker
    }

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

        property bool onSortChildrenSignalTriggered: false
        onSortChildrenChanged: onSortChildrenSignalTriggered = true

        property bool onClickedSignalTriggered: false
        onClicked: onClickedSignalTriggered = true

        Item3D {
            id: child1
        }

        Item3D {
            id: child2
        }

        Item3D {
            id: inheritEventTestChild
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

                verify(item.x == 0, "Default x");
                verify(item.y == 0, "Default y");
                verify(item.z == 0, "Default z");

                verify(!item.positionHasBeenChanged, "pre signal test");
                item.position = Qt.vector3d(1,-6,1234567);
                compare(item.positionHasBeenChanged, 1, "onPositionChanged signal");
                verify(item.position.x == 1, "Set position x");
                verify(item.position.y == -6, "Set position y negative");
                verify(item.position.z == 1234567, "seven significant digits");

                verify(item.x == 1, "position changes x");
                verify(item.y == -6, "position changes y");
                verify(item.z == 1234567, "position changex z");

                item.position.x = -1;
                compare(item.positionHasBeenChanged, 2, "onPositionChanged signal from position.x property");
                item.position.y = 12;
                compare(item.positionHasBeenChanged, 3, "onPositionChanged signal from position.y property");
                item.position.z = 83461;
                compare(item.positionHasBeenChanged, 4, "onPositionChanged signal from position.z property");
                verify(item.x == -1, "position.x changes x");
                verify(item.y == 12, "position.y changes y");
                verify(item.z == 83461, "position.z changes z");
            }

            function test_x()
            {
                item.positionHasBeenChanged = 0;
                verify(!item.positionHasBeenChanged, "pre signal test");

                item.x = 324;
                verify(item.x == 324, "Set x");
                verify(item.position.x == 324, "position modified by setting x");
                compare(item.positionHasBeenChanged, 1, "onPositionChanged signal after setting x");

                item.x = -7;
                verify(item.x == -7, "Set x negative");
                verify(item.position.x == -7, "position modified by setting x negative");
                compare(item.positionHasBeenChanged, 2, "onPositionChanged signal after setting x negative");

                item.position = Qt.vector3d(2,-7,2345671);
                verify(item.x == 2, "x changed by setting postion");
            }

            function test_y()
            {
                item.positionHasBeenChanged = 0;
                verify(!item.positionHasBeenChanged, "pre signal test");

                item.y = 346;
                verify(item.y == 346, "Set y");
                verify(item.position.y == 346, "position modified by setting y");
                compare(item.positionHasBeenChanged, 1, "onPositionChanged signal after setting y");

                item.y = -12;
                verify(item.y == -12, "Set y negative");
                verify(item.position.y == -12, "position modified by setting y negative");
                compare(item.positionHasBeenChanged, 2, "onPositionChanged signal after setting y negative");

                item.position = Qt.vector3d(2, 13,2345671);
                verify(item.y == 13, "y changed by setting postion");
            }

            function test_z()
            {
                item.positionHasBeenChanged = 0;
                verify(!item.positionHasBeenChanged, "pre signal test");

                item.z = 19375;
                verify(item.z == 19375, "Set z");
                verify(item.position.z == 19375, "position modified by setting z");
                compare(item.positionHasBeenChanged, 1, "onPositionChanged signal after setting z");

                item.z = -74635;
                verify(item.z == -74635, "Set z negative");
                verify(item.position.z == -74635, "position modified by setting z negative");
                compare(item.positionHasBeenChanged, 2, "onPositionChanged signal after setting z negative");

                item.z = 0.435;
                compare(item.z, 0.435, "setting z to a real number")
                compare(item.position.z, 0.435, "position modified by setting z to a real");
                compare(item.positionHasBeenChanged, 3, "onPositionChanged signal after setting z to a real");

                item.position = Qt.vector3d(2, 67,2345671);
                verify(item.z == 2345671, "z changed by setting postion");
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
                compare(item.onClickedSignalTriggered, false, "pretest onClickedSignalTriggered check");
                compare(inheritEventTestChild.parent, item, "pretest parent check");

                mouseClicker.click(inheritEventTestChild);
                compare(item.onClickedSignalTriggered, false,
                        "no parent event when inheritEvents is false");

                item.inheritEvents = true;
                verify(item.inheritEvents, "setInheritEvents()");
                mouseClicker.click(inheritEventTestChild);
                compare(item.onClickedSignalTriggered, true, "parent event when inheritEvents is true");
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
                verify(child1.parent == item, "child1 item")
                verify(child2.parent == item, "child2 item")
                compare(item.children.length, 3, "root children")
                compare(child1.children.length, 0, "child1 children")
                verify(item.children[0] == child1, "children[0] is child1")
                verify(item.children[1] == child2, "children[1] is child2")

                // Reparent the second child and re-test.
                child2.parent = child1
                verify(item.parent == null, "root item (B)")
                verify(child1.parent == item, "child1's parent (B)")
                verify(child2.parent == child1, "child2's parent (B)")
                compare(item.children.length, 2, "root children (B)")
                compare(child1.children.length, 1, "child1 children (B)")
                verify(item.children[0] == child1, "children[0] is child1 (B)")
                verify(child1.children[0] == child2, "children[1] is child2 (B)")
            }

            function test_sortChildren()
            {
                verify(item.sortChildren == Item3D.DefaultSorting, "default value")
                item.sortChildren = Item3D.BackToFront;
                verify(item.sortChildren == Item3D.BackToFront, "back-to-front")
                verify(item.onSortChildrenSignalTriggered, "sortChildren signal")
            }
        }
    }
}
