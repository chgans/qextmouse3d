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

#include "qgraphicsrotation3d.h"

/*!
    \class QGraphicsRotation3D
    \brief The QGraphicsRotation3D class supports arbitrary rotation around an axis in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview
    \ingroup qt3d::qml3d

    \section1 Properties

    QGraphicsRotation3D is derived directly from the QGraphicsRotation class,
    and has support for the \c origin, \c angle, and \c axis properties
    contained in the parent class.  The difference between the two
    classes is that QGraphicsRotation performs a \i projected rotation
    around the \c axis, whereas QGraphicsRotation3D performs a true
    3D rotation without projecting back to 2D.

    \section1 Usage in QML/3D

    Frequently a user will create and item in the 3d world and immediately
    wish to apply a rotation to that item before it is displayed, or,
    optionally, perform an animation on that rotation parameter based on
    user inputs, or other events.  Such an rotation can easily be defined
    in QML/3D using the following code:

    \code
    Item3d {
        id: helicoptor
        mesh:  {source: "bellUH1.3ds"}
        effect: Effect {}
        cullFaces: "CullBackFaces"

        transform: [
                        Rotation3D {
                                id: rotate1
                                angle: 5
                                axis: Qt.vector3d(1, 0, 0)
                        },
                        Rotation3D {
                                id: rotate2
                                angle: 5
                                axis: Qt.vector3d(0, 1, 0)
                        },
                        Rotation3D {
                                id: rotate3
                                angle: 45
                                axis: Qt.vector3d(0, 0, 1)
                        }
                ]

       SequentialAnimation {
            NumberAnimation {target: rotate1; property: "angle"; to : 360.0; duration: 3000; easing:"easeOutQuad" }
        }
    }
    \endcode

    Notice here that we create a list of rotations for the \c transform
    property of the container item.  By doing this we allow rotations
    around each of the axes individually in a manner which is conducive
    to animation and interaction.

    Each of the rotations has an \c axis property which is a QVector3D.
    This vector contains a value for each of the three components
    corresponding to x, y, and z.  In the above example, we first
    rotate by 5 degrees about the x axis, then 5 degrees about the y
    axis, and finally by 45 degrees about the z axis.

    By giving each rotation a unique \c id users can then refer to these
    rotations in the QML source in order to perform rotational animations.

    \sa QGraphicsTranslation3D, QGraphicsScale3D
*/

QT_BEGIN_NAMESPACE

/*!
    Create a 3D rotation transformation and attach it to \a parent.
*/
QGraphicsRotation3D::QGraphicsRotation3D(QObject *parent)
    : QGraphicsRotation(parent)
{
}

/*!
    Destroy this 3D rotation transformation.
*/
QGraphicsRotation3D::~QGraphicsRotation3D()
{
}

/*!
    \internal
*/
void QGraphicsRotation3D::applyTo(QMatrix4x4 *matrix) const
{
    qreal angle = this->angle();
    QVector3D axis = this->axis();
    QVector3D origin = this->origin();

    if (angle == 0. || axis.isNull())
        return;

    matrix->translate(origin);
    matrix->rotate(angle, axis.x(), axis.y(), axis.z());
    matrix->translate(-origin);
}

QT_END_NAMESPACE
