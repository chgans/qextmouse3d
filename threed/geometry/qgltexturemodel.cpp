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

#include "qgltexturemodel.h"

/*!
    \class QGLTextureModel
    \brief The QGLTextureModel class manages incremental texture geometry.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    The QGLTextureModel class encapsulates texture coordinates for the
    purpose of applying them to geometry.

    The texture coordinates can be set on an individual basis for all four
    corners of the texture-mapping.  By default the coordinates are all zero,
    and the specifier is null - that is isNull() returns true.

    In the simplest case the entire texture is applied to a quad:
    \code
    // origin 0, 0 and width x height of 1 x 1
    QGLTextureModel texSpec(0, 0, 1, 1);
    myDisplayList.appendQuad(a, b, c, d, texSpec);
    \endcode

    To apply a sequence of texture coordinate sets, where the values of the
    coordinates are interpolated and tiled across incrementally, over
    the faces being textured, use startTileRight() and tileRight().  Here
    two faces \c{a-b-c-d} and \c{b-e-f-c} are adjacent, sharing edge \c{b-c}.
    The first faces width is 1, and the second faces width is 2.  The
    texture will be split and tiled across the faces 1/3 and 2/3.
    \code
    // origin 0, 0 and width x height of 1 x 1
    QGLTextureModel texSpec(0, 0, 1, 1);
    texSpec.startTileRight(3.0f);
    myDisplayList.appendQuad(a, b, c, d, texSpec.tileRight(2.0f));
    myDisplayList.appendQuad(b, e, f, c, texSpec.tileRight(1.0f));
    \endcode
*/

/*!
    Construct a null texture specifier.
*/
QGLTextureModel::QGLTextureModel()
    : m_bl(QLogicalVertex::InvalidTexCoord)
    , m_br(QLogicalVertex::InvalidTexCoord)
    , m_tr(QLogicalVertex::InvalidTexCoord)
    , m_tl(QLogicalVertex::InvalidTexCoord)
    , m_divisor(1.0f)
    , m_tile(0)
{
}

/*!
    Construct a rectangular QGLTextureModel with \a left and \a bottom origin
    coordinates, and the other coordinates determined by the \a width and \a height.
    By default all values are 0, making a null texture specifier.
*/
QGLTextureModel::QGLTextureModel(qreal left, qreal bottom,
                                                qreal width, qreal height)
    : m_bl(left, bottom)
    , m_br(left + width, bottom)
    , m_tr(left + width, bottom + height)
    , m_tl(left, bottom + height)
    , m_divisor(1.0f)
    , m_tile(0)
{
}

/*!
    Delete this texture specifier, recovering any resources.
*/
QGLTextureModel::~QGLTextureModel()
{
    delete m_tile;
}

/*!
    \fn QVector2D QGLTextureModel::bottomLeft() const
    Returns the texture coordinate for the bottom left of this texture
    specifier.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn void QGLTextureModel::setBottomLeft(const QVector2D &v)
    Sets the texture coordinate for the bottom left of this texture
    specifier to \a v.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn QVector2D QGLTextureModel::bottomRight() const
    Returns the texture coordinate for the bottom left of this texture
    specifier.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn void QGLTextureModel::setBottomRight(const QVector2D &v)
    Sets the texture coordinate for the bottom right of this texture
    specifier to \a v.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn QVector2D QGLTextureModel::topLeft() const
    Returns the texture coordinate for the top left of this texture
    specifier.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn QGLTextureModel::setTopLeft(const QVector2D &v)
    Sets the texture coordinate for the top left of this texture
    specifier to \a v.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn QVector2D QGLTextureModel::topRight() const
    Returns the texture coordinate for the top right of this texture
    specifier.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn void QGLTextureModel::setTopRight(const QVector2D &v)
    Sets the texture coordinate for the top right of this texture
    specifier to \a v.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn bool QGLTextureModel::isNull() const
    Returns true if this texture specifier is null, that is all its corners
    are set to QLogicalVertex::InvalidTexCoord.
*/

/*!
    \fn void QGLTextureModel::startTileRight(qreal divisor)
    Reset this QGLTextureModel ready for a tiling of texture divisions.
    The total width of the texture will be divided over \a divisor.  The
    default value of divisor is 1.0f.  The first tile generated by
    tileRight() will have left-bottom and -top coordinates equal to this
    specifier.  The last tile generated will have right-bottom and -top
    coordinates equal to this specifier.  The intervening tiles will be
    interpolated according to the divisions given to tileRight() but
    divided by \a divisor.
    \sa tileRight()
*/

/*!
    \fn QGLTextureModel QGLTextureModel::tileRight(qreal extent)
    const Returns a texture specifier for a tile \a extent wide.  The
    specifier returned will respectively have top-left and bottom-left
    equal to the top-right and bottom-right of the previously returned
    tiles; while the top-right and and bottom-right are advanced by the
    \a extent along an interpolated path from this specifier corners.
*/
