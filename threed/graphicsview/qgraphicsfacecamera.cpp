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

#include "qgraphicsfacecamera.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsFaceCamera
    \brief The QGraphicsFaceCamera class implements a transformation that causes objects to face the camera.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::graphicsview
    \ingroup qt3d::qml3d

    Sometimes it can be useful to make an object face towards the camera
    no matter what orientation the scene is in.  The common name for
    this technique is "billboarding".

    When applied as a transformation, this class will replace the top-left
    3x3 part of the transformation matrix with the identity.  This has the
    effect of removing the rotation and scale components from the current
    world co-ordinate orientation.  In QML, this can be used as follows
    to orient a Pane to point towards the viewer:

    \code
    Pane {
        position: Qt.vector3d(2, 0, -20)
        transform: FaceCamera {}
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode

    Because the camera-facing transformation will strip any further
    alterations to the matrix, it will usually be the last transformation
    that is applied to the \c transform property:

    \code
    Pane {
        position: Qt.vector3d(2, 0, -20)
        transform: [
            Scale3D { scale: 0.5 },
            Rotation3D { angle: 30 },
            FaceCamera {}
        ]
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode

    The \c scale property is applied after \c transform has performed
    the camera-facing transformation, so the above can also be written
    as follows:

    \code
    Pane {
        position: Qt.vector3d(2, 0, -20)
        scale: 0.5
        transform: [
            Rotation3D { angle: 30 },
            FaceCamera {}
        ]
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode

    By default the camera-facing transform will cause the object to
    face directly at the camera no matter how the world co-ordinate
    system is rotated.  Sometimes the billboard needs to stay at right
    angles to the "ground plane" even if the user's viewpoint is
    elevated.  This behavior can be enabled using the preserveUpVector()
    property:

    \code
    Pane {
        position: Qt.vector3d(2, 0, -20)
        transform: FaceCamera { preserveUpVector: true }
        effect: Effect { texture: "picture.jpg" }
    }
    \endcode
*/

class QGraphicsFaceCameraPrivate
{
public:
    QGraphicsFaceCameraPrivate() : preserveUpVector(false) {}

    bool preserveUpVector;
};

/*!
    Construct a camera-facing transform and attach it to \a parent.
*/
QGraphicsFaceCamera::QGraphicsFaceCamera(QObject *parent)
    : QGraphicsTransform(parent), d_ptr(new QGraphicsFaceCameraPrivate)
{
}

/*!
    Destroy this camera-facing transform.
*/
QGraphicsFaceCamera::~QGraphicsFaceCamera()
{
}

/*!
    \property QGraphicsFaceCamera::preserveUpVector
    \brief true to preserve the up orientation.

    The default value for this property is false, which indicates that
    the object being transformed should always face directly to the camera
    This is also known as a "spherical billboard".

    If the value for this property is true, then the object will have
    its up orientation preserved.  This is also known as a "cylindrical
    billboard".
*/

bool QGraphicsFaceCamera::preserveUpVector() const
{
    Q_D(const QGraphicsFaceCamera);
    return d->preserveUpVector;
}

void QGraphicsFaceCamera::setPreserveUpVector(bool value)
{
    Q_D(QGraphicsFaceCamera);
    if (d->preserveUpVector != value) {
        d->preserveUpVector = value;
        update();
        emit preserveUpVectorChanged();
    }
}

/*!
    \internal
*/
void QGraphicsFaceCamera::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QGraphicsFaceCamera);
    if (!d->preserveUpVector) {
        // Replace the top-left 3x3 of the matrix with the identity.
        // The technique is "Cheating Spherical Billboards", described here:
        // http://www.lighthouse3d.com/opengl/billboarding/index.php?billCheat
        (*matrix)(0, 0) = 1.0f;
        (*matrix)(0, 1) = 0.0f;
        (*matrix)(0, 2) = 0.0f;
        (*matrix)(1, 0) = 0.0f;
        (*matrix)(1, 1) = 1.0f;
        (*matrix)(1, 2) = 0.0f;
        (*matrix)(2, 0) = 0.0f;
        (*matrix)(2, 1) = 0.0f;
        (*matrix)(2, 2) = 1.0f;
    } else {
        // Replace some of the top-left 3x3 of the matrix with the identity,
        // but leave the up vector component in the second column as-is.
        // The technique is "Cheating Cylindrical Billboards", described here:
        // http://www.lighthouse3d.com/opengl/billboarding/index.php?billCheat1
        (*matrix)(0, 0) = 1.0f;
        (*matrix)(0, 2) = 0.0f;
        (*matrix)(1, 0) = 0.0f;
        (*matrix)(1, 2) = 0.0f;
        (*matrix)(2, 0) = 0.0f;
        (*matrix)(2, 2) = 1.0f;
    }
    matrix->optimize();
}

/*!
    \fn void QGraphicsFaceCamera::preserveUpVectorChanged()

    Signal that is emitted when preserveUpVector() changes.
*/

QT_END_NAMESPACE
