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

#include "qbox3d.h"
#include "qplane3d.h"
#include <QtGui/qmatrix4x4.h>
#include <QtCore/qlist.h>

QT_BEGIN_NAMESPACE

/*!
    \class QBox3D
    \brief The QBox3D class represents an axis-aligned box in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::math

    QBox3D can be used to represent the bounding box of objects in a 3D
    scene so that they can be easily culled if they are out of view.

    The sides of the box are always aligned with the x, y, and z axes of
    the world co-ordinate system.  Transforming a box with transformed()
    will result in the smallest axis-aligned bounding box that contains
    the transformed box.

    Boxes may be null, finite, or infinite.  A null box does not occupy
    any space and does not intersect with any other box.  A finite
    box consists of a minimum() and maximum() extent in 3D space.
    An infinite box encompasses all points in 3D space.

    The extents of a finite box are also included within the box.
    A box with minimum() and maximum() set to the same value
    contains a single point.
*/

/*!
    \fn QBox3D::QBox3D()

    Constructs a null box in 3D space.

    \sa isNull()
*/

/*!
    \fn QBox3D::QBox3D(const QVector3D& corner1, const QVector3D& corner2)

    Constructs a finite box in 3D space from \a corner1 to \a corner2.
    The minimum() and maximum() co-ordinates of the new box are set
    to the minimum and maximum x, y, and z values from \a corner1 and
    \a corner2.  The \a corner1 and \a corner2 values can be any two
    opposite corners that define the box.

    \sa isFinite(), minimum(), maximum()
*/

/*!
    \fn bool QBox3D::isNull() const

    Returns true if this box is null; false otherwise.

    \sa isFinite(), isInfinite(), setToNull()
*/

/*!
    \fn bool QBox3D::isFinite() const

    Returns true if this box is finite in size; false otherwise.

    \sa isNull(), isInfinite(), setExtents()
*/

/*!
    \fn bool QBox3D::isInfinite() const

    Returns true if this box is infinite in size; false otherwise.

    \sa isNull(), isFinite(), setToInfinite()
*/

/*!
    \fn QVector3D QBox3D::minimum() const

    Returns the minimum corner of this box.

    \sa maximum(), setExtents()
*/

/*!
    \fn QVector3D QBox3D::maximum() const

    Returns the maximum corner of this box.

    \sa minimum(), setExtents()
*/

/*!
    \fn void QBox3D::setExtents(const QVector3D& corner1, const QVector3D& corner2)

    Sets the extents of this box to a finite region from \a corner1 to
    \a corner2.  The minimum() and maximum() co-ordinates of the box are
    set to the minimum and maximum x, y, and z values from \a corner1 and
    \a corner2.  The \a corner1 and \a corner2 values can be any two
    opposite corners that define the box.

    \sa minimum(), maximum()
*/

/*!
    \fn void QBox3D::setToNull()

    Sets this box to null.

    \sa isNull()
*/

/*!
    \fn void QBox3D::setToInfinite()

    Sets this box to be infinite in size.

    \sa isInfinite()
*/

/*!
    \fn QVector3D QBox3D::size() const

    Returns the finite size of this box.  If this box is null or
    infinite, the returned value will be zero.

    \sa center(), isNull(), isInfinite()
*/

/*!
    \fn QVector3D QBox3D::center() const

    Returns the finite center of this box.  If this box is null
    or infinite, the returned value will be zero.

    \sa size(), isNull(), isInfinite()
*/

/*!
    \fn bool QBox3D::contains(const QVector3D& point) const

    Returns true if this box contains \a point; false otherwise.
    Null boxes do not contain any points and infinite boxes contain
    all points.

    Containment is not a strict test: the point is contained if it
    lies on one of the faces of the box.

    \sa intersects()
*/

/*!
    \fn bool QBox3D::contains(const QBox3D& box) const

    Returns true if this box completely contains \a box.  If this box
    is null, then it will not contain \a box.  If this box is infinite,
    and \a box is not null, then \a box will be contained within this box.
    If \a box is infinite, then this box must also be infinite to contain it.

    \sa intersects()
*/

