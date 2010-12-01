import Qt 4.7
import Qt3D 1.0
import Qt3D.Test 1.0

Viewport {
    id: viewport


    Item3D {
        id: rotateTestItem
        Item3D {
            id: rotateTestItemChild
            transform: Rotation3D {angle: 90 ; axis: Qt.vector3d(0,1,0)}
        }
        transform: Rotation3D {angle: 90 ; axis: Qt.vector3d(0,1,0)}
    }

    Item3D {
        id: scaleTestItem
        Item3D {
            id: scaleTestItemChild
            transform: Scale3D { scale: 0.5 }
        }
        transform: Scale3D { scale: 2.0 }
    }

    Item3D {
        id: translateTestItem
        position: Qt.vector3d(1,1,1)

        Item3D {
            id: child
            position: Qt.vector3d(1,1,1)
        }

        TestCase {
            name: "Item3DRelativeTransformTests"

            function test_localToWorld_translate()
            {
                var testVectorNull = Qt.vector3d(0,0,0);
                var resultVector = translateTestItem.localToWorld(testVectorNull);

                compare(resultVector.x, translateTestItem.position.x, "testVectorNull x translation");
                compare(resultVector.y, translateTestItem.position.y, "testVectorNull y translation");
                compare(resultVector.z, translateTestItem.position.z, "testVectorNull z translation");

                var childResult = child.localToWorld(testVectorNull);
                compare(childResult.x, translateTestItem.x + child.x, "child testVectorNull x translation");
                compare(childResult.y, translateTestItem.y + child.y, "child testVectorNull y translation");
                compare(childResult.z, translateTestItem.z + child.z, "child testVectorNull z translation");

                var testVector2 = Qt.vector3d(-1,0,123456);
                var childResult2 = child.localToWorld(testVector2);
                compare(childResult2.x, translateTestItem.x + child.x + testVector2.x, "non-zero point, x translation");
                compare(childResult2.y, translateTestItem.y + child.y, "non-zero point, y translation");
                compare(childResult2.z, translateTestItem.z + child.z + testVector2.z, "non-zero point, z translation");
            }

            function test_localToWorld_rotate()
            {
                var testVectorX = Qt.vector3d(1,0,0);
                var resultVector = rotateTestItem.localToWorld(testVectorX);
                compare(resultVector.x, 0, "x value of testVectorX rotated 90 degrees around y unit vector");
                compare(resultVector.y, 0, "y value of testVectorX rotated 90 degrees around y unit vector");
                compare(resultVector.z, -1, "z value of testVectorX rotated 90 degrees around y unit vector");

                var testVectorY = Qt.vector3d(0,1,0);
                var resultVectorY = rotateTestItem.localToWorld(testVectorY);
                compare(resultVectorY.x, 0, "x value of testVectorY rotated 90 degrees around y unit vector");
                compare(resultVectorY.y, 1, "y value of testVectorY rotated 90 degrees around y unit vector");
                compare(resultVectorY.z, 0, "z value of testVectorY rotated 90 degrees around y unit vector");
            }

            function test_localToWorld_scale()
            {
                var testVectorNull = Qt.vector3d(0,0,0);
                var testVectorX = Qt.vector3d(1,0,0);

                var resultVector = scaleTestItem.localToWorld(testVectorNull);
                compare(resultVector.x, 0, "x value of testVectorNull scale: 2.0");
                compare(resultVector.y, 0, "y value of testVectorNull scale: 2.0");
                compare(resultVector.z, 0, "z value of testVectorNull scale: 2.0");

                resultVector = scaleTestItem.localToWorld(testVectorX);
                compare(resultVector.x, 2, "testVectorX.x scale: 2.0");
                compare(resultVector.y, 0, "testVectorX.y scale: 2.0");
                compare(resultVector.z, 0, "testVectorX.z scale: 2.0");

                resultVector = scaleTestItemChild.localToWorld(testVectorNull);
                compare(resultVector.x, testVectorNull.x, "testVectorNull.x scaled up and back down");
                compare(resultVector.y, testVectorNull.y, "testVectorNull.y scaled up and back down");
                compare(resultVector.z, testVectorNull.z, "testVectorNull.z scaled up and back down");

                resultVector = scaleTestItemChild.localToWorld(testVectorX);
                compare(resultVector.x, testVectorX.x, "testVectorX.x scaled up and back down");
                compare(resultVector.y, testVectorX.y, "testVectorX.y scaled up and back down");
                compare(resultVector.z, testVectorX.z, "testVectorX.z scaled up and back down");
            }

            function test_worldToLocal_rotate()
            {
                var testVector = Qt.vector3d(0,0,-1);
                var resultVector = rotateTestItem.worldToLocal(testVector);
                compare(resultVector.x, 1, "x value of x unitVector rotated 90 degrees around y unit vector");
                compare(resultVector.y, 0, "y value of x unitVector rotated 90 degrees around y unit vector");
                compare(resultVector.z, 0, "z value of x unitVector rotated 90 degrees around y unit vector");
            }

            function test_worldToLocal_scale()
            {
                var testVectorNull = Qt.vector3d(0,0,0);
                var testVectorX = Qt.vector3d(1,0,0);

                var resultVector = scaleTestItem.worldToLocal(testVectorNull);
                compare(resultVector.x, 0, "x value of testVectorNull scale: 2.0");
                compare(resultVector.y, 0, "y value of testVectorNull scale: 2.0");
                compare(resultVector.z, 0, "z value of testVectorNull scale: 2.0");

                resultVector = scaleTestItem.worldToLocal(testVectorX);
                compare(resultVector.x, 0.5, "testVectorX.x scale: 2.0");
                compare(resultVector.y, 0, "testVectorX.y scale: 2.0");
                compare(resultVector.z, 0, "testVectorX.z scale: 2.0");

                resultVector = scaleTestItemChild.worldToLocal(testVectorNull);
                compare(resultVector.x, testVectorNull.x, "testVectorNull.x scaled up and back down");
                compare(resultVector.y, testVectorNull.y, "testVectorNull.y scaled up and back down");
                compare(resultVector.z, testVectorNull.z, "testVectorNull.z scaled up and back down");

                resultVector = scaleTestItemChild.worldToLocal(testVectorX);
                compare(resultVector.x, testVectorX.x, "testVectorX.x scaled up and back down");
                compare(resultVector.y, testVectorX.y, "testVectorX.y scaled up and back down");
                compare(resultVector.z, testVectorX.z, "testVectorX.z scaled up and back down");
            }

            function test_worldToLocal_translate()
            {
                var testVectorNull = Qt.vector3d(0,0,0);
                var resultVectorNull = translateTestItem.worldToLocal(testVectorNull);

                compare(resultVectorNull.x, -translateTestItem.position.x, "testVectorNull x translation");
                compare(resultVectorNull.y, -translateTestItem.position.y, "testVectorNull y translation");
                compare(resultVectorNull.z, -translateTestItem.position.z, "testVectorNull z translation");

                var childResult = child.worldToLocal(testVectorNull);
                compare(childResult.x, -translateTestItem.x - child.x, "child x translation");
                compare(childResult.y, -translateTestItem.y - child.y, "child y translation");
                compare(childResult.z, -translateTestItem.z - child.z, "child z translation");

                var testVector2 = Qt.vector3d(-1,0,123456);
                var childResult2 = child.worldToLocal(testVector2);
                compare(childResult2.x, testVector2.x -translateTestItem.x - child.x, "non-zero point, x translation");
                compare(childResult2.y, testVector2.y -translateTestItem.y - child.y, "non-zero point, y translation");
                compare(childResult2.z, testVector2.z -translateTestItem.z - child.z, "non-zero point, z translation");
            }
        }
    }
}
