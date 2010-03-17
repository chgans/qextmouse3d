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

#include "rotation3d.h"

/*!
    \class Rotation3D
    \brief The Rotation3D class contains parameters and functions required to support 3d rotation of
    items in the QML/3d environment.
    \since 4.6.q
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Properties

    The Rotation3d class is derived directly from the QGraphicsRotation class, and has
    QML support for the \c origin, \c angle, and \c axis properties contained in this
    parent class.

    \section1 Usage in QML/3d

    Frequently a user will create and item in the 3d world and immediately wish to apply
    a rotation to that item before it is displayed, or, optionally, perform an animation
    on that rotation parameter based on user inputs, or other events.

    Such an rotation can easily be defined in QML/3d using the following code:

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

    Notice here that we create a list of reotations for the \c transform property of the
    container item.  By doing this we allow rotations around each of the axes individually in
    a manner which is conducive to animation and interaction.

    Each of the rotations has an \c axis property which is a QtVector3d.  This vector contains
    a value for each of the three components corresponding to x, y, and z.  A non-zero value in
    one or more of these fields means that the \c angle or rotation is to be applied to

    By giving each rotation a unique \c id users can then refer to these rotations in the QML
    source in order to perform rotational animations.
*/

QT_BEGIN_NAMESPACE

//QML_DEFINE_TYPE(Qt,4,6,Rotation3D,Rotation3D)

/*!
  Object creation and assigment of \a parent object.
*/
Rotation3D::Rotation3D(QObject *parent)
    : QGraphicsRotation(parent)
{
}

/*!
  Currently there is no specific destruction required for the class.
*/
Rotation3D::~Rotation3D()
{
}

/*!
  Application of the rotation to a given 4x4 \a matrix is performed by this function.  The rotation
  is performed by translating the contents of the matrix to the origin, performing the individual
  axial rotations, then translating back to the original position.
*/
void Rotation3D::applyTo(QMatrix4x4 *matrix) const
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
