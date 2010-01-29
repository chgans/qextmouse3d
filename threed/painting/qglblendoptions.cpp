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

#include "qglblendoptions.h"
#include "qglpainter_p.h"
#include <QtCore/qatomic.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLBlendOptions
    \since 4.7
    \brief The QGLBlendOptions class stores values that describe the blending modes.
    \ingroup qt3d
    \ingroup qt3d::painting

    QGLBlendOptions is typically used to build a blend configuration at
    startup time that is applied when the application wants to change
    the actual blending modes.  Options are applied to a QGLPainter
    using apply().

    \sa QGLPainter
*/

/*!
    \enum QGLBlendOptions::BlendFactor
    This enum defines a blending factor to use with QGLBlendOptions.

    \value Zero Blending factor is zero in all components.
    \value One Blending factor is one in all components.
    \value SrcColor Blending factor is defined by the source color.
    \value OneMinusSrcColor Blending factor is defined by 1 minus the
           source color.
    \value SrcAlpha Blending factor is defined by the source alpha.
    \value OneMinusSrcAlpha Blending factor is defined by 1 minus the
           source alpha.
    \value DstAlpha Blending factor is defined by the destination alpha.
    \value OneMinusDstAlpha Blending factor is defined by 1 minus the
           destination alpha.
    \value DstColor Blending factor is defined by the destination color.
    \value OneMinusDstColor Blending factor is defined by 1 minus the
           destination color.
    \value SrcAlphaSaturate Blending factor is the minimum of the
           source alpha and 1 minus the destination alpha.  If specified
           as QGLBlendOptions::sourceAlphaFactor(), SrcAlphaSaturate
           is the same as One.  This value cannot be specified as a
           destination blending factor.
    \value ConstantColor Blending factor is defined by
           QGLBlendOptions::blendColor().  Not supported under OpenGL/ES 1.x.
    \value OneMinusConstantColor Blending factor is defined by 1 minus
           QGLBlendOptions::blendColor().  Not supported under OpenGL/ES 1.x.
    \value ConstantAlpha Blending factor is defined by the alpha
           component of QGLBlendOptions::blendColor().  Not supported under
           OpenGL/ES 1.x.
    \value OneMinusConstantAlpha Blending factor is defined by 1 minus
           the alpha component of QGLBlendOptions::blendColor().
           Not supported under OpenGL/ES 1.x.
*/

/*!
    \enum QGLBlendOptions::BlendEquation
    This enum defines how source and destination pixels are blended.

    \value Add Adds the source and destination colors together.
    \value Minimum Minimum of the source and destination colors.
           This value is not supported under OpenGL/ES.
    \value Maximum Maximum of the source and destination colors.
           This value is not supported under OpenGL/ES.
    \value Subtract Subtracts the destination color from the source color.
    \value ReverseSubtract Subtracts the source color from the
           destination color.
*/

class QGLBlendOptionsPrivate
{
public:
    QGLBlendOptionsPrivate()
        : blendColor(0, 0, 0, 0)
    {
        ref = 1;
        isEnabled = false;
        sourceColorFactor = QGLBlendOptions::One;
        sourceAlphaFactor = QGLBlendOptions::One;
        destinationColorFactor = QGLBlendOptions::Zero;
        destinationAlphaFactor = QGLBlendOptions::Zero;
        colorEquation = QGLBlendOptions::Add;
        alphaEquation = QGLBlendOptions::Add;
    }
    QGLBlendOptionsPrivate(const QGLBlendOptionsPrivate *other)
        : blendColor(other->blendColor)
    {
        ref = 1;
        isEnabled = other->isEnabled;
        sourceColorFactor = other->sourceColorFactor;
        sourceAlphaFactor = other->sourceAlphaFactor;
        destinationColorFactor = other->destinationColorFactor;
        destinationAlphaFactor = other->destinationAlphaFactor;
        colorEquation = other->colorEquation;
        alphaEquation = other->alphaEquation;
    }

    inline bool equal(const QGLBlendOptionsPrivate *other) const
    {
        return isEnabled == other->isEnabled &&
               blendColor == other->blendColor &&
               sourceColorFactor == other->sourceColorFactor &&
               sourceAlphaFactor == other->sourceAlphaFactor &&
               destinationColorFactor == other->destinationColorFactor &&
               destinationAlphaFactor == other->destinationAlphaFactor &&
               colorEquation == other->colorEquation &&
               alphaEquation == other->alphaEquation;
    }

