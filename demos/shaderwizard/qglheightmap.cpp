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

#include "qglheightmap.h"
#include <QVector3D>
#include "qglvertexarray.h"
#include "qgldisplaylist.h"
#include "qgloperation.h"

/*!
    \class QGLHeightMap
    \brief The QGLHeightMap class is a convenience class designed to make it easy to create a simple 2D surface in 3D space.
    \since 4.7
    \ingroup painting-3D

    Many 3D objects can be conveniently thought of as a 2D object in a 3D space,
    such as flags, sheets of paper, mirrors, etc.

    It can be very helpful when learning about 3D programming to easily create
    and manipulate these in isolation before moving on to more complicated
    objects that might involve many different surfaces used in many different
    ways.

    QGLHeightMap makes it easy to create and draw simple height maps.  The
    surfaces created are characterized by being composed of vertices spaced
    equally between 0.0 and 1.0 in the x and y dimensions, but varying in the
    z dimension.  

    After creation, general QGLPainter functions can be used to manipulate
    these surfaces into any desired orientation or scale, and to apply 
    materials or textures as desired.

    A QGLHeightmap is intended to be added to a display list, using the
    << operator, and is then ready to be drawn with a QGLPainter.  For example:

    \code
    QGLDisplayList heightMapList
    heightMapList << QGLHeightMap(51, 51);
    \endcode

    Assuming that an appropriate QGLContext or QGLView is available, a
    height map can be created and drawn immediately with the QGLGeometry::draw()
    function.  Calculating the vertexes is expensive, however, and it is not
    recommend that a QGLHeightMap be recreated every update.

    The default implementation creates a flat plane, where the every vertex
    lies in the plane z = 0.0, with vertex and texture coordinates spaced
    evenly across that plane.

    For example, the QGLHeightMap(11, 11) would be a flat plane in the
    x,y dimension with vertices at x= [0.0, 0.1, 0.2... 0.9, 1.0] and
    y=[0.0, 0.1, 0.2 ... , 0.9, 1.0], with a z value of 0.0 for all vertexes,
    and tex coordinates for each vertex equal to that vertexes x and y values.

    Custom heights and texture coordinates can be set in a subclass by
    overriding the height() and texCoords() functions.

    For example a wavy effect might look like:
    \code
    #include <qglheightmap.h>
    #include <QtCore/qmath.h>
    #include "math.h"

    class MyHeightMap : public QGLHeightMap
    {
    public:
        MyHeightMap(int xPoints, int yPoints) : QGLHeightMap(xPoints,yPoints){};
        float height(float s, float t)
        {
            return (sin(s * M_PI * 4.0) + cos(t * M_PI * 4.0)) * 0.05;
        }
    };
    \endcode

    \sa QGLDisplayList, QGLPainger, height(), texCoords()
*/

/*!
    \fn QGLHeightMap::QGLHeightMap(int xPoints, int yPoints, float xSize, float ySize, QGL::Smoothing smoothing)

    Constructs a QGLHeightMap with the specified number of vertexes across
    each dimension, and calculates all vertex and normal information.  The
    total number of vertexes will therefor be \a xPoints * \a yPoints, with floating
    point calculations for each normal on top of the actual height function.
    Beware the memory and performance implications of large numbers of points.

    It is not valid for a QGLHeightMap to have 0 or less points in either
    dimension.

    The \a xSize and \ySize parameters determine the size of the created 
    height map in the x and y dimensions respectively.  The height-map will
    reach from 0 in the x and y dimensions to [xSize,ySize] (inclusive) in
    the x and y dimensions.  The default for both values is 1.0.
    
    The \smoothing determines whether normals for a vertex are averaged.
    The default, QGL::Smooth will average the normals, making the surface look
    rounder and smoother. The alternative, QGL::Faceted, will give the
    height map angular edges where faces meet, like a jewel.

*/
QGLHeightMap::QGLHeightMap(int xPoints, int yPoints, float xSize, float ySize, QGL::Smoothing smoothing) :
        m_xPoints(xPoints),
        m_yPoints(yPoints),
        m_xSize(xSize),
        m_ySize(ySize),
        m_Smoothing(smoothing)
{
    Q_ASSERT(xPoints > 0 && yPoints > 0);
}

QGLHeightMap::~QGLHeightMap()
{
}

