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

#include "qtriangle3d.h"
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qnumeric.h>

QT_BEGIN_NAMESPACE

/*!
    \class QTriangle3D
    \brief The QTriangle3D class represents a triangle as three points in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::math

    A triangle is defined by 3 points in 3D space.  Since any 3 points define
    a plane, the triangle can be thought of as defining a plane, and forming a
    geometric region in that plane.
    
    If you need a simple plane, with no particular geometry, then
    QPlane3D is a more compact and mathematically sufficient class.
    
    The three points are labelled p(), q() and r() for consistency with
    textbook treatments.  It is recommended that the points be supplied
    in counter-clockwise order for correct orientation of the
    triangle's plane().

    \sa QPlane3D
*/

/*!
    \fn QTriangle3D::QTriangle3D()

    Constructs a default triangle which lies in the x-z plane,
    with the three vertices (0, 0, 0), (1, 0, 0), and (0, 1, 0).
*/

/*!
    \fn QTriangle3D::QTriangle3D(const QVector3D &p, const QVector3D &q, const QVector3D &r)

    Constructs a triangle with the supplied \a p, \a q and \a r vertices.
*/

/*!
    \fn QVector3D QTriangle3D::p() const

    Returns the value of the P vertex on the triangle.

    \sa q(), r(), setP()
*/

/*!
    \fn void QTriangle3D::setP(const QVector3D &point)

    Sets the value of the P vertex on the triangle to \a point.

    \sa setQ(), setR(), p()
*/

/*!
    \fn QVector3D QTriangle3D::q() const

    Returns the value of the Q vertex on the triangle.

    \sa p(), r(), setQ()
*/

/*!
    \fn void QTriangle3D::setQ(const QVector3D &point)

    Sets the value of the Q vertex on the triangle \a point.

    \sa setP(), setR(), q()
*/


/*!
    \fn QVector3D QTriangle3D::r() const

    Returns the value of the R vertex on the triangle.

    \sa p(), q(), setR()
*/

/*!
    \fn void QTriangle3D::setR(const QVector3D &point)

    Sets the value of the R vertex on the triangle \a point.

    \sa setP(), setQ(), r()
*/

/*!
    \fn QPlane3D QTriangle3D::plane() const

    Returns the plane in which the triangle lies.

    \sa QPlane3D
*/

/*!
    \fn QVector3D QTriangle3D::center() const

    Returns the center of the triangle, which is the geometric average of the
    three vertices.
*/

/*!
    \fn QVector3D QTriangle3D::faceNormal() const

    Returns the vector normal to this triangle, computed from the
    cross-product of P-Q and Q-R.  The result is not normalized.
*/

/*!
    Returns true if this triangle contains \a point; false otherwise.
    To contain the \a point means that:
    \list
    \o the point lies on the same plane as the triangle, and
    \o the point
        \list
        \o lies either wholly within the triangle, or
        \o lies on one of the sides, or
        \o coincides with one of the 3 vertices
        \endlist
    \endlist
    No check is made for a degenerate triangle.

    \sa intersects()
*/
bool QTriangle3D::contains(const QVector3D &point) const
{
    // Check if the point is on the triangle's plane first.
    QVector3D normal = QVector3D::crossProduct(m_q - m_p, m_r - m_q);
    if (!qFuzzyIsNull(QVector3D::dotProduct(normal, m_p - point)))
        return false;

    // Determine if the point is on the same side of P-Q as R.
    QVector3D pq = m_q - m_p;
    if (QVector3D::dotProduct
            (QVector3D::crossProduct(pq, point - m_p),
             QVector3D::crossProduct(pq, m_r - m_p)) < 0.0f)
        return false;

    // Determine if the point is on the same side of Q-R as P.
    QVector3D qr = m_r - m_q;
    if (QVector3D::dotProduct
            (QVector3D::crossProduct(qr, point - m_q),
             QVector3D::crossProduct(qr, m_p - m_q)) < 0.0f)
        return false;

    // Determine if the point is on the same side of R-P as Q.
    QVector3D rp = m_p - m_r;
    if (QVector3D::dotProduct
            (QVector3D::crossProduct(rp, point - m_r),
             QVector3D::crossProduct(rp, m_q - m_r)) < 0.0f)
        return false;

    // We have an intersection within the bounds of the triangle.
    return true;
}

/*!
    Returns true if the \a ray intersects this triangle; false otherwise.

    This function will return false if the triangle is degenerate.

    \sa contains(), intersection()
*/
bool QTriangle3D::intersects(const QRay3D &ray) const
{
    if (qFuzzyCompare(QVector3D::dotProduct(m_q - m_p, ray.direction()), qreal(1.0f)))
        return false;
    qreal t = plane().intersection(ray);
    if (qIsNaN(t))
        return false;
    return contains(ray.point(t));
}

/*!
    Returns the t value at which \a ray intersects this triangle, or
    not-a-number if there is no intersection.

    When the \a ray intersects this triangle, the return value is a
    parametric value that can be passed to QRay3D::point() to determine
    the actual intersection point, as shown in the following example:

    \code
    qreal t = triangle.intersection(ray);
    QVector3D pt;
    if (qIsNaN(t)) {
        qWarning("no intersection occurred");
    else
        pt = ray.point(t);
    \endcode

    \sa intersects(), contains(), QRay3D::point()
 */
qreal QTriangle3D::intersection(const QRay3D &ray) const
{
    if (qFuzzyCompare(QVector3D::dotProduct(m_q - m_p, ray.direction()), qreal(1.0f)))
        return qSNaN();
    qreal t = plane().intersection(ray);
    if (qIsNaN(t) || contains(ray.point(t)))
        return t;
    return qSNaN();
}

/*!
    Transforms the points of this triangle according to \a matrix.

    \sa transformed()
*/
void QTriangle3D::transform(const QMatrix4x4 &matrix)
{
    m_p = matrix * m_p;
    m_q = matrix * m_q;
    m_r = matrix * m_r;
}

/*!
    Returns a new triangle that results from transforming this
    one using \a matrix.

    \sa transform()
*/
QTriangle3D QTriangle3D::transformed(const QMatrix4x4 &matrix) const
{
    return QTriangle3D(matrix * m_p, matrix * m_q, matrix * m_r);
}

/*!
    \fn bool qFuzzyCompare(const QTriangle3D &triangle1, const QTriangle3D &triangle2)
    \relates QTriangle3D

    Returns true if \a triangle1 and \a triangle2 are almost equal;
    false otherwise.
*/

QT_END_NAMESPACE
