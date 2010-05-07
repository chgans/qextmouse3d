import Qt 4.7

Item {
    CustomObject { id: theObject }
    property real scaleX : 1.0
    property real scaleY : 1.0
    property real scaleZ : 1.0
    property real translationX : 1.0
    property real translationY : 1.0
    property real translationZ : 1.0

    function createMatrix4x4WithConstants()
    {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
        theObject.variantProp = Qt3d.matrix4x4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0 );
    }

    function createOneMatrix4x4WithConstants()
    {
        theObject.variantProp = 0;
        theObject.variantProp = Qt3d.matrix4x4(
                1.0, 0.0, 0.0, 0.0,
                0.0, 1.0, 0.0, 0.0,
                0.0, 0.0, 1.0, 0.0,
                0.0, 0.0, 0.0, 1.0 );
    }

    function createMatrix4x4WithVariables()
    {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp = Qt3d.matrix4x4 (
                    scaleX, 0.0, 0.0, translationX,
                    0.0, scaleY, 0.0, translationY,
                    0.0, 0.0, scaleY, translationZ,
                    0.0, 0.0, 0.0, 1.0 );
    }

    function createVariantListWithConstants()
    {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp = [
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0 ];
    }

    function createOneVariantListWithConstants()
    {
        theObject.variantProp = 0;
        theObject.variantProp = [
            1.0, 0.0, 0.0, 0.0,
            0.0, 1.0, 0.0, 0.0,
            0.0, 0.0, 1.0, 0.0,
            0.0, 0.0, 0.0, 1.0 ];
    }

    function createVariantListWithVariables() {
        theObject.variantProp = 0;
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp = [
            scalex, 0.0, 0.0, translationX,
            0.0, scaley, 0.0, translationY,
            0.0, 0.0, scalez, translationZ,
            0.0, 0.0, 0.0, 1.0 ];
    }

    function modifyMatrix4x4() {
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp.m(2,3) = 1.0;
    }

    function modifyVariantList() {
        for (var i = 0; i < 1000; ++i)
            theObject.variantProp[4] = 2.0;
    }

}
