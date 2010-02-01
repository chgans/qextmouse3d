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

#include "qglcube.h"
#include "qgldisplaylist.h"
#include "qgloperation.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLCube
    \brief The QGLCube class represents the geometry of simple six-sided cube in 3D space.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example adds a cube of 2 units on a side to a
    display list, and draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLDisplayList list;
    list << QGLCube(2);
    painter->translate(10, 25, 0);
    list.draw(painter);
    \endcode

    \sa QGLCubeFace
*/

/*!
    \fn QGLCube::QGLCube(qreal size)

    Constructs the geometry for a regular cube of \a size
    units on a side.
*/

/*!
    \fn qreal QGLCube::size() const

    Returns the size of this cube.

    \sa setSize()
*/

/*!
    \fn void QGLCube::setSize(qreal size)

    Sets the \a size of this cube.

    \sa size()
*/

/*!
    \class QGLCubeFace
    \brief The QGLCubeFace class represents the geometry of one face of a simple six-sided cube in 3D space.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example adds a cube of 2 units on a side to a
    display list, one face at a time:

    \code
    QGLDisplayList list;
    list << QGLCubeFace(QGLCubeFace::Left, 2);
    list << QGLCubeFace(QGLCubeFace::Top, 2);
    list << QGLCubeFace(QGLCubeFace::Right, 2);
    list << QGLCubeFace(QGLCubeFace::Bottom, 2);
    list << QGLCubeFace(QGLCubeFace::Front, 2);
    list << QGLCubeFace(QGLCubeFace::Back, 2);
    \endcode

    QGLCubeFace allows greater control over the texture co-ordinates
    of a cube face than is possible with QGLCube.  The following
    example adds the left face of a cube with texture co-ordinates
    extending from (0, 0.5) to (0.5, 1.0):

    \code
    QGLDisplayList list;
    QGLCubeFace left(QGLCubeFace::Left, 2);
    left.setBottomLeftTextureCoord(0.0f, 0.5f);
    left.setTopRightTextureCoord(0.5f, 1.0f);
    list << left;
    \endcode

    \sa QGLCube
*/

/*!
    \enum QGLCubeFace::Face
    This enum defines the face to generate for a QGLCubeFace object.

    \value Left Left of the cube at -X.
    \value Top Top of the cube at +Y.
    \value Right Right of the cube at +X.
    \value Bottom Bottom of the cube at -Y.
    \value Front Front of the cube at +Z
    \value Back Back of the cube at -Z.
*/

/*!
    \fn QGLCubeFace::QGLCubeFace(QGLCubeFace::Face face, qreal size)

    Constructs a specific \a face of a regular cube of
    \a size units on a side.
*/

/*!
    \fn qreal QGLCubeFace::size() const

    Returns the size of this cube face.

    \sa setSize()
*/

/*!
    \fn void QGLCubeFace::setSize(qreal size)

    Sets the \a size of this cube face.

    \sa size()
*/

/*!
    \fn QGLCubeFace::Face QGLCubeFace::face() const

    Returns the cube face that will be generated by this object.

    \sa setFace()
*/

/*!
    \fn void QGLCubeFace::setFace(QGLCubeFace::Face face)

    Sets the cube \a face that will be generated by this object.

    \sa face()
*/

/*!
    \fn QVector2D QGLCubeFace::bottomLeftTextureCoord() const

    Returns the texture co-ordinate of the bottom-left corner
    of the cube face.  The default value is (0, 0).

    \sa setBottomLeftTextureCoord(), topRightTextureCoord()
*/

/*!
    \fn void QGLCubeFace::setBottomLeftTextureCoord(const QVector2D& value)

    Sets the texture co-ordinate of the bottom-left corner of
    the cube face to \a value.

    \sa bottomLeftTextureCoord(), setTopRightTextureCoord()
*/

/*!
    \fn void QGLCubeFace::setBottomLeftTextureCoord(qreal x, qreal y)
    \overload

    Sets the texture co-ordinate of the bottom-left corner of
    the cube face to (\a x, \a y).
*/

/*!
    \fn QVector2D QGLCubeFace::topRightTextureCoord() const

    Returns the texture co-ordinate of the top-right corner
    of the cube face.  The default value is (1, 1).

    \sa setTopRightTextureCoord(), bottomLeftTextureCoord()
*/

/*!
    \fn void QGLCubeFace::setTopRightTextureCoord(const QVector2D& value)

    Sets the texture co-ordinate of the top-right corner of
    the cube face to \a value.

    \sa topRightTextureCoord(), setBottomLeftTextureCoord()
*/

/*!
    \fn void QGLCubeFace::setTopRightTextureCoord(qreal x, qreal y)
    \overload

    Sets the texture co-ordinate of the top-right corner of
    the cube face to (\a x, \a y).
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
    \relates QGLCube

    Builds the geometry for \a cube within the specified
    display \a list.

    This operator specifies the positions, normals, and 2D texture
    co-ordinates for all of the vertices that make up the cube.

    For each face, the texture co-ordinate (0, 0) corresponds to
    the bottom-left corner of each side and the texture co-ordinate
    (1, 1) corresponds to the upper-right corner of each side.
*/
QGLDisplayList& operator<<(QGLDisplayList& list, const QGLCube& cube)
{
    list.newSection(QGL::Faceted);
    QGLOperation op(&list, QGL::TRIANGLE);
    qreal size = cube.size();
    for (int vertex = 0; vertex < QGL_CUBE_SIZE; vertex += 8) {
        op.addVertex(QVector3D(cubeVertices[vertex] * size,
                               cubeVertices[vertex + 1] * size,
                               cubeVertices[vertex + 2] * size));
        op.addNormal(QVector3D(cubeVertices[vertex + 3],
                               cubeVertices[vertex + 4],
                               cubeVertices[vertex + 5]));
        op.addTexCoord(QVector2D(cubeVertices[vertex + 6],
                                 cubeVertices[vertex + 7]));
    }
    return list;
}

/*!
    \relates QGLCubeFace

    Builds the geometry for \a face within the specified
    display \a list.

    This operator specifies the positions, normals, and 2D texture
    co-ordinates for all of the vertices that make up the cube face.
*/
QGLDisplayList& operator<<(QGLDisplayList& list, const QGLCubeFace& face)
{
    list.newSection(QGL::Faceted);
    QGLOperation op(&list, QGL::TRIANGLE);
    int offset = int(face.face()) * 8;
    qreal size = face.size();
    QVector2D bottomLeft = face.bottomLeftTextureCoord();
    QVector2D topRight = face.topRightTextureCoord();
    for (int vertex = 0; vertex < 6; ++vertex, offset += 8) {
        op.addVertex(QVector3D(cubeVertices[offset] * size,
                               cubeVertices[offset + 1] * size,
                               cubeVertices[offset + 2] * size));
        op.addNormal(QVector3D(cubeVertices[offset + 3],
                               cubeVertices[offset + 4],
                               cubeVertices[offset + 5]));
        qreal tx = (cubeVertices[offset + 6] == 0.0f
                        ? bottomLeft.x() : topRight.x());
        qreal ty = (cubeVertices[offset + 7] == 0.0f
                        ? bottomLeft.y() : topRight.y());
        op.addTexCoord(QVector2D(tx, ty));
    }
    return list;
}

QT_END_NAMESPACE
