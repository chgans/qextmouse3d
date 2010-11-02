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
#include "qglbuilder.h"
#include "qvector3darray.h"

QT_BEGIN_NAMESPACE

/*!
    \class QGLCube
    \brief The QGLCube class represents the geometry of simple six-sided cube in 3D space.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example adds a cube of 2 units on a side to a
    geometry builder, and draws it at (10, 25, 0) in a QGLPainter:

    \code
    QGLBuilder list;
    list.newSection(QGL::Faceted);
    list << QGLCube(2);
    painter->translate(10, 25, 0);
    list.draw(painter);
    \endcode

    QGLCube will automatically create a default set of texture coordinates
    on the displaylist.  See QGLCubeFace to gain greater control of these
    coordinates.

    \image cube.png

    This image shows the cube example program running, with the
    \l{QGLCubeFace::Front}{front face} showing foremost.  The default orientation of
    the \l{QGLCubeFace::Top}{top face} and \l{QGLCubeFace::Left}{left face}
    can also be seen.

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
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::geometry

    The following example adds a cube of 2 units on a side to a
    geometry builder, one face at a time.

    The orientation of the faces are shown in these labelled screenshots
    from the cube example program.

    \code
    QGLBuilder list;
    list << QGLCubeFace(QGLCubeFace::Top, 2);
    list << QGLCubeFace(QGLCubeFace::Front, 2);
    list << QGLCubeFace(QGLCubeFace::Right, 2);
    \endcode

    \image cube-front.png

    \code
    list << QGLCubeFace(QGLCubeFace::Back, 2);
    list << QGLCubeFace(QGLCubeFace::Left, 2);
    list << QGLCubeFace(QGLCubeFace::Bottom, 2);
    \endcode

    \image cube-rear.png

    QGLCubeFace allows greater control over the texture co-ordinates
    of a cube face than is possible with QGLCube.  The following
    example adds the left face of a cube with texture co-ordinates
    extending from (0, 0.5) to (0.5, 1.0) so that the just top-left
    quadrant of the texture is addressed:

    \code
    QGLBuilder list;
    QGLCubeFace left(QGLCubeFace::Left, 2);
    left.setTextureCoord(QGLCubeFace::BottomLeft, 0.0f, 0.5f);
    left.setTextureCoord(QGLCubeFace::BottomRight, 0.5f, 0.5f);
    left.setTextureCoord(QGLCubeFace::TopRight, 0.5f, 1.0f);
    list << left;
    \endcode

    This code has the effect of rotating the texture into a
    an orientation at 90 degrees to the default:

    \code
    QGLCubeFace right(QGLCubeFace::Right, 2);
    right.setTextureCoord(QGLCubeFace::BottomLeft, 1.0f, 0.0f);
    right.setTextureCoord(QGLCubeFace::BottomRight, 1.0f, 1.0f);
    right.setTextureCoord(QGLCubeFace::TopRight, 0.0f, 1.0f);
    right.setTextureCoord(QGLCubeFace::TopLeft, 0.0f, 0.0f);
    cube << right;
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
    \enum QGLCubeFace::Corner
    This enum specifies the corner of the QGLCubeFace to apply texture coordinates.

    \value BottomRight The bottom-right corner: default texture coords \c{(1.0, 0.0)}.
    \value TopRight  The top-right corner of the face: default texture coords \c{(1.0, 1.0)}.
    \value TopLeft  The top-left corner of the face: default texture coords \c{(0.0, 1.0)}.
    \value BottomLeft  The bottom-left corner of the face: default texture coords \c{(0.0, 0.0)}.
*/
static const int vertexDataLen = 6 * 4 * 3;

static const float vertexData[vertexDataLen] = {
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, -0.5f,

    -0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, -0.5f,

    0.5f, 0.5f, -0.5f,
    0.5f, 0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,

    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, 0.5f,

    0.5f, 0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f
};

static const int texCoordDataLen = 4 * 2;

static const float texCoordData[texCoordDataLen] = {
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
    0.0f, 0.0f
};

/*!
    Constructs a specific \a face of a regular cube of
    \a size units on a side.
*/
QGLCubeFace::QGLCubeFace(QGLCubeFace::Face face, qreal size)
    : m_size(size)
    , m_face(face)
    , m_texCoords(QVector2DArray::fromRawData(
            reinterpret_cast<const QVector2D *>(texCoordData), 4))
{
}

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
    \fn QVector2D QGLCubeFace::textureCoord(QGLCubeFace::Corner corner) const

    Returns the texture co-ordinate of the \a corner
    of the cube face.  See the \l{QGLCubeFace::Corner}{corner enum} for
    default values for each corner.

    \sa setTextureCoord()
*/

/*!
    \fn void QGLCubeFace::setTextureCoord(QGLCubeFace::Corner corner, const QVector2D& value)

    Sets the texture co-ordinate of the \a corner of
    the cube face to \a value.

    \sa textureCoord()
*/

/*!
    \fn void QGLCubeFace::setTextureCoord(QGLCubeFace::Corner corner, qreal x, qreal y)
    \overload

    Sets the texture co-ordinate of the \a corner of
    the cube face to (\a x, \a y).
*/

/*!
    \fn QVector2DArray QGLCubeFace::textureCoords() const

    Returns all the four texture coordinates of the corners of the face,
    as a QVector2DArray.  The individual corners can be indexed with the
    QGLCubeFace::Corner enum to find each corner.

    \sa setTextureCoords()
*/

/*!
    \fn void QGLCubeFace::setTextureCoords(const QVector2DArray &array)

    Sets all the four texture coordinates of the corners of the face
    to the values in \a array.  The \a array is expected to be in the
    same order as specified by the QGLCubeFace::Corner enum.
*/

/*!
    \relates QGLCube

    Builds the geometry for \a cube within the specified
    display \a list.

    This operator specifies the positions, and 2D texture
    co-ordinates for all of the vertices that make up the cube.
    Normals will be calculated by the \a list, depending on its
    current section's smoothing setting.

    See the documentation for QGLCubeFace to determine the orientation
    of the faces with respect to texture coordinates.
*/
QGLBuilder& operator<<(QGLBuilder& list, const QGLCube& cube)
{
    QGeometryData op;

    QVector3DArray vrts = QVector3DArray::fromRawData(
            reinterpret_cast<const QVector3D *>(vertexData), vertexDataLen / 3);
    if (cube.size() != 1.0f)
        vrts.scale(cube.size());

    op.appendVertexArray(vrts);

    QVector2DArray texx = QVector2DArray::fromRawData(
            reinterpret_cast<const QVector2D *>(texCoordData), texCoordDataLen / 2);

    for (int i = 0; i < 6; ++i)
        op.appendTexCoordArray(texx);

    list.addQuads(op);
    return list;
}

/*!
    \relates QGLCubeFace

    Builds the geometry for \a face within the specified
    display \a list.

    This operator specifies the positions, normals, and 2D texture
    co-ordinates for all of the vertices that make up the cube face.
*/
QGLBuilder& operator<<(QGLBuilder& list, const QGLCubeFace& face)
{
    QGeometryData op;

    QVector3DArray vrts = QVector3DArray::fromRawData(
            reinterpret_cast<const QVector3D *>
                (vertexData + face.face() * 4 * 3), 4);
    if (face.size() != 1.0f)
        vrts.scale(face.size());

    op.appendVertexArray(vrts);

    op.appendTexCoordArray(face.textureCoords());

    list.addQuads(op);
    return list;
}

QT_END_NAMESPACE