    inline bool isDefault() const
    {
        return isEnabled == false &&
               blendColor.red() == 0 &&
               blendColor.green() == 0 &&
               blendColor.blue() == 0 &&
               blendColor.alpha() == 0 &&
               sourceColorFactor == QGLBlendOptions::One &&
               sourceAlphaFactor == QGLBlendOptions::One &&
               destinationColorFactor == QGLBlendOptions::Zero &&
               destinationAlphaFactor == QGLBlendOptions::Zero &&
               colorEquation == QGLBlendOptions::Add &&
               alphaEquation == QGLBlendOptions::Add;
    }

    QBasicAtomicInt ref;
    bool isEnabled;
    QColor blendColor;
    QGLBlendOptions::BlendFactor sourceColorFactor;
    QGLBlendOptions::BlendFactor sourceAlphaFactor;
    QGLBlendOptions::BlendFactor destinationColorFactor;
    QGLBlendOptions::BlendFactor destinationAlphaFactor;
    QGLBlendOptions::BlendEquation colorEquation;
    QGLBlendOptions::BlendEquation alphaEquation;
};

/*!
    Constructs a null blend options object.  All properties will
    have their default values.

    \sa isNull()
*/
QGLBlendOptions::QGLBlendOptions()
{
    d = 0;
}

/*!
    Constructs a copy of \a other.
*/
QGLBlendOptions::QGLBlendOptions(const QGLBlendOptions& other)
{
    d = other.d;
    if (d)
        d->ref.ref();
}

/*!
    Destroys this blend options object.
*/
QGLBlendOptions::~QGLBlendOptions()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Assigns the contents of \a other to this object.
*/
QGLBlendOptions& QGLBlendOptions::operator=(const QGLBlendOptions& other)
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

QGLBlendOptionsPrivate *QGLBlendOptions::dwrite()
{
    if (!d) {
        d = new QGLBlendOptionsPrivate();
    } else if (d->ref != 1) {
        QGLBlendOptionsPrivate *newd =
            new QGLBlendOptionsPrivate(d);
        d->ref.deref();
        d = newd;
    }
    return d;
}

/*!
    \fn bool QGLBlendOptions::isNull() const

    Returns true if this blend options object is null; false otherwise.
    A blend options object becomes non-null the first time one of
    the set methods is called on a property.
*/

/*!
    Returns true if blending is enabled; false otherwise.
    The default value is false.

    \sa setEnabled()
*/
bool QGLBlendOptions::isEnabled() const
{
    if (d)
        return d->isEnabled;
    else
        return false;
}

/*!
    Enables or disables blending according to \a value.

    \sa isEnabled()
*/
void QGLBlendOptions::setEnabled(bool value)
{
    dwrite()->isEnabled = value;
}

/*!
    Returns the blending color.  The default value is (0, 0, 0, 0).

    Blending colors are only supported in desktop OpenGL 1.2
    and higher, or OpenGL/ES 2.0 and higher.

    \sa setBlendColor()
*/
QColor QGLBlendOptions::blendColor() const
{
    if (d)
        return d->blendColor;
    else
        return QColor(0, 0, 0, 0);
}

/*!
    Sets the blending color to \a value.

    Blending colors are only supported in desktop OpenGL 1.2
    and higher, or OpenGL/ES 2.0 and higher.

    \sa blendColor()
*/
void QGLBlendOptions::setBlendColor(const QColor& value)
{
    dwrite()->blendColor = value;
}

/*!
    Returns the source blending factor for RGB color components.
    The default value is One.

    \sa setSourceColorFactor(), sourceAlphaFactor()
    \sa destinationColorFactor()
*/
QGLBlendOptions::BlendFactor QGLBlendOptions::sourceColorFactor() const
{
    if (d)
        return d->sourceColorFactor;
    else
        return One;
}

/*!
    Sets the source blending factor for RGB color components to \a value.

    \sa sourceColorFactor(), setSourceAlphaFactor()
    \sa setDestinationColorFactor()
*/
void QGLBlendOptions::setSourceColorFactor(QGLBlendOptions::BlendFactor value)
{
    dwrite()->sourceColorFactor = value;
}

/*!
    Returns the source blending factor for alpha color components.
    The default value is One.

    Source alpha blending factors are only supported in desktop OpenGL 1.4
    and higher, or OpenGL/ES 2.0 and higher.  Under other versions,
    sourceColorFactor() will be used instead.

    \sa setSourceAlphaFactor(), sourceColorFactor()
    \sa destinationAlphaFactor()
*/
QGLBlendOptions::BlendFactor QGLBlendOptions::sourceAlphaFactor() const
{
    if (d)
        return d->sourceAlphaFactor;
    else
        return One;
}

