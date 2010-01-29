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

#include "qglstencilbufferoptions.h"
#include "qglpainter_p.h"
#include <QtCore/qatomic.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLStencilBufferOptions
    \since 4.6
    \brief The QGLStencilBufferOptions class stores values that describe the configuration of a GL stencil buffer.
    \ingroup qt3d
    \ingroup qt3d::painting

    QGLStencilBufferOptions is typically used to build a stencil buffer
    configuration at startup time that is applied when the application
    wants to change the actual GL stencil buffer.

    Options are applied to a QGLPainter using apply().  The following
    example applies a stencil buffer configuration with less-than
    testing on both front and back faces:

    \code
    QGLPainter painter;

    QGLStencilBufferOptions options;
    options.setEnabled(true);
    options.setFunction(QGLStencilBufferOptions::Less);
    options.apply(&painter);
    \endcode

    The standard GL defaults can be explicitly set on a QGLPainter
    as follows:

    \code
    QGLStencilBufferOptions().apply(&painter);
    \endcode

    Stencil properties can be set independently on front and back faces.
    Older systems may not support separated stencil modes.  On such systems,
    the front face properties will be used on both faces.

    \sa QGLPainter
*/

/*!
    \enum QGLStencilBufferOptions::Function
    This enum defines the stencil test function to apply to a GL context.

    \value Never Stencil testing is enabled, but the test never passes.
    \value Less Stencil testing is enabled, and the test passes if the
           reference stencil value is less than the stored stencil value.
    \value Equal Stencil testing is enabled, and the test passes if the
           reference stencil value is equal to the stored stencil value.
    \value LessOrEqual Stencil testing is enabled, and the test passes
           if the reference stencil value is less than or equal to the stored
           stencil value.
    \value Greater Stencil testing is enabled, and the test passes if the
           reference stencil value is greater than the stored stencil value.
    \value NotEqual Stencil testing is enabled, and the test passes if the
           reference stencil value is not equal to the stored stencil value.
    \value GreaterOrEqual Stencil testing is enabled, and the test passes
           if the reference stencil value is greater than or equal to the
           stored stencil value.
    \value Always Stencil testing is enabled, and the test always passes.
*/

/*!
    \enum QGLStencilBufferOptions::Action
    This enum defines the action to take when a stencil test passes or fails.

    \value Zero Set the stencil buffer to zero.
    \value Keep Keep the current stencil buffer value.
    \value Replace Replace the stencil buffer value with the
           reference value.
    \value Increment Increment the stencil buffer value and clamp
           at the maximum value.
    \value Decrement Decrement the stencil buffer value and clamp
           at the minimum value.
    \value Invert Bitwise invert the stencil buffer value.
    \value IncrementWrap Increment the stencil buffer value and
           wrap around when the maximum value is exceeded.
    \value DecrementWrap Decrement the stencil buffer value and
           wrap around to the maximum value when the minimum is passed.
*/

class QGLStencilBufferOptionsPrivate
{
public:
    QGLStencilBufferOptionsPrivate()
    {
        ref = 1;
        isEnabled = false;
        sameFunctions = true;
        sameWriteMasks = true;
        sameActions = true;
        frontFunction = QGLStencilBufferOptions::Always;
        frontReferenceValue = 0;
        frontReferenceMask = -1;
        frontWriteMask = -1;
        frontStencilFailAction = QGLStencilBufferOptions::Keep;
        frontDepthFailAction = QGLStencilBufferOptions::Keep;
        frontPassAction = QGLStencilBufferOptions::Keep;
        backFunction = QGLStencilBufferOptions::Always;
        backReferenceValue = 0;
        backReferenceMask = -1;
        backWriteMask = -1;
        backStencilFailAction = QGLStencilBufferOptions::Keep;
        backDepthFailAction = QGLStencilBufferOptions::Keep;
        backPassAction = QGLStencilBufferOptions::Keep;
    }
    QGLStencilBufferOptionsPrivate(const QGLStencilBufferOptionsPrivate *other)
    {
        ref = 1;
        isEnabled = other->isEnabled;
        sameFunctions = other->sameFunctions;
        sameWriteMasks = other->sameWriteMasks;
        sameActions = other->sameActions;
        frontFunction = other->frontFunction;
        frontReferenceValue = other->frontReferenceValue;
        frontReferenceMask = other->frontReferenceMask;
        frontWriteMask = other->frontWriteMask;
        frontStencilFailAction = other->frontStencilFailAction;
        frontDepthFailAction = other->frontDepthFailAction;
        frontPassAction = other->frontPassAction;
        backFunction = other->backFunction;
        backReferenceValue = other->backReferenceValue;
        backReferenceMask = other->backReferenceMask;
        backWriteMask = other->backWriteMask;
        backStencilFailAction = other->backStencilFailAction;
        backDepthFailAction = other->backDepthFailAction;
        backPassAction = other->backPassAction;
    }

