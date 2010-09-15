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

#include "qray3d.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QRay3D
    \brief The QRay3D class defines a directional line in 3D space extending through an origin point.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::math

    A ray is defined by the origin() point and the direction() vector.
    Rays are infinite in length, extending out from origin() in
    both directions.  If the direction() is zero length, then the
    behavior of the class is undefined.

    A ray can be thought of as a one-dimensional co-ordinate system.
    If the co-ordinate is \bold t then the origin() point is at
    \bold t = 0, the point origin() + direction() is at \bold t = 1,
    and the point origin() - direction() is at \bold t = -1.
    The point() method can be used to obtain the position of a point
    within this one-dimensional co-ordinate system.  The fromPoint()
    method can be used to convert a point into a value in this
    one-dimensional co-ordinate system.
*/

/*!
    \fn QRay3D::QRay3D()

    Construct a default ray with an origin() of (0, 0, 0) and a
    direction() of (1, 0, 0).
*/

/*!
    \fn QRay3D::QRay3D(const QVector3D &origin, const QVector3D &direction)

    Construct a ray given its defining \a origin and \a direction.  The
    \a direction does not need to be normalized.

    To construct a ray that passes through two points, use the following:

    \code
    QRay3D thruAB(pointA, pointB - pointA);
    \endcode
*/

/*!
    \fn QVector3D QRay3D::origin() const

    Returns the origin of this ray.  The default value is (0, 0, 0).

    \sa setOrigin(), direction()
*/

/*!
    \fn void QRay3D::setOrigin(const QVector3D &value)

    Sets the origin point of this ray to \a value.

    \sa origin(), setDirection()
 */

/*!
    \fn QVector3D QRay3D::direction() const

    Returns the direction vector of this ray.  The default value is (1, 0, 0).

    \sa setDirection(), origin()
*/

/*!
    \fn void QRay3D::setDirection(const QVector3D &direction)

    Sets the direction vector of this ray to \a direction.

    \sa direction(), setOrigin()
*/

/*!
    Returns true if \a point lies on this ray; false otherwise.
*/
bool QRay3D::contains(const QVector3D &point) const
{
    QVector3D ppVec(point - m_origin);
    if (ppVec.isNull()) // point coincides with origin
        return true;
    qreal dot = QVector3D::dotProduct(ppVec, m_direction);
    if (qFuzzyIsNull(dot))
        return false;
    return qFuzzyCompare(dot*dot, ppVec.lengthSquared() * m_direction.lengthSquared());
}

/*!
    Returns true if \a ray lies on this ray; false otherwise.  If true,
    this implies that the two rays are the actually the same, but with
    different origin() points or an inverted direction().
*/
bool QRay3D::contains(const QRay3D &ray) const
{
    qreal dot = QVector3D::dotProduct(m_direction, ray.direction());
    if (!qFuzzyCompare(dot*dot, m_direction.lengthSquared() * ray.direction().lengthSquared()))
        return false;
    return contains(ray.origin());
}

/*!
    \fn QVector3D QRay3D::point(qreal t) const

    Returns the point on the ray defined by moving \a t units
    along the ray in the direction of the direction() vector.
    Note that \a t may be negative in which case the point returned
    will lie behind the origin() point with respect to the
    direction() vector.

    The units for \a t are defined by direction().  The return value
    is precisely origin() + t * direction().

    \sa fromPoint(), distance()
*/

/*!
    Returns the number of direction() units along the ray from origin()
    to \a point.  Essentially, this function computes the value t, where
    \a point = origin() + t * direction().  If \a point is not on the ray,
    then the closest point that is on the ray will be used instead.

    If the return value is positive, then \a point lies in front of
    the origin() with respect to the direction() vector.  If the return
    value is negative, then \a point lies behind the origin() with
    respect to the direction() vector.

    \sa point(), project()
*/
qreal QRay3D::fromPoint(const QVector3D &point) const
{
    return QVector3D::dotProduct(point - m_origin, m_direction) /
                m_direction.lengthSquared();
}

/*!
    Returns the projection of \a vector onto this ray.  In the
    following diagram, the dotted line is the ray, and V is the
    \a vector.  The return value will be the vector V':

    \image qray3d-project.png

    \sa fromPoint()
*/
QVector3D QRay3D::project(const QVector3D &vector) const
{
    QVector3D norm = m_direction.normalized();
    return QVector3D::dotProduct(vector, norm) * norm;
}

/*!
    Returns the minimum distance of \a point from this ray, or equivalently
    the length of a line perpendicular to this ray which passes through
    \a point.  If \a point is on the ray, then this function will return zero.

    \sa point()
*/
qreal QRay3D::distance(const QVector3D &point) const
{
    qreal t = fromPoint(point);
    return (point - (m_origin + t * m_direction)).length();
}

/*!
    \fn bool QRay3D::operator==(const QRay3D &other)

    Returns true if this ray is the same as \a other; false otherwise.

    \sa operator!=()
*/

/*!
    \fn bool QRay3D::operator!=(const QRay3D &other)

    Returns true if this ray is not the same as \a other; false otherwise.

    \sa operator==()
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QRay3D &ray)
{
    dbg.nospace() << "QRay3D(origin("
        << ray.origin().x() << ", " << ray.origin().y() << ", "
        << ray.origin().z() << ") - direction("
        << ray.direction().x() << ", " << ray.direction().y() << ", "
        << ray.direction().z() << ')';
    return dbg.space();
}

#endif

QT_END_NAMESPACE
