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

#include "qglheightmap.h"
#include <QVector3D>
#include "qglvertexarray.h"

class QGLHeightMapPrivate
{
    public:
    float xPoints;
    float yPoints;

    bool vertexesCalculated;

    float indexToS(int i) { return xPoints == 1 ? 0.0 : float(i) / float(xPoints - 1); }
    float indexToT(int i) { return yPoints == 1 ? 0.0 : float(i) / float(yPoints - 1); }
    int XYToIndex(int x, int y) { return y * xPoints + x; }
};

/*!
    \class QGLHeightMap
    \brief The QGLHeightMap class is a convenience class designed to make it easy to create a simple 2D surface in 3D space.
    \since 4.6
    \ingroup painting-3D

    Many 3D objects can be conviently thought of as a 2D object in a 3D space,
    such as flags, sheets of paper, mirrors, etc.

    It can be very helpful when learning about 3D programming to easily create
    and manipulate these in isolation before moving on to more complicated
    objects that might involve many different surfaces used in many different
    ways.

    QGLHeightMap makes it easy to create and draw simple height maps.  The
    surfaces created are characterised by being composed of vertices spaced
    equally between 0.0 and 1.0 in the x and y dimensions, but varying in the
    z dimension.  After creation, general 3D functions can be used to
    manipulate these surfaces into any desired orientation or scale.

    The surfaces that QGLHeightMap creates are smooth, with each vertex having
    a normal that is the average of all faces that the vertex is part of.

    Assuming that an appropriate QGLContext or QGLView is available, a
    heightmap can be created and drawn immediately with the QGLGeometry::draw()
    function.  Calculating the vertexes is expensive, however, and it is not
    recommend that a QGLHeightMap be recreated every update.

    The default implementation creates a flat plane, where the every vertex
    lies in the plane z = 0.0, with vertex and texture coordinates spaced
    evenly across that plane.

    For example, the QGLHeightMap(11, 11) would be a flat plane in the
    x,y dimension with vertices at x= [0.0, 0.1, 0.2... 0.9, 1.0] and
    y=[0.0, 0.1, 0.2 ... , 0.9, 1.0], with a z value of 0.0 for all vertexes,
    and tex coords for each vertex equal to that vertexes x and y values.

    The default behaviour can be altered by overriding the calculation
    functions in a subclass, allowing for a variety of surfaces.

    For example, a ripple-like effect, centered on the middle of the the
    heightmap might look like:
    \code
    #include <qglheightmap.h>
    #include <QtCore/qmath.h>
    #include "math.h"
    #define PI 3.141592

    class MyHeightMap : public QGLHeightMap
    {
    public:
        MyHeightMap(int xPoints, int yPoints) : QGLHeightMap(xPoints,yPoints){};
        float height(float s, float t)
        {
            return 0.1 * cos( 5 * 2.0 * PI * sqrt((s - 0.5) * (s - 0.5) + (t - 0.5) * (t - 0.5)))
        }
    };
    \endcode

    \sa QGLGeometry, height(), calculateTexCoords()
*/

/*!
    \fn QGLHeightMap::QGLHeightMap(int xPoints, int yPoints)

    Constructs a QGLHeightMap with the specified number of vertexes across
    each dimension, and calculates all vertex and normal information.  The
    total number of vertexes will therefor be \a xPoints * \a yPoints, with floating
    point calculations for each normal on top of the actual height function.

    Beware the memory and performance implications of large numbers or points.

    It is not valid for a QGLHeightMap to have 0 or less points in either
    dimension.
*/
QGLHeightMap::QGLHeightMap(int xPoints, int yPoints) :
        d(new QGLHeightMapPrivate)
{
    d->xPoints = xPoints;
    d->yPoints = yPoints;
    d->vertexesCalculated = false;
}

QGLHeightMap::~QGLHeightMap()
{
}


void QGLHeightMap::draw(QGLPainter *painter)
{
    if(!d->vertexesCalculated)
    {
        init();
    }

    QGLGeometry::draw(painter);
}

/*!
    \fn QGLHeightMap::init()

    Populates the QGLHeightMap's VertexArray with Vertex, Surface, and Tex
    fields, ready to draw().

    Automatically called the first time draw() is called.

    \sa QGLGeometry::draw()
*/

void QGLHeightMap::init()
{
    calculateVertexes();
}


/*!
    \fn calculateVertexes()

    This function creates the vertex and index array and assigns them to the
    QGLGeometry.

    More specifically, it generates the position texture coordinates for each
    vertexes in the heightmap, then calculates the indexes for the triangles
    and the normals for the vertexes.

    The normals are calculated using the average of the normals of the
    faces that a given vertex is part of, resulting in a smooth mesh.

    \sa calculateTexCoords(), height()
*/