    inline bool equal(const QGLStencilBufferOptionsPrivate *other) const
    {
        return isEnabled == other->isEnabled &&
               frontFunction == other->frontFunction &&
               frontReferenceValue == other->frontReferenceValue &&
               frontReferenceMask == other->frontReferenceMask &&
               frontWriteMask == other->frontWriteMask &&
               frontStencilFailAction == other->frontStencilFailAction &&
               frontDepthFailAction == other->frontDepthFailAction &&
               frontPassAction == other->frontPassAction &&
               backFunction == other->backFunction &&
               backReferenceValue == other->backReferenceValue &&
               backReferenceMask == other->backReferenceMask &&
               backWriteMask == other->backWriteMask &&
               backStencilFailAction == other->backStencilFailAction &&
               backDepthFailAction == other->backDepthFailAction &&
               backPassAction == other->backPassAction;
    }

    inline bool isDefault() const
    {
        return !isEnabled &&
               frontFunction == QGLStencilBufferOptions::Always &&
               frontReferenceValue == 0 &&
               frontReferenceMask == -1 &&
               frontWriteMask == -1 &&
               frontStencilFailAction == QGLStencilBufferOptions::Keep &&
               frontDepthFailAction == QGLStencilBufferOptions::Keep &&
               frontPassAction == QGLStencilBufferOptions::Keep &&
               backFunction == QGLStencilBufferOptions::Always &&
               backReferenceValue == 0 &&
               backReferenceMask == -1 &&
               backWriteMask == -1 &&
               backStencilFailAction == QGLStencilBufferOptions::Keep &&
               backDepthFailAction == QGLStencilBufferOptions::Keep &&
               backPassAction == QGLStencilBufferOptions::Keep;
    }

    void recomputeSameFunctions()
    {
        sameFunctions = (frontFunction == backFunction &&
                         frontReferenceValue == backReferenceValue &&
                         frontReferenceMask == backReferenceMask);
    }

    void recomputeSameWriteMasks()
    {
        sameWriteMasks = (frontWriteMask == backWriteMask);
    }

    void recomputeSameActions()
    {
        sameActions = (frontStencilFailAction == backStencilFailAction &&
                       frontDepthFailAction == backDepthFailAction &&
                       frontPassAction == backPassAction);
    }

    QBasicAtomicInt ref;
    bool isEnabled;
    bool sameFunctions;
    bool sameWriteMasks;
    bool sameActions;
    QGLStencilBufferOptions::Function frontFunction;
    int frontReferenceValue;
    int frontReferenceMask;
    int frontWriteMask;
    QGLStencilBufferOptions::Action frontStencilFailAction;
    QGLStencilBufferOptions::Action frontDepthFailAction;
    QGLStencilBufferOptions::Action frontPassAction;
    QGLStencilBufferOptions::Function backFunction;
    int backReferenceValue;
    int backReferenceMask;
    int backWriteMask;
    QGLStencilBufferOptions::Action backStencilFailAction;
    QGLStencilBufferOptions::Action backDepthFailAction;
    QGLStencilBufferOptions::Action backPassAction;
};