/*!
    Sets the source blending factor for alpha color components to \a value.

    Source alpha blending factors are only supported in desktop OpenGL 1.4
    and higher, or OpenGL/ES 2.0 and higher.  Under other versions,
    sourceColorFactor() will be used instead.

    \sa sourceAlphaFactor(), setSourceColorFactor()
    \sa setDestinationAlphaFactor()
*/
void QGLBlendOptions::setSourceAlphaFactor(QGLBlendOptions::BlendFactor value)
{
    dwrite()->sourceAlphaFactor = value;
}

/*!
    Returns the destination blending factor for RGB color components.
    The default value is Zero.

    \sa setDestinationColorFactor(), destinationAlphaFactor()
    \sa sourceColorFactor()
*/
QGLBlendOptions::BlendFactor QGLBlendOptions::destinationColorFactor() const
{
    if (d)
        return d->destinationColorFactor;
    else
        return Zero;
}

/*!
    Sets the destination blending factor for RGB color components to \a value.

    \sa destinationColorFactor(), setDestinationAlphaFactor()
    \sa setSourceColorFactor()
*/
void QGLBlendOptions::setDestinationColorFactor(QGLBlendOptions::BlendFactor value)
{
    dwrite()->destinationColorFactor = value;
}

/*!
    Returns the destination blending factor for alpha color components.
    The default value is Zero.

    Destination alpha blending factors are only supported in desktop
    OpenGL 1.4 and higher, or OpenGL/ES 2.0 and higher.  Under other versions,
    destinationColorFactor() will be used instead.

    \sa setDestinationAlphaFactor(), destinationColorFactor()
    \sa sourceAlphaFactor()
*/
QGLBlendOptions::BlendFactor QGLBlendOptions::destinationAlphaFactor() const
{
    if (d)
        return d->destinationAlphaFactor;
    else
        return Zero;
}

/*!
    Sets the destination blending factor for alpha color components
    to \a value.

    Destination alpha blending factors are only supported in desktop
    OpenGL 1.4 and higher, or OpenGL/ES 2.0 and higher.  Under other versions,
    destinationColorFactor() will be used instead.

    \sa destinationAlphaFactor(), setDestinationColorFactor()
    \sa setSourceAlphaFactor()
*/
void QGLBlendOptions::setDestinationAlphaFactor(QGLBlendOptions::BlendFactor value)
{
    dwrite()->destinationAlphaFactor = value;
}

/*!
    Returns the equation to use to combine source and destination
    RGB color components.  The default value is Add.

    Blending equations are only supported in desktop OpenGL 1.2
    and higher, or OpenGL/ES 2.0 and higher.

    \sa setColorEquation(), alphaEquation()
*/
QGLBlendOptions::BlendEquation QGLBlendOptions::colorEquation() const
{
    if (d)
        return d->colorEquation;
    else
        return Add;
}

/*!
    Sets the equation to use to combine source and destination
    RGB color components to \a value.

    Blending equations are only supported in desktop OpenGL 1.2
    and higher, or OpenGL/ES 2.0 and higher.

    \sa colorEquation(), setAlphaEquation()
*/
void QGLBlendOptions::setColorEquation(QGLBlendOptions::BlendEquation value)
{
    dwrite()->colorEquation = value;
}

/*!
    Returns the equation to use to combine source and destination
    alpha color components.  The default value is Add.

    Alpha blending equations are only supported in desktop
    OpenGL 2.0 and higher, or OpenGL/ES 2.0 and higher.
    In other desktop OpenGL versions, colorEquation() will
    be used instead.

    \sa setAlphaEquation(), colorEquation()
*/
QGLBlendOptions::BlendEquation QGLBlendOptions::alphaEquation() const
{
    if (d)
        return d->alphaEquation;
    else
        return Add;
}

/*!
    Sets the equation to use to combine source and destination
    alpha color components to \a value.

    Alpha blending equations are only supported in desktop
    OpenGL 2.0 and higher, or OpenGL/ES 2.0 and higher.
    In other desktop OpenGL versions, colorEquation() will
    be used instead.

    \sa alphaEquation(), setColorEquation()
*/
void QGLBlendOptions::setAlphaEquation(QGLBlendOptions::BlendEquation value)
{
    dwrite()->alphaEquation = value;
}

/*!
    Returns true if this blend options object contains the
    same values as \a other; false otherwise.

    \sa operator!=()
*/
bool QGLBlendOptions::operator==(const QGLBlendOptions& other) const
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
    Returns true if this blend options object does not contain the
    same values as \a other; false otherwise.

    \sa operator==()