void QGLHeightMap::calculateVertexes()
{
    QGLVertexArray positions(QGL::Position, 3);
    QGLVertexArray normals(QGL::Normal, 3);
    QGLVertexArray texes(QGL::TextureCoord0, 2);

    QGLIndexArray indexArray;
    float s,t;

    // Calculate vertexes positions and texture coords for each vertex.
    for(int y = 0; y < d->yPoints; y++)
    {
        for(int x = 0; x < d->xPoints; x++)
        {
            s = d->indexToS(x);
            t = d->indexToT(y);
            // Vertex
            positions.append(QVector3D( s, t, height(s, t)));
            // Texture Coords
            texes.append(calculateTexCoords(s,t));
        };
    }

    // Reserve and zero space for normals
    normals.reserve(d->yPoints * d->xPoints);
    for(int i = 0; i < d->yPoints * d->xPoints; i++)
        normals.append(0.0, 0.0, 0.0);

    // Calculate indexes and sums of normals
    int topLeftIndex, topRightIndex, bottomLeftIndex, bottomRightIndex;
    for(int y = 0; y < d->yPoints - 1; y++)
    {
        for(int x = 0; x < d->xPoints - 1; x++)
        {
            topLeftIndex = d->XYToIndex(x,y);
            topRightIndex = d->XYToIndex(x + 1,y);
            bottomLeftIndex = d->XYToIndex(x,y + 1);
            bottomRightIndex = d->XYToIndex(x + 1, y + 1);

            // Add the two triangles that make up this quad to the index array
            indexArray.append(topLeftIndex, bottomLeftIndex, topRightIndex);
            indexArray.append(topRightIndex, bottomLeftIndex, bottomRightIndex);

            // Then calculate the normals for each triangle
            QVector3D topLeftPos = positions.vector3DAt(topLeftIndex, 0);
            QVector3D topRightPos = positions.vector3DAt(topRightIndex, 0);
            QVector3D bottomLeftPos = positions.vector3DAt(bottomLeftIndex, 0);
            QVector3D bottomRightPos = positions.vector3DAt(bottomRightIndex, 0);

            QVector3D topLeftTriangleNormal =
                QVector3D::crossProduct(
                        // middle edge anticlockwise:
                        topRightPos - bottomLeftPos,
                        // left edge anticlockwise:
                        bottomLeftPos - topLeftPos);

            // Add the surface normal to the vertex normals of the triangle.
            normals.setAt(topLeftIndex, 0,
                normals.vector3DAt(topLeftIndex, 0) + topLeftTriangleNormal);
            normals.setAt(topRightIndex, 0,
                normals.vector3DAt(topRightIndex, 0) + topLeftTriangleNormal);
            normals.setAt(bottomLeftIndex, 0,
                normals.vector3DAt(bottomLeftIndex, 0) + topLeftTriangleNormal);

             QVector3D bottomRightTriangleNormal =
                QVector3D::crossProduct(
                        // bottom edge anticlockwise:
                        bottomRightPos - bottomLeftPos,
                        // middle edge anticlockwise (reverse of other triangle):
                        bottomLeftPos - topRightPos);

             normals.setAt(topRightIndex, 0,
                normals.vector3DAt(topRightIndex, 0) + bottomRightTriangleNormal);
            normals.setAt(bottomLeftIndex, 0,
                normals.vector3DAt(bottomLeftIndex, 0) + bottomRightTriangleNormal);
            normals.setAt(bottomRightIndex, 0,
                normals.vector3DAt(bottomRightIndex, 0) + bottomRightTriangleNormal);
        }
    }

    // Now normalize the normals
    for(int i = 0 ; i < normals.vertexCount(); i++)
    {
         normals.setAt(i, 0,
            normals.vector3DAt(i, 0).normalized());
    }

    QGLVertexArray combinedArray = positions.interleaved(normals).interleaved(texes);
    setVertexArray(combinedArray);
    setIndexArray(indexArray);

    setDrawingMode(QGL::Triangles);

    d->vertexesCalculated = true;
}

/*!
    \fn QGLHeightMap::height()

    Calculates the actual z coordinate of a given the nominal x, y and z coordinates.
    s and t should be in the range 0.0 - 1.0.

    The default implementation simply returns 0.0 for every point to create a flat plane.

    Override this function in a subclass to change the shape of the heightmap.

    \sa calculateVertexes()
  */

float QGLHeightMap::height(float s, float t)
{
    Q_UNUSED(s);
    Q_UNUSED(t);
    return 0.0;
}

/*!
  \fn calculateTexCoord(float s, float t)

    This function returns the tex coord for a given vertex.  Reimplement this
    function in a subclass to change the way texCoords are calculated.

    The default implementation sets the tex coords to (s,t) for each vertex.
    This should give a good effect in most cases, but may cause distortion if
    there is a large variance in z.

    The potential problem should be made clear with this example: Consider a heightmap with 11*11 vertecies that is mostly flat,
    but with the rightmost set of points (t == 1.0) raised by the width of the heightmap.  The one set of quads on the rightmost
    edge will cover a comparable area as the rest of the quads in that row combined, but will have the same amount of texture data
    as only one of them spread over that area.  This is likely to look bad.

    This class is not well suited to solving this problem, but may provide a
    useful example while developing one that is.
*/

QPointF QGLHeightMap::calculateTexCoords(float s, float t)
{
    return QPointF(s,t);
}

