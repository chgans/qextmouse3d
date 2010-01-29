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

#include "qgldepthbufferoptions.h"
#include "qglvertexarray.h"
#include "qglpainter.h"
#include <QtCore/qatomic.h>
#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLDepthBufferOptions
    \since 4.6
    \brief The QGLDepthBufferOptions class stores values that describe the configuration of a GL depth buffer.
    \ingroup qt3d
    \ingroup qt3d::painting

    QGLDepthBufferOptions is typically used to build a depth buffer
    configuration at startup time that is applied when the application
    wants to change the actual GL depth buffer.

    Options are applied to a QGLPainter using apply().  The following
    example applies a normal depth buffer configuration with less-than
    testing:

    \code
    QGLPainter painter;

    QGLDepthBufferOptions options;
    options.setEnabled(true);
    options.setFunction(QGLDepthBufferOptions::Less);
    options.apply(&painter);
    \endcode

    The standard GL defaults are depth testing off, less-than testing
    function, depth buffer writing enabled, and clipping planes 0 and 1.
    These defaults can be explicitly set on a QGLPainter as follows:

    \code
    QGLDepthBufferOptions().apply(&painter);
    \endcode

    \sa QGLPainter
*/

/*!
    \enum QGLDepthBufferOptions::Function
    This enum defines the depth test function to apply to a GL context.

    \value Never Depth test never passes.
    \value Less Depth test passes if the incoming depth value is less
           than the stored depth value.  This is the default.
    \value Equal Depth test passes if the incoming depth value is equal
           to the stored depth value.
    \value LessOrEqual Depth test passes if the incoming depth value is
           less than or equal to the stored depth value.
    \value Greater Depth test passes if the incoming depth value is greater
           than the stored depth value.
    \value NotEqual Depth test passes if the incoming depth value is not
           equal to the stored depth value.
    \value GreaterOrEqual Depth test passes if the incoming depth value
           is greater than or equal to the stored depth value.
    \value Always Depth test always passes.
*/

class QGLDepthBufferOptionsPrivate
{
public:
    QGLDepthBufferOptionsPrivate()
    {
        ref = 1;
        isEnabled = false;
        isWriteEnabled = true;
        function = QGLDepthBufferOptions::Less;
        nearRange = 0.0f;
        farRange = 1.0f;
    }
    QGLDepthBufferOptionsPrivate(const QGLDepthBufferOptionsPrivate *other)
    {
        ref = 1;
        isEnabled = other->isEnabled;
        isWriteEnabled = other->isWriteEnabled;
        function = other->function;
        nearRange = other->nearRange;
        farRange = other->farRange;
    }

    inline bool equal(const QGLDepthBufferOptionsPrivate *other) const
    {
        return isEnabled == other->isEnabled &&
               isWriteEnabled == other->isWriteEnabled &&
               function == other->function &&
               nearRange == other->nearRange &&
               farRange == other->farRange;
    }

    inline bool isDefault() const
    {
        return !isEnabled && isWriteEnabled &&
               function == QGLDepthBufferOptions::Less &&
               nearRange == 0.0f && farRange == 1.0f;
    }

    QBasicAtomicInt ref;
    bool isEnabled;
    bool isWriteEnabled;
    QGLDepthBufferOptions::Function function;
    qreal nearRange;
    qreal farRange;
};

/*!
    Constructs a null depth buffer options object.  All properties will
    have their default values.

    \sa isNull()
*/
QGLDepthBufferOptions::QGLDepthBufferOptions()
{
    d = 0;
}

/*!
    Constructs a copy of \a other.
*/
QGLDepthBufferOptions::QGLDepthBufferOptions(const QGLDepthBufferOptions& other)
{
    d = other.d;
    if (d)
        d->ref.ref();
}

/*!
    Destroys this depth buffer options object.
*/
QGLDepthBufferOptions::~QGLDepthBufferOptions()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Assigns the contents of \a other to this object.
*/
QGLDepthBufferOptions& QGLDepthBufferOptions::operator=
    (const QGLDepthBufferOptions& other)
{
    if (d != other.d) {
        if (d && !d->ref.deref())
            delete d;
        d = other.d;
        if (d)
            d->ref.ref();
    }
    return *this;
}

QGLDepthBufferOptionsPrivate *QGLDepthBufferOptions::dwrite()
{
    if (!d) {
        d = new QGLDepthBufferOptionsPrivate();
    } else if (d->ref != 1) {
        QGLDepthBufferOptionsPrivate *newd =
            new QGLDepthBufferOptionsPrivate(d);
        d->ref.deref();
        d = newd;
    }
    return d;
}

