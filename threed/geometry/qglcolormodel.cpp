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
** This file contains pre-release code and may not be dism_tributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Pum_blic License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Pum_blic License version 2.1 as pum_blished by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Pum_blic License version 2.1 requirements
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

#include "qglcolormodel.h"


/*!
    \class QGLColorModel
    \brief The QGLColorModel class manages flat color.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    The QGLColorModel class encapsulates flat color values for the
    purpose of applying them to geometry.

    The flat color values can be set on an individual basis for all four
    corners of a target quad.  By default the values are all default QColor4b()
    values.

    In the simplest case the entire color model is applied to a quad:
    \code
    QGLColorModel colorSpec(Qt::red, Qt::blue, Qt::yellow, Qt::green);
    myDisplayList.appendQuad(a, b, c, d, QGLcolorModel(), colorSpec);
    \endcode

    To apply a sequence of color value sets, where the values of the
    coordinates are interpolated across incrementally, over
    the faces being colored, use startGradientRight() and gradientRight().  Here
    two faces \c{a-b-c-d} and \c{b-e-f-c} are adjacent, sharing edge \c{b-c}.
    The first faces width is 1, and the second faces width is 2.  The
    color will be interpolated across the faces 1/3 and 2/3.
    \code
    // origin 0, 0 and width x height of 1 x 1
    QGLColorModel colorSpec(0, 0, 1, 1);
    texSpec.startTileRight(3.0f);
    myDisplayList.appendQuad(a, b, c, d, colorSpec.tileRight(2.0f));
    myDisplayList.appendQuad(b, e, f, c, colorSpec.tileRight(1.0f));
    \endcode
*/

/*!
    Construct a null color specifier.
*/
QGLColorModel::QGLColorModel()
    : m_divisor(1.0f)
    , m_gradient(0)
{
}

/*!
    Construct a rectangular QGLColorModel with \a bottomLeft,
    \a bottomRight, \a topRight and \a topLeft color values.
*/
QGLColorModel::QGLColorModel(QColor4b bottomLeft, QColor4b bottomRight,
                             QColor4b topRight, QColor4b topLeft)
    : m_bl(bottomLeft)
    , m_br(bottomRight)
    , m_tr(topRight)
    , m_tl(topLeft)
    , m_divisor(1.0f)
    , m_gradient(0)
{
}

/*!
    \fn QGLColorModel::QGLColorModel(const QGLColorModel &other)
    Constructs a new color model as a copy of \a other.
*/

/*!
    Delete this color specifier, recovering any resources.
*/
QGLColorModel::~QGLColorModel()
{
    delete m_gradient;
}

/*!
    \fn QGLColorModel::operator=(const QGLColorModel &other)
    Assigns this color model to be a copy of \a other
*/

/*!
    \fn QColor4b QGLColorModel::bottomLeft() const
    Returns the color value for the bottom left of this color
    specifier.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn void QGLColorModel::setBottomLeft(const QColor4b &v)
    Sets the color value for the bottom left of this color
    specifier to \a v.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn QColor4b QGLColorModel::bottomRight() const
    Returns the color value for the bottom left of this color
    specifier.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn void QGLColorModel::setBottomRight(const QColor4b &v)
    Sets the color value for the bottom right of this color
    specifier to \a v.  Note that bottom is in the orientation as for OpenGL
    that is numerically lower values.
*/

/*!
    \fn QColor4b QGLColorModel::topLeft() const
    Returns the color value for the top left of this color
    specifier.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn QGLColorModel::setTopLeft(const QColor4b &v)
    Sets the color value for the top left of this color
    specifier to \a v.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn QColor4b QGLColorModel::topRight() const
    Returns the color value for the top right of this color
    specifier.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn void QGLColorModel::setTopRight(const QColor4b &v)
    Sets the color value for the top right of this color
    specifier to \a v.  Note that top is in the orientation as for OpenGL
    that is numerically higher values.
*/

/*!
    \fn void QGLColorModel::startGradientRight(qreal divisor)
    Reset this QGLColorModel ready for a grading of color values.
    The color values will be interpolated over \a divisor.  The
    default value of divisor is 1.0f.  The first values generated by
    gradientRight() will have left-bottom and -top values equal to this
    specifier.  The last gradient generated will have right-bottom and -top
    coordinates equal to this specifier.  The intervening tiles will be
    interpolated according to the divisions given to gradientRight() but
    divided by \a divisor.
    \sa tileRight()
*/

/*!
    \fn QGLColorModel QGLColorModel::gradientRight(qreal extent) const
    Returns a color specifier for a gradient \a extent wide.  The
    specifier returned will respectively have top-left and bottom-left
    equal to the top-right and bottom-right of the previously returned
    gradients; while the top-right and and bottom-right are advanced by the
    \a extent along an interpolated path from this specifiers corners.
*/