/*!
    Constructs a null stencil buffer options object.  All properties will
    have their default values.

    \sa isNull()
*/
QGLStencilBufferOptions::QGLStencilBufferOptions()
{
    d = 0;
}

/*!
    Constructs a copy of \a other.
*/
QGLStencilBufferOptions::QGLStencilBufferOptions
    (const QGLStencilBufferOptions& other)
{
    d = other.d;
    d->ref.ref();
}

/*!
    Destroys this stencil buffer options object.
*/
QGLStencilBufferOptions::~QGLStencilBufferOptions()
{
    if (d && !d->ref.deref())
        delete d;
}

/*!
    Assigns the contents of \a other to this object.
*/
QGLStencilBufferOptions& QGLStencilBufferOptions::operator=
        (const QGLStencilBufferOptions& other)
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

QGLStencilBufferOptionsPrivate *QGLStencilBufferOptions::dwrite()
{
    if (!d) {
        d = new QGLStencilBufferOptionsPrivate();
    } else if (d->ref != 1) {
        QGLStencilBufferOptionsPrivate *newd =
            new QGLStencilBufferOptionsPrivate(d);
        d->ref.deref();
        d = newd;
    }
    return d;
}

/*!
    \fn bool QGLStencilBufferOptions::isNull() const

    Returns true if this stencil buffer options object is null;
    false otherwise.  A stencil buffer options object becomes
    non-null the first time one of the set methods is called
    on a property.
*/

/*!
    Returns true if stencil testing is enabled; false otherwise.
    The default value is false.

    \sa setEnabled(), frontFunction()
*/
bool QGLStencilBufferOptions::isEnabled() const
{
    if (d)
        return d->isEnabled;
    else
        return false;
}

/*!
    Enables or disables stencil testing according to \a value.

    \sa isEnabled(), setFrontFunction()
*/
void QGLStencilBufferOptions::setEnabled(bool value)
{
    dwrite()->isEnabled = value;
}

/*!
    Returns the stencil test function for front faces.  The default
    value is Always.

    \sa setFrontFunction(), backFunction()
*/
QGLStencilBufferOptions::Function QGLStencilBufferOptions::frontFunction() const
{
    if (d)
        return d->frontFunction;
    else
        return QGLStencilBufferOptions::Always;
}

