/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglprimitive.h"
#include "qgldisplaylist.h"

/*!
    \class QGLPrimitive
    \brief The QGLPrimitive class is a data container for QGLDisplayList operations.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    Use QGLPrimitive instances to pass defining geometry information into
    QGLGeometry calls.  As a subclass of QGLGeometry, QGLPrimitive inherits
    all the low-level vertex and other data definition functions.  QGLPrimitive
    adds a range of higher-level functions for defining how QGLDisplayList
    will treat that data when definining primitives:
    \list
        \o Use setFlags() for example to change QGL::QUADS_ZIPPED face winding
        \o Set a normal to apply to all vertices with setCommonNormal()
        \o Create an out-of-band control vector with setControl()
    \endlist

    QGLPrimitive also defines a few useful functions for processing data in
    common ways in geometry definition:
    \list
        \o reversed() returns a copy of this primitve with data in reverse order
        \o translated() returns a copy of this primitive translated by a vector
        \o generateTextureCoordinates() automatically calculates texture coordinates
    \endlist
*/

/*!
    Constructs a new QGLPrimitive.
*/
QGLPrimitive::QGLPrimitive()
{
}

/*!
    \fn QGLPrimitive::~QGLPrimitive()
    Destroys this QGLPrimitive reclaiming any resources.
*/

/*!
    Returns the coordinates of the center of the primitive.
    The center is calculated as the centroid or geometric barycenter
    of the vertices (the average of the vertices).  For a convex hull this
    is guaranteed to be inside the figure.
*/
QVector3D QGLPrimitive::center() const
{
    QVector3D center;
    QDataArray<QVector3D> v = vertices();
    for (int i = 0; i < v.count(); ++i)
        center += v[i];
    return center / (float)v.count();
}

/*!
    \fn void QGLPrimitive::setFlags(QGL::OperationFlags flags)
    Sets the primitive's operation flags to \a flags.
*/

/*!
    \fn QGL::OperationFlags QGLPrimitive::flags() const
    Returns the primitive's operation flags, or QGL::NO_FLAG if none has been set.
*/

/*!
    Returns a copy of this primitive with all data in reverse order.
*/
QGLPrimitive QGLPrimitive::reversed() const
{
    QGLPrimitive r;
    for (int i = count() - 1; i >= 0; --i)
        r.appendVertex(vertexAt(i));
    return r;
}

/*!
    Returns a copy of this primitive with QGL::Position data translated by
    the vector \a t.  The other fields are unchanged.
*/
QGLPrimitive QGLPrimitive::translated(const QVector3D &t) const
{
    QGLPrimitive r;
    QDataArray<QVector3D> v = vertices();
    for (int i = 0; i < count(); ++i)
    {
        r.appendVertex(vertexAt(i));
        r.vertexRef(i) = v[i] + t;
    }
    return r;
}

/*!
    Returns a QGLPrimitive instance containing alternating vertices from
    this primitive and \a other.  The resulting geometry contains N vertices
    where \c{N == qMin(count(), other.count())}, and has only the fields
    that are in both primitives.
*/
QGLPrimitive QGLPrimitive::zippedWith(const QGLPrimitive &other) const
{
    QGLPrimitive temp = *this;
    temp.zipWith(other);
    return temp;
}

/*!
    Modifies this primitive by generating texture data based on QGL::Position
    values.  If \a orientation is Qt::Horizontal (the default) then x-coordinate
    values are generated, and y-coordinate values are set to 0.0; otherwise
    y-coordinate values are generated and x-coordinate values are set to 0.0.
    The values are appended to the texture coordinate \a field.

    The method of calculation is based on the assumption that the vertex data
    is a list of extents which span across the texture space horizontally, from
    x = 0.0 to x = 1.0, in the case of Qt::Horizontal; or vertically in the
    case of Qt::Vertical.  The texture space of 1.0 is divided up proportionately
    by the length of each extent.

    \image texture-coords-gen.png

    In this diagram the large blue numbers are the lengths of each extent, and
    the texture coordinates generated are shown as \c{t(7/16, 1)} and so on.

    Thus the texture coordinate t0 for vertex v0, is 0.0; t1 for vertex v1 is
    \c{(v1 - v0).length() / totalLength} and so on.

    The code to produce the texture coordinates for the quads in the image is:
    \code
    QGLPrimitive top;

    // add data to the primitive
    top.appendVertex(QVector3D(0.0, 0.0, 0.0));
    top.appendVertex(QVector3D(6.0, 3.6, 0.0));    // (v1 - v0).length() = 7.0
    top.appendVertex(QVector3D(10.0, 0.6, 0.0));   // (v2 - v1).length() = 5.0
    top.appendVertex(QVector3D(13.0, 3.24, 0.0));  // (v3 - v2).length() = 4.0

    // generate x (Qt::Horizontal) texture coordinates over the primitive
    top.generateTextureCoordinates();              // spread over 7 + 5 + 4 = 16

    // make a copy translated down, the copy has y texture coordinates all 0
    QGLPrimitive bottom = top.translated(QVector3D(0, 0, -1));

    // now modify the top so its y texture coordinates are all 1
    for (int i = 0; i < top.count(); ++i)
        top.texCoordRef(QGL::TextureCoord0).setY(1.0);

    displayList->addQuadsZipped(top, bottom);
    \endcode
*/
void QGLPrimitive::generateTextureCoordinates(Qt::Orientation orientation, QGL::VertexAttribute field)
{
    QDataArray<qreal> extents;
    extents.append(0.0);
    qreal totalExtents = 0.0;
    QDataArray<QVector3D> v = vertices();
    for (int i = 0; i < v.count() - 1; ++i)
    {
        int n = (i + 1) % v.count();
        QVector3D e = v[n] - v[i];
        qreal extent = e.length();
        totalExtents += extent;
        extents.append(totalExtents);
    }
    if (hasField(field))
        clear(field);
    if (orientation == Qt::Horizontal)
    {
        for (int i = 0; i < v.count(); ++i)
            appendTexCoord(QVector2D(extents[i] / totalExtents, 0.0), field);
    }
    else
    {
        for (int i = 0; i < v.count(); ++i)
            appendTexCoord(QVector2D(0.0, extents[i] / totalExtents), field);
    }
}