/*!
    Returns true if \a box intersects this box; false otherwise.

    \sa intersect(), intersected(), contains()
*/
bool QBox3D::intersects(const QBox3D& box) const
{
    if (boxtype == Null)
        return false;
    else if (boxtype == Infinite)
        return box.boxtype != Null;
    else if (box.boxtype == Null)
        return false;
    else if (box.boxtype == Infinite)
        return true;

    if (maxcorner.x() < box.mincorner.x())
        return false;
    if (mincorner.x() > box.maxcorner.x())
        return false;

    if (maxcorner.y() < box.mincorner.y())
        return false;
    if (mincorner.y() > box.maxcorner.y())
        return false;

    if (maxcorner.z() < box.mincorner.z())
        return false;
    if (mincorner.z() > box.maxcorner.z())
        return false;

    return true;
}

/*!
    Returns true if \a ray intersects this box; false otherwise.

    \sa intersection()
*/
bool QBox3D::intersects(const QRay3D &ray) const
{
    return !qIsNaN(intersection(ray));
}

/*!
  \internal
  Use a kind of cheap hill-climbing approach to find the point intersection,
  where the heuristic is the value of t, the scalar representing the distance
  from the line's origin point to the intersection under consideration.  If
  the t for the intersection being considered is less than t's for other
  intersections then it might be a solution, in which case do the calculation
  to find the point Q = P + tV on the line.  Once the point is found, test it
  to see if it really is a solution (lies in the box) before updating the
  heuristic.
 */
inline static void trackIntersection(qreal t, qreal *closest_t,
                         const QBox3D &box, const QRay3D &line)
{
    if ((t > 0.0f && t < *closest_t) || qIsNull(*closest_t)) {
        // only consider non-negative values of t, and ones better than best
        // value found so far
        QVector3D p = line.point(t);
        if (box.contains(p))
            *closest_t = t;
    }
}

/*!
    Returns the t value at which \a ray intersects this box, or
    not-a-number if there is no intersection.

    When the \a ray intersects this box, the return value is a
    parametric value that can be passed to QRay3D::point() to determine
    the actual intersection point, as shown in the following example:

    \code
    qreal t = box.intersection(ray);
    QVector3D pt;
    if (qIsNaN(t)) {
        qWarning("no intersection occurred");
    else
        pt = ray.point(t);
    \endcode

    The \a ray might intersect at two points - as the line passes through
    the box - one on the near side, one on the far side; where near and far
    are relative to the origin point of the line.  This function only
    returns the near intersection point.

    Only positive values on the line are considered, that is if
    \c{t == ray.point(intersectionPoint)} and t is positive.

    This means that if the origin point of the line is inside the box, there
    is only one solution, not two.  To get the other solution, simply change
    the sign of the ray's direction vector.

    When the ray does not intersect the box, or the box is infinite,
    then the return value is not-a-number.

    \sa intersects(), QRay3D::point()
*/
qreal QBox3D::intersection(const QRay3D &ray) const
{
    if (boxtype == Null)
        return qSNaN();
    if (boxtype == Infinite)
        return qSNaN();

    QVector3D org = ray.origin();
    Partition xpos, ypos, zpos;
    partition(org, &xpos, &ypos, &zpos);
    // if the lines origin lies on one of the faces, return it as the intersection
    if ((xpos | ypos | zpos) == (equalMin | equalMax))
        return 0.0f;

    // Could use the line/plane intersection functions, with 6 box planes defined by
    // 3 normals at the min and 3 at the max.  But since the planes are axis-aligned
    // there is a cheap optimization.  If the line P + tV intersects a AA-plane with
    // x = c at intersection point X, then P.x + tV.x = X.x = c; and therefore
    // t = (c - P.x) / V.x.  Here the value t is the scalar distance along V from P and
    // so it measures the value of the solution - see trackIntersection() above.
    qreal closest_t = 0.0f;
    QVector3D ln = ray.direction();
    for (const QVector3D *p = &maxcorner; p; p = (p == &maxcorner) ? &mincorner : 0)
    {
        if (!qIsNull(ln.x()))
            trackIntersection(p->x() - org.x() / ln.x(), &closest_t, *this, ray);
        if (!qIsNull(ln.y()))
            trackIntersection(p->y() - org.y() / ln.y(), &closest_t, *this, ray);
        if (!qIsNull(ln.z()))
            trackIntersection(p->z() - org.z() / ln.z(), &closest_t, *this, ray);
    }
    if (!qIsNull(closest_t))
        return closest_t;
    else
        return qSNaN();
}