/*!
    \fn bool QGLDepthBufferOptions::isNull() const

    Returns true if this depth buffer options object is null; false otherwise.
    A depth buffer options object becomes non-null the first time one of
    the set methods is called on a property.
*/

/*!
    Returns true if depth testing is enabled; false otherwise.
    The default value is false.

    \sa setEnabled(), function()
*/
bool QGLDepthBufferOptions::isEnabled() const
{
    if (d)
        return d->isEnabled;
    else
        return false;
}

/*!
    Enables or disables depth testing according to \a value.

    \sa isEnabled(), setFunction()
*/
void QGLDepthBufferOptions::setEnabled(bool value)
{
    dwrite()->isEnabled = value;
}

/*!
    Returns true if writing to the depth buffer is enabled; false otherwise.
    The default value is true.

    \sa setWriteEnabled()
*/
bool QGLDepthBufferOptions::isWriteEnabled() const
{
    if (d)
        return d->isWriteEnabled;
    else
        return true;
}

/*!
    Enables or disables writing to the depth buffer according to \a value.

    \sa isWriteEnabled()
*/
void QGLDepthBufferOptions::setWriteEnabled(bool value)
{
    dwrite()->isWriteEnabled = value;
}

/*!
    Returns the depth testing function.  The default is Less.
    Depth testing according to this function will only be performed if
    isEnabled() returns true.

    \sa setFunction(), isEnabled()
*/
QGLDepthBufferOptions::Function QGLDepthBufferOptions::function() const
{
    if (d)
        return d->function;
    else
        return QGLDepthBufferOptions::Less;
}

/*!
    Sets the depth testing function to \a value.  Depth testing according
    to \a value will only be performed if isEnabled() returns true.

    \sa function(), isEnabled()
*/
void QGLDepthBufferOptions::setFunction(QGLDepthBufferOptions::Function value)
{
    dwrite()->function = value;
}

/*!
    Returns the mapping of the near clipping plane to window
    co-ordinates.  The default value is 0.

    \sa farRange(), setRange()
*/
qreal QGLDepthBufferOptions::nearRange() const
{
    if (d)
        return d->nearRange;
    else
        return 0.0f;
}

/*!
    Returns the mapping of the far clipping plane to window
    co-ordinates.  The default value is 1.

    \sa nearRange(), setRange()
*/
qreal QGLDepthBufferOptions::farRange() const
{
    if (d)
        return d->farRange;
    else
        return 1.0f;
}

/*!
    Sets the depth values for mapping the near and far clipping
    values to window co-ordinates \a nearRange and \a farRange respectively.

    \sa nearRange(), farRange()
*/
void QGLDepthBufferOptions::setRange(qreal nearRange, qreal farRange)
{
    dwrite()->nearRange = nearRange;
    dwrite()->farRange = farRange;
}

/*!
    Returns true if this depth buffer options object contains the
    same values as \a other; false otherwise.

    \sa operator!=()
*/
bool QGLDepthBufferOptions::operator==(const QGLDepthBufferOptions& other) const
{
    if (d == other.d)
        return true;
    else if (!d && !other.d)
        return true;
    else if (!d)
        return other.d->isDefault();
    else if (!other.d)
        return d->isDefault();
    else
        return d->equal(other.d);
}

/*!
    Returns true if this depth buffer options object does not contain the
    same values as \a other; false otherwise.

    \sa operator==()
*/
bool QGLDepthBufferOptions::operator!=(const QGLDepthBufferOptions& other) const
{
    if (d == other.d)
        return false;
    else if (!d && !other.d)
        return false;
    else if (!d)
        return !other.d->isDefault();
    else if (!other.d)
        return !d->isDefault();
    else
        return !d->equal(other.d);
}

/*!
    Applies the depth buffer options in this object to \a painter.
    This will reconfigure the actual depth buffer to match the
    application's requirements.
*/
void QGLDepthBufferOptions::apply(QGLPainter *painter) const
{
    Q_UNUSED(painter);  // Maybe use this to cache previous settings.
    if (d) {
        if (d->isEnabled)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        glDepthFunc((GLenum)(d->function));
        glDepthMask(d->isWriteEnabled);
#if defined(QT_OPENGL_ES)
        glDepthRangef(d->nearRange, d->farRange);
#else
        glDepthRange(d->nearRange, d->farRange);
#endif
    } else {
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);
#if defined(QT_OPENGL_ES)
        glDepthRangef(0.0f, 1.0f);
#else
        glDepthRange(0.0f, 1.0f);
#endif
    }
}

QT_END_NAMESPACE