/*!
    Sets the stencil test function for front faces to \a value.

    \sa frontFunction(), setBackFunction()
*/
void QGLStencilBufferOptions::setFrontFunction
        (QGLStencilBufferOptions::Function value)
{
    dwrite()->frontFunction = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Returns the reference value for the stencil test on front faces.
    The default value is 0.

    \sa setFrontReferenceValue(), frontReferenceMask(), backReferenceValue()
*/
int QGLStencilBufferOptions::frontReferenceValue() const
{
    if (d)
        return d->frontReferenceValue;
    else
        return 0;
}

/*!
    Sets the reference value for the stencil test on front faces to \a value.

    \sa frontReferenceValue(), setFrontReferenceMask(), setBackReferenceValue()
*/
void QGLStencilBufferOptions::setFrontReferenceValue(int value)
{
    dwrite()->frontReferenceValue = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Returns the reference mask for the stencil test on front faces.
    The default value is -1, indicating that all stencil bits should
    be considered relevant when performing stencil tests.

    \sa setFrontReferenceMask(), frontReferenceValue(), backReferenceMask()
*/
int QGLStencilBufferOptions::frontReferenceMask() const
{
    if (d)
        return d->frontReferenceMask;
    else
        return -1;
}

/*!
    Sets the reference mask for the stencil test on front faces to \a value.
    Only bits that are set to 1 in \a value are considered relevant
    when performing stencil tests.

    \sa frontReferenceMask(), setFrontReferenceValue(), setBackReferenceMask()
*/
void QGLStencilBufferOptions::setFrontReferenceMask(int value)
{
    dwrite()->frontReferenceMask = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Returns the write mask for stencil operations on front faces.
    The default value is -1, indicating that all stencil bits
    will be written.

    \sa setFrontWriteMask(), backWriteMask()
*/
int QGLStencilBufferOptions::frontWriteMask() const
{
    if (d)
        return d->frontWriteMask;
    else
        return -1;
}

/*!
    Sets the write mask for stencil operations on front faces to \a value.
    Only bits that are set to 1 in \a value are written.

    \sa frontWriteMask(), setBackWriteMask()
*/
void QGLStencilBufferOptions::setFrontWriteMask(int value)
{
    dwrite()->frontWriteMask = value;
    dwrite()->recomputeSameWriteMasks();
}

/*!
    Returns the action to take when the stencil test fails on
    front faces.  The default value is Keep.

    \sa setFrontStencilFailAction(), frontDepthFailAction()
    \sa frontPassAction(), backStencilFailAction()
*/
QGLStencilBufferOptions::Action QGLStencilBufferOptions::frontStencilFailAction() const
{
    if (d)
        return d->frontStencilFailAction;
    else
        return QGLStencilBufferOptions::Keep;
}

/*!
    Sets the action to take when the stencil test fails on front
    faces to \a value.

    \sa frontStencilFailAction(), setFrontDepthFailAction()
    \sa setFrontPassAction(), setBackStencilFailAction()
*/
void QGLStencilBufferOptions::setFrontStencilFailAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->frontStencilFailAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Returns the action to take when the stencil test passes on
    front faces but the depth test fails.  The default value is Keep.

    \sa setFrontDepthFailAction(), frontStencilFailAction()
    \sa frontPassAction(), backDepthFailAction()
*/
QGLStencilBufferOptions::Action QGLStencilBufferOptions::frontDepthFailAction() const
{
    if (d)
        return d->frontDepthFailAction;
    else
        return QGLStencilBufferOptions::Keep;
}

/*!
    Sets the action to take when the stencil test passes on
    front faces, but the depth test fails, to \a value.

    \sa frontDepthFailAction(), setFrontStencilFailAction()
    \sa setFrontPassAction(), setBackDepthFailAction()
*/
void QGLStencilBufferOptions::setFrontDepthFailAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->frontDepthFailAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Returns the action to take when both the stencil test and depth test
    passes on front faces.  The default value is Keep.

    \sa setFrontPassAction(), frontStencilFailAction()
    \sa frontDepthFailAction(), backPassAction()
*/
QGLStencilBufferOptions::Action QGLStencilBufferOptions::frontPassAction() const
{
    if (d)
        return d->frontPassAction;
    else
        return QGLStencilBufferOptions::Keep;
}

/*!
    Sets the action to take when both the stencil test and depth test
    passes on front faces to \a value.

    \sa frontPassAction(), setFrontStencilFailAction()
    \sa setFrontDepthFailAction(), setBackPassAction()
*/
void QGLStencilBufferOptions::setFrontPassAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->frontPassAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Returns the stencil test function for back faces.  The default
    value is Always.

    \sa setBackFunction(), frontFunction()
*/
QGLStencilBufferOptions::Function QGLStencilBufferOptions::backFunction() const
{
    if (d)
        return d->backFunction;
    else
        return QGLStencilBufferOptions::Always;
}

/*!
    Sets the stencil test function for back faces to \a value.

    \sa backFunction(), setFrontFunction()
*/
void QGLStencilBufferOptions::setBackFunction
        (QGLStencilBufferOptions::Function value)
{
    dwrite()->backFunction = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Returns the reference value for the stencil test on back faces.
    The default value is 0.

    \sa setBackReferenceValue(), backReferenceMask(), frontReferenceValue()
*/
int QGLStencilBufferOptions::backReferenceValue() const
{
    if (d)
        return d->backReferenceValue;
    else
        return 0;
}

/*!
    Sets the reference value for the stencil test on back faces to \a value.

    \sa backReferenceValue(), setBackReferenceMask(), setFrontReferenceValue()
*/
void QGLStencilBufferOptions::setBackReferenceValue(int value)
{
    dwrite()->backReferenceValue = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Returns the reference mask for the stencil test on back faces.
    The default value is -1, indicating that all stencil bits should
    be considered relevant when performing stencil tests.

    \sa setBackReferenceMask(), backReferenceValue(), frontReferenceMask()
*/
int QGLStencilBufferOptions::backReferenceMask() const
{
    if (d)
        return d->backReferenceMask;
    else
        return -1;
}

/*!
    Sets the reference mask for the stencil test on back faces to \a value.
    Only bits that are set to 1 in \a value are considered relevant
    when performing stencil tests.

    \sa backReferenceMask(), setBackReferenceValue(), setFrontReferenceMask()
*/
void QGLStencilBufferOptions::setBackReferenceMask(int value)
{
    dwrite()->backReferenceMask = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Returns the write mask for stencil operations on back faces.
    The default value is -1, indicating that all stencil bits
    will be written.

    \sa setBackWriteMask(), frontWriteMask()
*/
int QGLStencilBufferOptions::backWriteMask() const
{
    if (d)
        return d->backWriteMask;
    else
        return -1;
}

/*!
    Sets the write mask for stencil operations on back faces to \a value.
    Only bits that are set to 1 in \a value are written.

    \sa backWriteMask(), setFrontWriteMask()
*/
void QGLStencilBufferOptions::setBackWriteMask(int value)
{
    dwrite()->backWriteMask = value;
    dwrite()->recomputeSameWriteMasks();
}

/*!
    Returns the action to take when the stencil test fails on
    back faces.  The default value is Keep.

    \sa setBackStencilFailAction(), backDepthFailAction()
    \sa backPassAction(), frontStencilFailAction()
*/
QGLStencilBufferOptions::Action QGLStencilBufferOptions::backStencilFailAction() const
{
    if (d)
        return d->backStencilFailAction;
    else
        return QGLStencilBufferOptions::Keep;
}

/*!
    Sets the action to take when the stencil test fails on back
    faces to \a value.

    \sa backStencilFailAction(), setBackDepthFailAction()
    \sa setBackPassAction(), setFrontStencilFailAction()
*/
void QGLStencilBufferOptions::setBackStencilFailAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->backStencilFailAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Returns the action to take when the stencil test passes on
    back faces but the depth test fails.  The default value is Keep.

    \sa setBackDepthFailAction(), backStencilFailAction()
    \sa backPassAction(), frontDepthFailAction()
*/
QGLStencilBufferOptions::Action QGLStencilBufferOptions::backDepthFailAction() const
{
    if (d)
        return d->backDepthFailAction;
    else
        return QGLStencilBufferOptions::Keep;
}

/*!
    Sets the action to take when the stencil test passes on
    back faces, but the depth test fails, to \a value.

    \sa backDepthFailAction(), setBackStencilFailAction()
    \sa setBackPassAction(), setFrontDepthFailAction()
*/
void QGLStencilBufferOptions::setBackDepthFailAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->backDepthFailAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Returns the action to take when both the stencil test and depth test
    passes on back faces.  The default value is Keep.

    \sa setBackPassAction(), backStencilFailAction()
    \sa backDepthFailAction(), frontPassAction()
*/
QGLStencilBufferOptions::Action QGLStencilBufferOptions::backPassAction() const
{
    if (d)
        return d->backPassAction;
    else
        return QGLStencilBufferOptions::Keep;
}

/*!
    Sets the action to take when both the stencil test and depth test
    passes on back faces to \a value.

    \sa backPassAction(), setBackStencilFailAction()
    \sa setBackDepthFailAction(), setFrontPassAction()
*/
void QGLStencilBufferOptions::setBackPassAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->backPassAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Sets the stencil test function for both front and back faces
    to \a value.

    \sa setFrontFunction(), setBackFunction()
*/
void QGLStencilBufferOptions::setFunction
        (QGLStencilBufferOptions::Function value)
{
    dwrite()->frontFunction = value;
    dwrite()->backFunction = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Sets the reference value for the stencil test on both front and
    back faces to \a value.

    \sa setFrontReferenceValue(), setBackReferenceValue()
*/
void QGLStencilBufferOptions::setReferenceValue(int value)
{
    dwrite()->frontReferenceValue = value;
    dwrite()->backReferenceValue = value;
    dwrite()->recomputeSameFunctions();
}

/*!
    Sets the write mask for stencil operations on both front and back
    faces to \a value.  Only bits that are set to 1 in \a value are written.

    \sa setFrontWriteMask(), setBackWriteMask()
*/
void QGLStencilBufferOptions::setWriteMask(int value)
{
    dwrite()->frontWriteMask = value;
    dwrite()->backWriteMask = value;
    dwrite()->recomputeSameWriteMasks();
}

/*!
    Sets the action to take when the stencil test fails on front
    and back faces to \a value.

    \sa setFrontStencilFailAction(), setBackStencilFailAction()
*/
void QGLStencilBufferOptions::setStencilFailAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->frontStencilFailAction = value;
    dwrite()->backStencilFailAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Sets the action to take when the stencil test passes on
    front and back faces, but the depth test fails, to \a value.

    \sa setFrontDepthFailAction(), setBackDepthFailAction()
*/
void QGLStencilBufferOptions::setDepthFailAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->frontDepthFailAction = value;
    dwrite()->backDepthFailAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Sets the action to take when both the stencil test and depth test
    passes on front and back faces to \a value.

    \sa setFrontPassAction(), setBackPassAction()
*/
void QGLStencilBufferOptions::setPassAction
        (QGLStencilBufferOptions::Action value)
{
    dwrite()->frontPassAction = value;
    dwrite()->backPassAction = value;
    dwrite()->recomputeSameActions();
}

/*!
    Returns true if this stencil buffer options object contains the
    same values as \a other; false otherwise.

    \sa operator!=()
*/
bool QGLStencilBufferOptions::operator==
        (const QGLStencilBufferOptions& other) const
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
bool QGLStencilBufferOptions::operator!=
        (const QGLStencilBufferOptions& other) const
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

#ifndef GL_FRONT
#define GL_FRONT 0x0404
#endif
#ifndef GL_BACK
#define GL_BACK  0x0405
#endif
#ifndef GL_FRONT_AND_BACK
#define GL_FRONT_AND_BACK 0x0408
#endif

/*!
    Applies the stencil buffer options in this object to \a painter.
    This will reconfigure the actual stencil buffer to match the
    application's requirements.
*/
void QGLStencilBufferOptions::apply(QGLPainter *painter) const
{
    Q_UNUSED(painter);
    if (d) {
        if (d->isEnabled)
            glEnable(GL_STENCIL_TEST);
        else
            glDisable(GL_STENCIL_TEST);
#if defined(QT_OPENGL_ES_2)
        if (d->sameFunctions) {
            glStencilFuncSeparate(GL_FRONT_AND_BACK,
                                  (GLenum)(d->frontFunction),
                                  (GLint)(d->frontReferenceValue),
                                  (GLuint)(d->frontReferenceMask));
        } else {
            glStencilFuncSeparate(GL_FRONT, (GLenum)(d->frontFunction),
                                  (GLint)(d->frontReferenceValue),
                                  (GLuint)(d->frontReferenceMask));
            glStencilFuncSeparate(GL_BACK, (GLenum)(d->backFunction),
                                  (GLint)(d->backReferenceValue),
                                  (GLuint)(d->backReferenceMask));
        }
        if (d->sameWriteMasks) {
            glStencilMaskSeparate
                (GL_FRONT_AND_BACK, (GLuint)(d->frontWriteMask));
        } else {
            glStencilMaskSeparate(GL_FRONT, (GLuint)(d->frontWriteMask));
            glStencilMaskSeparate(GL_BACK, (GLuint)(d->backWriteMask));
        }
        if (d->sameActions) {
            glStencilOpSeparate(GL_FRONT_AND_BACK,
                                (GLenum)(d->frontStencilFailAction),
                                (GLenum)(d->frontDepthFailAction),
                                (GLenum)(d->frontPassAction));
        } else {
            glStencilOpSeparate(GL_FRONT, (GLenum)(d->frontStencilFailAction),
                                (GLenum)(d->frontDepthFailAction),
                                (GLenum)(d->frontPassAction));
            glStencilOpSeparate(GL_BACK, (GLenum)(d->backStencilFailAction),
                                (GLenum)(d->backDepthFailAction),
                                (GLenum)(d->backPassAction));
        }
#elif defined(QT_OPENGL_ES)
        glStencilFunc((GLenum)(d->frontFunction),
                      (GLint)(d->frontReferenceValue),
                      (GLuint)(d->frontReferenceMask));
        glStencilMask((GLuint)(d->frontWriteMask));
        glStencilOp((GLenum)(d->frontStencilFailAction),
                    (GLenum)(d->frontDepthFailAction),
                    (GLenum)(d->frontPassAction));
#else
        QGLPainterPrivate *pd = painter->d_ptr;
        if (!pd || !pd->context)
            return;
        QGLPainterExtensions *extn = pd->extensions();
        if (!(extn->stencilResolved)) {
            extn->stencilResolved = true;
            if (!extn->stencilFuncSeparate) {
                extn->stencilFuncSeparate = (q_PFNGLSTENCILFUNCSEPARATEPROC)
                    pd->context->getProcAddress
                        (QLatin1String("glStencilFuncSeparate"));
            }
            if (!extn->stencilMaskSeparate) {
                extn->stencilMaskSeparate = (q_PFNGLSTENCILMASKSEPARATEPROC)
                    pd->context->getProcAddress
                        (QLatin1String("glStencilMaskSeparate"));
            }
            if (!extn->stencilOpSeparate) {
                extn->stencilOpSeparate = (q_PFNGLSTENCILOPSEPARATEPROC)
                    pd->context->getProcAddress
                        (QLatin1String("glStencilOpSeparate"));
            }
        }
        if (extn->stencilFuncSeparate && !d->sameFunctions) {
            (*extn->stencilFuncSeparate)
                (GL_FRONT, (GLenum)(d->frontFunction),
                 (GLint)(d->frontReferenceValue),
                 (GLuint)(d->frontReferenceMask));
            (*extn->stencilFuncSeparate)
                (GL_BACK, (GLenum)(d->backFunction),
                 (GLint)(d->backReferenceValue),
                 (GLuint)(d->backReferenceMask));
        } else {
            glStencilFunc((GLenum)(d->frontFunction),
                          (GLint)(d->frontReferenceValue),
                          (GLuint)(d->frontReferenceMask));
        }
        if (extn->stencilMaskSeparate && !d->sameWriteMasks) {
            (*extn->stencilMaskSeparate)(GL_FRONT, (GLuint)(d->frontWriteMask));
            (*extn->stencilMaskSeparate)(GL_BACK, (GLuint)(d->backWriteMask));
        } else {
            glStencilMask((GLuint)(d->frontWriteMask));
        }
        if (extn->stencilOpSeparate && !d->sameActions) {
            (*extn->stencilOpSeparate)
                (GL_FRONT, (GLenum)(d->frontStencilFailAction),
                 (GLenum)(d->frontDepthFailAction),
                 (GLenum)(d->frontPassAction));
            (*extn->stencilOpSeparate)
                (GL_BACK, (GLenum)(d->backStencilFailAction),
                 (GLenum)(d->backDepthFailAction),
                 (GLenum)(d->backPassAction));
        } else {
            glStencilOp((GLenum)(d->frontStencilFailAction),
                        (GLenum)(d->frontDepthFailAction),
                        (GLenum)(d->frontPassAction));
        }
#endif
    } else {
        glDisable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0, (GLuint)(-1));
        glStencilMask((GLuint)(-1));
        glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    }
}

QT_END_NAMESPACE
