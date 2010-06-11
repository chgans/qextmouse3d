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

#include "qsphericalanimation.h"
#include <QtCore/qmath.h>
#include <QtCore/qdebug.h>
#include <QtGui/qquaternion.h>

QT_BEGIN_NAMESPACE

#ifndef QT_NO_ANIMATION

/*!
    \class QSphericalAnimation
    \brief The QSphericalAnimation class animates objects rotating around a point on the surface of a sphere.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::enablers

    The animation occurs along a circular path on surface of a sphere
    centered at origin().  The start, end, and key values must be
    QVector3D instances indicating the points along the path.

    This class can also animate along an elliptical path by adjusting
    the points to define a curve with a longer axis in one direction.

    Due to the quaternion-based algorithm used, the key values must
    be separated by less than 180 degrees.  The algorithm will always
    choose the shortest path between two key values, which can never
    be greater than or equal to 180 degrees.

    For a full circle (or ellipse), it is recommended that 5 key values
    be provided for the 0, 90, 180, 270, and 360 degree positions
    (where the 360 degree position is the same as 0).  The following
    example demonstrates this for a circle of radius 3 in the
    z = 0 plane around the origin:

    \code
    qreal radius = 3.0f;
    animPlanet1 = new QSphericalAnimation(this, "planet1Pos", this);
    animPlanet1->setStartValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet1->setKeyValueAt(0.25f, qVariantFromValue(QVector3D(0.0f, radius, 0.0f)));
    animPlanet1->setKeyValueAt(0.5f, qVariantFromValue(QVector3D(-radius, 0.0f, 0.0f)));
    animPlanet1->setKeyValueAt(0.75f, qVariantFromValue(QVector3D(0.0f, -radius, 0.0f)));
    animPlanet1->setEndValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet1->setDuration(3000);
    animPlanet1->setLoopCount(-1);
    animPlanet1->start();
    \endcode
*/

class QSphericalAnimationPrivate
{
public:
    QVector3D origin;
};

/*!
    Constructs a spherical animation object and attaches it to \a parent.
*/
QSphericalAnimation::QSphericalAnimation(QObject *parent)
    : QPropertyAnimation(parent)
{
    d = new QSphericalAnimationPrivate();
}

/*!
    Constructs a spherical animation object and attaches it to \a parent.
    The animation will update \a propertyName on \a target as the
    animation proceeds.
*/
QSphericalAnimation::QSphericalAnimation(QObject *target, const QByteArray &propertyName, QObject *parent)
    : QPropertyAnimation(target, propertyName, parent)
{
    d = new QSphericalAnimationPrivate();
}

/*!
    Destroys this spherical animation object.
*/
QSphericalAnimation::~QSphericalAnimation()
{
    delete d;
}

/*!
    Returns the origin of the animation.  This is the point around
    which rotation occurs.

    \sa setOrigin()
*/
QVector3D QSphericalAnimation::origin() const
{
    return d->origin;
}

/*!
    Sets the origin of the animation to \a value.

    \sa origin()
*/
void QSphericalAnimation::setOrigin(const QVector3D& value)
{
    d->origin = value;
}

/*!
    \internal
*/
QVariant QSphericalAnimation::interpolated
    (const QVariant &from, const QVariant &to, qreal progress) const
{
    QVector3D f = qVariantValue<QVector3D>(from);
    QVector3D t = qVariantValue<QVector3D>(to);

    if (qFuzzyCompare(d->origin, f) || qFuzzyCompare(d->origin, t)) {
        // Start or end is the origin, so interpolate along a linear path.
        return qVariantFromValue(f + (t - f) * progress);
    }

    // Avoid interpolation rounding issues at the end points.
    if (progress == 0.0f)
        return from;
    else if (progress == 1.0f)
        return to;

    // Compute the vectors from the origin to the f and t points.
    QVector3D fv = f - d->origin;
    QVector3D tv = t - d->origin;

    // Determine the axis of rotation.  If null, then f and t
    // are along the same vector - we interpolate linearly for that.
    QVector3D axis = QVector3D::crossProduct(fv, tv);
    if (axis.isNull())
        return qVariantFromValue(f + (t - f) * progress);

    // Normalize the f and t vectors.
    QVector3D fvu = fv.normalized();
    QVector3D tvu = tv.normalized();

    // Determine the angle between the two vectors and interpolate it.
    qreal angle = qAcos(QVector3D::dotProduct(fvu, tvu)) * progress;
    angle = angle * 180.0f / M_PI;

    // Determine the interpolated length of the final vector.
    qreal fvl = fv.length();
    qreal tvl = tv.length();
    qreal length = fvl + (tvl - fvl) * progress;

    // Rotate fvu around the axis, multiply by the interpolated
    // length and then add back to the origin to get the final point.
    QQuaternion q = QQuaternion::fromAxisAndAngle(axis, angle);
    return qVariantFromValue
        (d->origin + q.rotatedVector(fvu) * length);
}

#endif //QT_NO_ANIMATION

QT_END_NAMESPACE
