/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

    Scale3D {
        id: scale1
        scale: 2.5
    }

    Scale3D {
        id: scale2
        scale: Qt.vector3d(1.0, 0.5, -2.0)
    }

    Scale3D {
        id: scale3
        property int changed: 0
        onScaleChanged: ++changed
    }

    Scale3D {
        id: scale4
        property int changed: 0
        onOriginChanged: ++changed
    }

    TestCase {
        name: "Scale3D"

        function test_uniformScale() {
            compare(scale1.scale, Qt.vector3d(2.5, 2.5, 2.5), "uniform1")
            scale1.scale = 4.5
            compare(scale1.scale, Qt.vector3d(4.5, 4.5, 4.5), "uniform2")
            scale1.scale = Qt.vector3d(3.0, -2.5, 1.5)
            compare(scale1.scale, Qt.vector3d(3.0, -2.5, 1.5), "uniform3")
        }

        function test_nonUniformScale() {
            compare(scale2.scale, Qt.vector3d(1.0, 0.5, -2.0), "nonuniform1")
            scale2.scale = Qt.vector3d(3.0, -2.5, 1.5)
            compare(scale2.scale, Qt.vector3d(3.0, -2.5, 1.5), "nonuniform2")
            scale2.scale = 4.5
            compare(scale2.scale, Qt.vector3d(4.5, 4.5, 4.5), "nonuniform3")
        }

        function test_scaleChanged() {
            compare(scale3.scale, Qt.vector3d(1.0, 1.0, 1.0), "changed1")
            compare(scale3.changed, 0, "changed2")
            scale3.scale = Qt.vector3d(3.0, -2.5, 1.5)
            compare(scale3.scale, Qt.vector3d(3.0, -2.5, 1.5), "changed3")
            compare(scale3.changed, 1, "changed3")
            scale3.scale = 4.5
            compare(scale3.scale, Qt.vector3d(4.5, 4.5, 4.5), "changed4")
            compare(scale3.changed, 2, "changed4")
        }

        function test_originChanged() {
            compare(scale4.origin, Qt.vector3d(0.0, 0.0, 0.0), "changed1")
            compare(scale4.changed, 0, "changed2")
            scale4.origin = Qt.vector3d(3.0, -2.5, 1.5)
            compare(scale4.origin, Qt.vector3d(3.0, -2.5, 1.5), "changed3")
            compare(scale4.changed, 1, "changed3")
        }
    }
}
