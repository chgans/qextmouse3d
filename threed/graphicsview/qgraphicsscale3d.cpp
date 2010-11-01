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

#include "qgraphicsscale3d.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGraphicsScale3D
    \brief The QGraphicsScale3D class supports scaling of items in 3D.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::graphicsview

    \sa QGraphicsRotation3D, QGraphicsTranslation3D
*/

/*!
    \qmlclass Scale3D QGraphicsScale3D
    \brief The Scale3D item supports scaling of items in 3D.
    \since 4.8
    \ingroup qt3d::qml3d

    3D items in QML can have a simple scale applied directly as follows:

    \code
    Item3D {
        mesh: Mesh { source: "chair.3ds" }
        scale: 0.5
    }
    \endcode

    An alternative is to use Scale3D to apply a transform directly
    to an item as part of a sequence of transformations:

    \code
    Item3D {
        mesh: Mesh { source: "chair.3ds" }
        transform: [
            Translation3D { translate: Qt.vector3D(5, 0, 0) },
            Scale3D { uniformScale: 0.5 }
        ]
    }
    \endcode

    This allows the application writer to control exactly when the
    scale occurs relative to other transformations.  In the example
    above, the item is first translated by 5 units along the x-axis,
    and then the co-ordinates are scaled by half.  This is distinct
    from the following example which scales the object to half its
    original size and then translates it by 5 units along the x-axis:

    \code
    Item3D {
        mesh: Mesh { source: "chair.3ds" }
        transform: [
            Scale3D { uniformScale: 0.5 },
            Translation3D { translate: Qt.vector3D(5, 0, 0) }
        ]
    }
    \endcode

    The scale property on the item itself is applied before
    any of the transforms.  So the previous example is equivalent to:

    \code
    Item3D {
        mesh: Mesh { source: "chair.3ds" }
        scale: 0.5
        transform: [
            Translation3D { translate: Qt.vector3D(5, 0, 0) }
        ]
    }
    \endcode

    \sa Rotation3D, Translation3D
*/

class QGraphicsScale3DPrivate
{
public:
    QGraphicsScale3DPrivate() : scale(1, 1, 1) {}

    QVector3D origin;
    QVector3D scale;
};

/*!
    Construct a 3D scale transform and attach it to \a parent.
*/
QGraphicsScale3D::QGraphicsScale3D(QObject *parent)
    : QGraphicsTransform3D(parent), d_ptr(new QGraphicsScale3DPrivate)
{
}

/*!
    Destroy this 3D scale transform.
*/
QGraphicsScale3D::~QGraphicsScale3D()
{
}

/*!
    \property QGraphicsScale3D::origin
    \brief the origin about which to scale.

    The default value for this property is (0, 0, 0).
*/

/*!
    \qmlproperty vector3D Scale3D::origin

    The origin about which to scale.  The default value for this
    property is (0, 0, 0).
*/

QVector3D QGraphicsScale3D::origin() const
{
    Q_D(const QGraphicsScale3D);
    return d->origin;
}

void QGraphicsScale3D::setOrigin(const QVector3D &value)
{
    Q_D(QGraphicsScale3D);
    if (d->origin != value) {
        d->origin = value;
        emit transformChanged();
        emit originChanged();
    }
}

/*!
    \property QGraphicsScale3D::scale
    \brief the amount with which to scale each component.

    The default value for this property is QVector3D(1, 1, 1).

    \sa uniformScale()
*/

/*!
    \qmlproperty vector3D Scale3D::scale

    The amount with which to scale each component.  The default value for
    this property is (1, 1, 1).

    The \l uniformScale property can be specified as a single floating-point
    value to set all axes to the same value.  In other words, the following
    two transformations are equivalent:

    \code
    Scale3D { uniformScale: 2 }
    Scale3D { scale: Qt.vector3d(2, 2, 2) }
    \endcode

    \sa uniformScale
*/

QVector3D QGraphicsScale3D::scale() const
{
    Q_D(const QGraphicsScale3D);
    return d->scale;
}

void QGraphicsScale3D::setScale(const QVector3D &value)
{
    Q_D(QGraphicsScale3D);
    if (d->scale != value) {
        d->scale = value;
        emit transformChanged();
        emit scaleChanged();
    }
}

/*!
    \property QGraphicsScale3D::uniformScale
    \brief the amount with which to scale each component uniformly.

    The default value for this property is 1.  It returns the
    average of the x, y, and z components of scale() when read,
    and sets all of x, y, and z to the same value when written.

    \sa scale()
*/

/*!
    \qmlproperty real Scale3D::uniformScale

    The amount with which to scale each component uniformly.  The default
    value for this property is 1.

    This property returns the average of the x, y, and z components of
    \l scale when read, and sets all of x, y, and z to the same value
    when written.  In other words, the following two transformations
    are equivalent:

    \code
    Scale3D { uniformScale: 2 }
    Scale3D { scale: Qt.vector3d(2, 2, 2) }
    \endcode

    \sa scale
*/

qreal QGraphicsScale3D::uniformScale() const
{
    Q_D(const QGraphicsScale3D);
    return (d->scale.x() + d->scale.y() + d->scale.z()) / 3.0f;
}

void QGraphicsScale3D::setUniformScale(qreal value)
{
    Q_D(QGraphicsScale3D);
    QVector3D scale(value, value, value);
    if (d->scale != scale) {
        d->scale = scale;
        emit transformChanged();
        emit scaleChanged();
    }
}

/*!
    \internal
*/
void QGraphicsScale3D::applyTo(QMatrix4x4 *matrix) const
{
    Q_D(const QGraphicsScale3D);
    matrix->translate(d->origin);
    matrix->scale(d->scale);
    matrix->translate(-d->origin);
}

/*!
    \fn void QGraphicsScale3D::originChanged()

    Signal that is emitted when origin() changes.
*/

/*!
    \fn void QGraphicsScale3D::scaleChanged()

    Signal that is emitted when scale() changes.
*/

QT_END_NAMESPACE