/*!
    \fn QGLHeightMap::height()

    Calculates the actual z coordinate of a given the nominal x, y and z coordinates.
    s and t should be in the range 0.0 - 1.0.

    The default implementation simply returns 0.0 for every point to create a flat plane.

    Override this function in a subclass to change the shape of the height map.

    \sa calculateVertexes()
  */

float QGLHeightMap::height(float s, float t) const
{
    Q_UNUSED(s);
    Q_UNUSED(t);
    return 0.0;
}

/*!
  \fn calculateTexCoord(float s, float t)

    This function returns the tex coordinate for a given vertex.  Reimplement this
    function in a subclass to change the way texCoords are calculated.

    The default implementation sets the tex coordinates to (s,t) for each vertex.
    This should give a good effect in most cases, but may cause distortion if
    there is a large variance in z.

    The potential problem should be made clear with this example: Consider a
    height map with 11*11 vertices that is mostly flat, but with the rightmost
    set of points (t == 1.0) raised by the width of the height map.  The one
    set of quads on the rightmost edge will cover a comparable area as the
    rest of the quads in that row combined, but will have the same amount of
    texture data as only one of them spread over that area.  This is likely to
    look bad.
*/

QVector2D QGLHeightMap::texCoords(float s, float t) const
{
    return QVector2D(s,t);
}

QGL::Smoothing QGLHeightMap::smoothing() const
{
    return m_Smoothing;
}


int QGLHeightMap::xPoints() const
{
    return m_xPoints;
}

int QGLHeightMap::yPoints() const
{
    return m_yPoints;
}

int QGLHeightMap::xSize() const
{
    return m_xSize;
}

int QGLHeightMap::ySize() const
{
    return m_ySize;
}

static float normalizedIndex(int i, int points) {
    return points <= 1 ? 0.0 : float(i) / float(points - 1);
}

/*!
    Builds the geometry for \a heightMap within the specified
    display \a list.

    This operator specifies the positions, normals, and 2D texture
    co-ordinates for all of the vertices that make up the height map.

    For a QGLHeightMap, the texture co-ordinate (0, 0) corresponds to
    the bottom-left corner of each side and the texture co-ordinate
    (1, 1) corresponds to the upper-right corner of each side.

    Subclasses may specify the height and texture coordinates differently by overriding the and texCoords() functions.
*/
QGLDisplayList& operator<<(QGLDisplayList& list, const QGLHeightMap& heightMap)
{
    int xPoints = heightMap.xPoints();
    int yPoints = heightMap.yPoints();

    list.newSection(heightMap.smoothing() );
    QGLOperation op(&list, QGL::TRIANGLE);

    for(int y = 0; y < yPoints - 1; y++)
    {
        for(int x = 0; x < xPoints - 1; x++)
        {
            float s0 = normalizedIndex(x, xPoints);
            float s1 = normalizedIndex(x + 1, xPoints);
            float t0 = normalizedIndex(y, yPoints);
            float t1 = normalizedIndex(y + 1, yPoints);
            QVector3D bottomLeftPoint(s0 * heightMap.xSize(), t0 * heightMap.ySize(), heightMap.height(s0, t0));
            QVector3D bottomRightPoint(s1 * heightMap.xSize(), t0 * heightMap.ySize(), heightMap.height(s1, t0));
            QVector3D topLeftPoint(s0 * heightMap.xSize(), t1 * heightMap.ySize(), heightMap.height(s0, t1));
            QVector3D topRightPoint(s1 * heightMap.xSize(), t1 * heightMap.ySize(), heightMap.height(s1, t1));

            // Top left triangle
            op.addVertex(bottomLeftPoint);
            op.addTexCoord(heightMap.texCoords(s0, t0));

            op.addVertex(topRightPoint);
            op.addTexCoord(heightMap.texCoords(s1, t1));

            op.addVertex(topLeftPoint);
            op.addTexCoord(heightMap.texCoords(s0, t1));

            // Bottom right triangle
            op.addVertex(bottomLeftPoint);
            op.addTexCoord(heightMap.texCoords(s0, t0));

            op.addVertex(bottomRightPoint);
            op.addTexCoord(heightMap.texCoords(s1, t0));

            op.addVertex(topRightPoint);
            op.addTexCoord(heightMap.texCoords(s1, t1));

        }
    }
    return list;
}
