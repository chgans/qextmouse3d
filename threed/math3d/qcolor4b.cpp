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

#include "qcolor4b.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QColor4B
    \brief The QColor4B class represents a color by four byte components.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::math

    OpenGL applications commonly use four byte values to compactly
    represent a color value.  QColor4B provides a convenience
    class for manipulating such compact color values.

    The alternative is to represent a color value as four floating-point
    values between 0.0 and 1.0.  The QVector4D class can be used for
    that purpose if required.
*/

/*!
    \fn QColor4B::QColor4B()

    Constructs a four-byte default color value of (0, 0, 0, 255).
*/

/*!
    \fn QColor4B::QColor4B(int red, int green, int blue, int alpha)

    Constructs a four-byte color value with the components \a red,
    \a green, \a blue, and \a alpha.
*/

/*!
    \fn QColor4B::QColor4B(const QColor& color)

    Constructs a four-byte color value from \a color.
*/

/*!
    \fn QColor4B::QColor4B(Qt::GlobalColor color)

    Constructs a four-byte color value from \a color.
*/

/*!
    \fn QColor4B::QColor4B(QRgb rgba)

    Constructs a four-byte color value from the red, green, blue, and
    alpha components of \a rgba.
*/

/*!
    \fn QColor4B& QColor4B::operator=(const QColor& color)

    Copies the red, green, blue, and alpha components of \a color
    into this object.
*/

/*!
    \fn QColor4B& QColor4B::operator=(Qt::GlobalColor color)

    Copies the red, green, blue, and alpha components of the
    specified global \a color name into this object.
*/

/*!
    \fn int QColor4B::red() const

    Returns the red component of this color, between 0 and 255.

    \sa green(), blue(), alpha(), setRed(), redF()
*/

/*!
    \fn int QColor4B::green() const

    Returns the green component of this color, between 0 and 255.

    \sa red(), blue(), alpha(), setGreen(), greenF()
*/

/*!
    \fn int QColor4B::blue() const

    Returns the blue component of this color, between 0 and 255.

    \sa red(), green(), alpha(), setBlue(), blueF()
*/

/*!
    \fn int QColor4B::alpha() const

    Returns the alpha component of this color, between 0 and 255.

    \sa red(), green(), blue(), setAlpha(), alphaF()
*/

/*!
    \fn void QColor4B::setRed(int value)

    Sets the red component of this color to \a value, between 0 and 255.

    \sa setGreen(), setBlue(), setAlpha(), red(), setRedF()
*/

/*!
    \fn void QColor4B::setGreen(int value)

    Sets the green component of this color to \a value, between 0 and 255.

    \sa setRed(), setBlue(), setAlpha(), green(), setGreenF()
*/

/*!
    \fn void QColor4B::setBlue(int value)

    Sets the blue component of this color to \a value, between 0 and 255.

    \sa setRed(), setGreen(), setAlpha(), blue(), setBlueF()
*/

/*!
    \fn void QColor4B::setAlpha(int value)

    Sets the alpha component of this color to \a value, between 0 and 255.

    \sa setRed(), setGreen(), setBlue(), alpha(), setAlphaF()
*/

/*!
    \fn qreal QColor4B::redF() const { return m_red / 255.0f; }

    Returns the red component of this color as a floating-point
    value between 0 and 1.

    \sa greenF(), blueF(), alphaF(), setRedF(), red()
*/

/*!
    \fn qreal QColor4B::greenF() const { return m_green / 255.0f; }

    Returns the green component of this color as a floating-point
    value between 0 and 1.

    \sa redF(), blueF(), alphaF(), setGreenF(), green()
*/

/*!
    \fn qreal QColor4B::blueF() const { return m_blue / 255.0f; }

    Returns the blue component of this color as a floating-point
    value between 0 and 1.

    \sa redF(), greenF(), alphaF(), setBlueF(), blue()
*/

/*!
    \fn qreal QColor4B::alphaF() const { return m_alpha / 255.0f; }

    Returns the alpha component of this color as a floating-point
    value between 0 and 1.

    \sa redF(), greenF(), blueF(), setAlphaF(), alpha()
*/

/*!
    \fn void QColor4B::setRedF(qreal value)

    Sets the red component of this color to a floating-point \a value,
    between 0 and 1.

    \sa setGreenF(), setBlueF(), setAlphaF(), redF(), setRed()
*/

/*!
    \fn void QColor4B::setGreenF(qreal value)

    Sets the green component of this color to a floating-point \a value,
    between 0 and 1.

    \sa setRedF(), setBlueF(), setAlphaF(), greenF(), setGreen()
*/

/*!
    \fn void QColor4B::setBlueF(qreal value)

    Sets the blue component of this color to a floating-point \a value,
    between 0 and 1.

    \sa setRedF(), setGreenF(), setAlphaF(), blueF(), setBlue()
*/

/*!
    \fn void QColor4B::setAlphaF(qreal value)

    Sets the alpha component of this color to a floating-point \a value,
    between 0 and 1.

    \sa setRedF(), setGreenF(), setBlueF(), alphaF(), setAlpha()
*/

/*!
    \fn void QColor4B::setRgb(int red, int green, int blue, int alpha)

    Sets the components of this color to \a red, \a green, \a blue,
    and \a alpha.  Each component is between 0 and 255.

    \sa setRgbF(), fromRgb()
*/

/*!
    \fn void QColor4B::setRgbF(qreal red, qreal green, qreal blue, qreal alpha)

    Sets the components of this color to \a red, \a green, \a blue,
    and \a alpha.  Each component is a floating-point value between 0 and 1.

    \sa setRgb(), fromRgbF()
*/

/*!
    \fn QColor4B QColor4B::fromRgb(int red, int green, int blue, int alpha)

    Returns a QColor4B with the components \a red, \a green, \a blue,
    and \a alpha.  Each component is between 0 and 255.

    \sa fromRgbF(), setRgb()
*/

/*!
    \fn QColor4B QColor4B::fromRgbF(qreal red, qreal green, qreal blue, qreal alpha)

    Returns a QColor4B with the components \a red, \a green, \a blue,
    and \a alpha.  Each component is a floating-point value between 0 and 1.

    \sa fromRgb(), setRgbF()
*/

/*!
    \fn QColor4B QColor4B::fromRaw(const uchar *data)

    Returns a QColor4B with components from the first four elements
    in \a data.  The \a data parameter must contain at least four
    elements and not be null.
*/

/*!
    \fn QColor QColor4B::toColor() const

    Returns this color as a QColor.
*/

/*!
    \fn bool QColor4B::operator==(const QColor4B& other) const

    Returns true if this color is the same as \a other; false otherwise.
*/

/*!
    \fn bool QColor4B::operator!=(const QColor4B& other) const

    Returns true if this color is not the same as \a other; false otherwise.
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QColor4B &color)
{
    dbg.nospace() << "QColor4B("
        << color.redF() << ", " << color.greenF() << ", "
        << color.blueF() << ", " << color.alphaF() << ')';
    return dbg.space();
}

#endif

QT_END_NAMESPACE
