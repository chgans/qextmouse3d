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

#include "scale3d.h"

QT_BEGIN_NAMESPACE

/*!
    \class Scale3D
    \brief The Scale3D class contains parameters and functions required to support 3d scaling of items in the QML/3D environment.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \sa Rotation3D, Translation3D
*/

/*!
    Construct a 3D scale transform and attach it to \a parent.
*/
Scale3D::Scale3D(QObject *parent)
    : QGraphicsTransform(parent)
    , m_scale(1.0f, 1.0f, 1.0f)
{
}

/*!
    Destroy this 3D scale transform.
*/
Scale3D::~Scale3D()
{
}

/*!
    \property Scale3D::origin
    \brief the origin about which to scale.

    The default value for this property is (0, 0, 0).
*/

void Scale3D::setOrigin(const QVector3D &value)
{
    if (m_origin != value) {
        m_origin = value;
        update();
        emit originChanged();
    }
}

/*!
    \property Scale3D::xScale
    \brief the amount with which to scale to x component.

    The default value for this property is 1.
*/

void Scale3D::setXScale(qreal value)
{
    if (m_scale.x() != value) {
        m_scale.setX(value);
        update();
        emit scaleChanged();
    }
}

/*!
    \property Scale3D::yScale
    \brief the amount with which to scale to y component.

    The default value for this property is 1.
*/

void Scale3D::setYScale(qreal value)
{
    if (m_scale.y() != value) {
        m_scale.setY(value);
        update();
        emit scaleChanged();
    }
}

/*!
    \property Scale3D::zScale
    \brief the amount with which to scale to z component.

    The default value for this property is 1.
*/

void Scale3D::setZScale(qreal value)
{
    if (m_scale.z() != value) {
        m_scale.setZ(value);
        update();
        emit scaleChanged();
    }
}

/*!
    \property Scale3D::scaleVector
    \brief the amount with which to scale each component.

    The default value for this property is (1, 1, 1).
*/

void Scale3D::setScaleVector(const QVector3D &value)
{
    if (m_scale != value) {
        m_scale = value;
        update();
        emit scaleChanged();
    }
}

/*!
    \property Scale3D::scale
    \brief the amount with which to scale.

    When read, this property returns the average of xScale,
    yScale, and zScale.  When written, this property will
    set the x, y, and z scales to the same value.

    The default value for this property is 1.
*/

qreal Scale3D::scale() const
{
    return (m_scale.x() + m_scale.y() + m_scale.z()) / 3.0f;
}

void Scale3D::setScale(qreal value)
{
    if (m_scale.x() != value || m_scale.y() != value || m_scale.z() != value) {
        m_scale = QVector3D(value, value, value);
        update();
        emit scaleChanged();
    }
}

/*!
    \internal
*/
void Scale3D::applyTo(QMatrix4x4 *matrix) const
{
    matrix->translate(-m_origin);
    matrix->scale(m_scale);
    matrix->translate(m_origin);
}

/*!
    \fn void Scale3D::originChanged()

    Signal that is emitted when origin() changes.
*/

/*!
    \fn void Scale3D::scaleChanged()

    Signal that is emitted when scaleVector() changes.
*/

QT_END_NAMESPACE
