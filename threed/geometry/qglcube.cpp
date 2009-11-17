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

#include "qglcube.h"
#include "qglpainter.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLCube
    \brief The QGLCube class represents the geometry of simple six-sided cube or prism in 3D space.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example creates a cube of 2 units on a side,
    and draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLCube cube(2);
    painter.translate(10, 25, 0);
    painter.draw(&cube);
    \endcode

    The QGLCube class specifies positions, normals, and 2D texture
    co-ordinates for all of the vertices that make up the cube.
    Each side is drawn as a quadrilateral.

    By default the texture co-ordinate (0, 0) corresponds to the upper-left
    corner of each side and the texture co-ordinate (1, 1) corresponds
    to the lower-right corner of each side.  The texture co-ordinates
    for the faces can be altered with setTextureCoords().

    \sa QGLGeometry, QGLPainter
*/

#define QGL_CUBE_SIZE (6 * 6 * (3 + 3 + 2))
static float const cubeVertices[QGL_CUBE_SIZE] = {
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f
};

/*!
    Constructs the geometry for a regular cube of \a size
    units on a side, and attaches it to \a parent.
*/
QGLCube::QGLCube(QObject *parent)
    : QGLGeometry(parent)
{
    setDrawingMode(QGL::Triangles);
    QGLVertexArray array
        (QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);
    array.setRawData(cubeVertices, QGL_CUBE_SIZE);
    setVertexArray(array);
}

/*!
    Constructs the geometry for a regular cube of size 1
    units on a side, and attaches it to \a parent.
*/
QGLCube::QGLCube(qreal size, QObject *parent)
    : QGLGeometry(parent)
{
    setDrawingMode(QGL::Triangles);
    if (size == 1.0f) {
        // Use the raw static data directly for cubes of size 1.
        QGLVertexArray array
            (QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);
        array.setRawData(cubeVertices, QGL_CUBE_SIZE);
        setVertexArray(array);
    } else {
        init(size, size, size);
    }
}

/*!
    Constructs the geometry for a prism which is \a width units
    in size in the x direction, \a height units in size in the y
    direction, and \a depth units in size in the z direction,
    and attaches it to \a parent.

    If \a width, \a height, and \a depth are all the same,
    the result will be a regular cube.
*/
QGLCube::QGLCube(qreal width, qreal height, qreal depth, QObject *parent)
    : QGLGeometry(parent)
{
    setDrawingMode(QGL::Triangles);
    init(width, height, depth);
}

/*!
    Destroys this cube geometry object.
*/
QGLCube::~QGLCube()
{
    // Nothing to do here at present.
}

/*!
    Sets the texture co-ordinates for the four corners of \a face
    to \a c1, \a c2, \a c3, and \a c4.  If \a face is -1, then
    all faces will have their texture co-ordinates set.
*/
void QGLCube::setTextureCoords
    (const QVector2D& c1, const QVector2D& c2,
     const QVector2D& c3, const QVector2D& c4, int face)
{
    QGLVertexArray array = vertexArray();
    if (face == -1) {
        for (face = 0; face < 6; ++face) {
            array.setAt(face * 6 + 0, 2, c1);
            array.setAt(face * 6 + 1, 2, c2);
            array.setAt(face * 6 + 2, 2, c3);
            array.setAt(face * 6 + 3, 2, c1);
            array.setAt(face * 6 + 4, 2, c3);
            array.setAt(face * 6 + 5, 2, c4);
        }
    } else if (face >= 0 && face < 6) {
        array.setAt(face * 6 + 0, 2, c1);
        array.setAt(face * 6 + 1, 2, c2);
        array.setAt(face * 6 + 2, 2, c3);
        array.setAt(face * 6 + 3, 2, c1);
        array.setAt(face * 6 + 4, 2, c3);
        array.setAt(face * 6 + 5, 2, c4);
    }
    setVertexArray(array);
}

/*!
    Draws a single \a face from this cube on \a painter.

    \sa draw()
*/
void QGLCube::drawFace(QGLPainter *painter, int face)
{
    if (face >= 0 && face <= 5) {
        painter->setVertexArray(vertexArray());
        painter->draw(QGL::Triangles, 6, face * 6);
    }
}

void QGLCube::init(qreal width, qreal height, qreal depth)
{
    QGLVertexArray vertices
        (QGL::Position, 3, QGL::Normal, 3, QGL::TextureCoord0, 2);
    for (int index = 0; index < 6 * 6; ++index) {
        vertices.append(cubeVertices[index * 8 + 0] * width,
                        cubeVertices[index * 8 + 1] * height,
                        cubeVertices[index * 8 + 2] * depth);
        vertices.append(cubeVertices[index * 8 + 3],
                        cubeVertices[index * 8 + 4],
                        cubeVertices[index * 8 + 5]);
        vertices.append(cubeVertices[index * 8 + 6],
                        cubeVertices[index * 8 + 7]);
    }
    setVertexArray(vertices);
}

QT_END_NAMESPACE