*/
bool QGLBlendOptions::operator!=(const QGLBlendOptions& other) const
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

#if !defined(QT_OPENGL_ES)

static QGLPainterExtensions *resolveBlendExtensions(QGLPainterPrivate *pd)
{
    QGLPainterExtensions *extn = pd->extensions();
    if (!(extn->blendResolved)) {
        extn->blendResolved = true;
        if (!extn->blendColor) {
            extn->blendColor = (q_PFNGLBLENDCOLORPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendColorEXT"));
        }
        if (!extn->blendColor) {
            extn->blendColor = (q_PFNGLBLENDCOLORPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendColor"));
        }
        if (!extn->blendFuncSeparate) {
            extn->blendFuncSeparate = (q_PFNGLBLENDFUNCSEPARATEPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendFuncSeparateEXT"));
        }
        if (!extn->blendFuncSeparate) {
            extn->blendFuncSeparate = (q_PFNGLBLENDFUNCSEPARATEPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendFuncSeparate"));
        }
        if (!extn->blendEquation) {
            extn->blendEquation = (q_PFNGLBLENDEQUATIONPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendEquationEXT"));
        }
        if (!extn->blendEquation) {
            extn->blendEquation = (q_PFNGLBLENDEQUATIONPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendEquation"));
        }
        if (!extn->blendEquationSeparate) {
            extn->blendEquationSeparate = (q_PFNGLBLENDEQUATIONSEPARATEPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendEquationSeparateEXT"));
        }
        if (!extn->blendEquationSeparate) {
            extn->blendEquationSeparate = (q_PFNGLBLENDEQUATIONSEPARATEPROC)
                pd->context->getProcAddress
                    (QLatin1String("glBlendEquationSeparate"));
        }
    }
    return extn;
}

#endif

/*!
    Applies the blend options in this object to \a painter.
    This will reconfigure the actual blending modes to match the
    application's requirements.
*/
void QGLBlendOptions::apply(QGLPainter *painter) const
{
    Q_UNUSED(painter);
    if (d) {
        if (d->isEnabled)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);
#if defined(QT_OPENGL_ES_2)
        glBlendColor(d->blendColor.redF(), d->blendColor.greenF(),
                     d->blendColor.blueF(), d->blendColor.alphaF());
        glBlendFuncSeparate((GLenum)(d->sourceColorFactor),
                            (GLenum)(d->destinationColorFactor),
                            (GLenum)(d->sourceAlphaFactor),
                            (GLenum)(d->destinationAlphaFactor));
        glBlendEquationSeparate((GLenum)(d->colorEquation),
                                (GLenum)(d->alphaEquation));
#elif defined(QT_OPENGL_ES)
        glBlendFunc((GLenum)(d->sourceColorFactor),
                    (GLenum)(d->destinationColorFactor));
#else
        QGLPainterPrivate *pd = painter->d_ptr;
        if (!pd || !pd->context)
            return;
        QGLPainterExtensions *extn = resolveBlendExtensions(pd);
        if (extn->blendColor) {
            (*extn->blendColor)
                (d->blendColor.redF(), d->blendColor.greenF(),
                 d->blendColor.blueF(), d->blendColor.alphaF());
        }
        if (extn->blendFuncSeparate) {
            (*extn->blendFuncSeparate)
                ((GLenum)(d->sourceColorFactor),
                 (GLenum)(d->destinationColorFactor),
                 (GLenum)(d->sourceAlphaFactor),
                 (GLenum)(d->destinationAlphaFactor));
        } else {
            glBlendFunc((GLenum)(d->sourceColorFactor),
                        (GLenum)(d->destinationColorFactor));
        }
        if (extn->blendEquationSeparate) {
            (*extn->blendEquationSeparate)
                ((GLenum)(d->colorEquation),
                 (GLenum)(d->alphaEquation));
        } else if (extn->blendEquation) {
            (*extn->blendEquation)((GLenum)(d->colorEquation));
        }
#endif
    } else {
        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ZERO);
#if defined(QT_OPENGL_ES_2)
        glBlendEquation(GL_FUNC_ADD);
        glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
#elif !defined(QT_OPENGL_ES)
        QGLPainterPrivate *pd = painter->d_ptr;
        if (!pd || !pd->context)
            return;
        QGLPainterExtensions *extn = resolveBlendExtensions(pd);
        if (extn->blendEquation)
            (*extn->blendEquation)((GLenum)Add);
        if (extn->blendColor)
            (*extn->blendColor)(0.0f, 0.0f, 0.0f, 0.0f);
#endif
    }
}

QT_END_NAMESPACE