/*!
    Intersects this box with \a box.

    \sa intersected(), intersects(), unite()
*/
void QBox3D::intersect(const QBox3D& box)
{
    // Handle the simple cases first.
    if (boxtype == Null) {
        // Null intersected with anything is null.
        return;
    } else if (boxtype == Infinite) {
        // Infinity intersected with a box is that box.
        *this = box;
        return;
    } else if (box.boxtype == Null) {
        // Anything intersected with null is null.
        setToNull();
        return;
    } else if (box.boxtype == Infinite) {
        // Box intersected with infinity is the box.
        return;
    }

    // Intersect two finite boxes.
    QVector3D min1 = mincorner;
    QVector3D max1 = maxcorner;
    QVector3D min2 = box.mincorner;
    QVector3D max2 = box.maxcorner;
    if (min2.x() > min1.x())
        min1.setX(min2.x());
    if (min2.y() > min1.y())
        min1.setY(min2.y());
    if (min2.z() > min1.z())
        min1.setZ(min2.z());
    if (max2.x() < max1.x())
        max1.setX(max2.x());
    if (max2.y() < max1.y())
        max1.setY(max2.y());
    if (max2.z() < max1.z())
        max1.setZ(max2.z());
    if (min1.x() > max1.x() || min1.y() > max1.y() || min1.z() > max1.z()) {
        setToNull();
    } else {
        mincorner = min1;
        maxcorner = max1;
    }
}

/*!
    Returns a new box which is the intersection of this box with \a box.

    \sa intersect(), intersects(), united()
*/
QBox3D QBox3D::intersected(const QBox3D& box) const
{
    QBox3D result(*this);
    result.intersect(box);
    return result;
}

/*!
    Unites this box with \a point by expanding it to encompass \a point.
    If \a point is already contained within this box, then this box
    will be unchanged.

    \sa united(), intersect()
*/
void QBox3D::unite(const QVector3D& point)
{
    if (boxtype == Finite) {
        if (point.x() < mincorner.x())
            mincorner.setX(point.x());
        else if (point.x() > maxcorner.x())
            maxcorner.setX(point.x());
        if (point.y() < mincorner.y())
            mincorner.setY(point.y());
        else if (point.y() > maxcorner.y())
            maxcorner.setY(point.y());
        if (point.z() < mincorner.z())
            mincorner.setZ(point.z());
        else if (point.z() > maxcorner.z())
            maxcorner.setZ(point.z());
    } else if (boxtype == Null) {
        boxtype = Finite;
        mincorner = point;
        maxcorner = point;
    }
}

/*!
    Unites this box with \a box by expanding this box to encompass the
    region defined by \a box.  If \a box is already contained within
    this box, then this box will be unchanged.

    \sa united(), intersect()
*/
void QBox3D::unite(const QBox3D& box)
{
    if (box.boxtype == Finite) {
        unite(box.minimum());
        unite(box.maximum());
    } else if (box.boxtype == Infinite) {
        setToInfinite();
    }
}

/*!
    Returns a new box which unites this box with \a point.  The returned
    value will be the smallest box that contains both this box and \a point.

    \sa unite(), intersected()
*/
QBox3D QBox3D::united(const QVector3D& point) const
{
    if (boxtype == Finite) {
        QBox3D result(*this);
        result.unite(point);
        return result;
    } else if (boxtype == Null) {
        return QBox3D(point, point);
    } else {
        return *this;
    }
}

/*!
    Returns a new box which unites this box with \a box.  The returned value
    will be the smallest box that contains both this box and \a box.

    \sa unite(), intersected()
*/
QBox3D QBox3D::united(const QBox3D& box) const
{
    if (boxtype == Finite) {
        QBox3D result(*this);
        result.unite(box);
        return result;
    } else if (boxtype == Null) {
        return box;
    } else {
        return *this;
    }
}

/*!
    Transforms this box according to \a matrix.  Each of the 8 box
    corners are transformed and then a new box that encompasses all
    of the transformed corner values is created.

    \sa transformed()
*/
void QBox3D::transform(const QMatrix4x4& matrix)
{
    *this = transformed(matrix);
}

/*!
    Returns this box transformed by \a matrix.  Each of the 8 box
    corners are transformed and then a new box that encompasses all
    of the transformed corner values is returned.

    \sa transform()
*/
QBox3D QBox3D::transformed(const QMatrix4x4& matrix) const
{
    if (boxtype != Finite)
        return *this;
    QBox3D result;
    result.unite(matrix * mincorner);
    result.unite(matrix * QVector3D(mincorner.x(), mincorner.y(), maxcorner.z()));
    result.unite(matrix * QVector3D(mincorner.x(), maxcorner.y(), maxcorner.z()));
    result.unite(matrix * QVector3D(mincorner.x(), maxcorner.y(), mincorner.z()));
    result.unite(matrix * QVector3D(maxcorner.x(), mincorner.y(), mincorner.z()));
    result.unite(matrix * QVector3D(maxcorner.x(), maxcorner.y(), mincorner.z()));
    result.unite(matrix * QVector3D(maxcorner.x(), mincorner.y(), maxcorner.z()));
    result.unite(matrix * maxcorner);
    return result;
}

/*!
    \fn bool QBox3D::operator==(const QBox3D& box) const

    Returns true if this box is identical to \a box.
*/

/*!
    \fn bool QBox3D::operator!=(const QBox3D& box) const

    Returns true if this box is not identical to \a box.
*/

void QBox3D::partition
    (const QVector3D &point, Partition *xpart,
     Partition *ypart, Partition *zpart) const
{
    *xpart = point.x() < mincorner.x() ? belowMin : (
            point.x() == mincorner.x() ? equalMin : (
                    point.x() < maxcorner.x() ? between : (
                            point.x() == maxcorner.x() ? equalMax : aboveMax)));
    *ypart = point.y() < mincorner.y() ? belowMin : (
            point.y() == mincorner.y() ? equalMin : (
                    point.y() < maxcorner.y() ? between : (
                            point.y() == maxcorner.y() ? equalMax : aboveMax)));
    *zpart = point.z() < mincorner.z() ? belowMin : (
            point.z() == mincorner.z() ? equalMin : (
                    point.z() < maxcorner.z() ? between : (
                            point.z() == maxcorner.z() ? equalMax : aboveMax)));
}

/*!
    \fn bool qFuzzyCompare(const QBox3D& box1, const QBox3D& box2)
    \relates QBox3D

    Returns true if \a box1 and \a box2 are almost equal; false otherwise.
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QBox3D &box)
{
    if (box.isFinite()) {
        dbg.nospace() << "QBox3D(("
            << box.minimum().x() << ", " << box.minimum().y() << ", "
            << box.minimum().z() << ") - ("
            << box.maximum().x() << ", " << box.maximum().y() << ", "
            << box.maximum().z() << "))";
        return dbg.space();
    } else if (box.isNull()) {
        dbg << "QBox3D(null)";
        return dbg;
    } else {
        dbg << "QBox3D(infinite)";
        return dbg;
    }
}

#endif

#ifndef QT_NO_DATASTREAM

/*!
    \fn QDataStream &operator<<(QDataStream &stream, const QBox3D &box)
    \relates QBox3D

    Writes the given \a box to the given \a stream and returns a
    reference to the stream.
*/

QDataStream &operator<<(QDataStream &stream, const QBox3D &box)
{
    if (box.isNull()) {
        stream << int(0);
    } else if (box.isInfinite()) {
        stream << int(2);
    } else {
        stream << int(1);
        stream << box.minimum();
        stream << box.maximum();
    }
    return stream;
}

/*!
    \fn QDataStream &operator>>(QDataStream &stream, QBox3D &box)
    \relates QBox3D

    Reads a 3D box from the given \a stream into the given \a box
    and returns a reference to the stream.
*/

QDataStream &operator>>(QDataStream &stream, QBox3D &box)
{
    int type;
    stream >> type;
    if (type == 1) {
        QVector3D minimum, maximum;
        stream >> minimum;
        stream >> maximum;
        box = QBox3D(minimum, maximum);
    } else if (type == 2) {
        box.setToInfinite();
    } else {
        box.setToNull();
    }
    return stream;
}

#endif // QT_NO_DATASTREAM

QT_END_NAMESPACE
