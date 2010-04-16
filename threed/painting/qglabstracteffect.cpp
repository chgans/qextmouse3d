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

#include "qglabstracteffect.h"
#include "qglpainter_p.h"
#include <QtOpenGL/qglshaderprogram.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractEffect
    \since 4.7
    \brief The QGLAbstractEffect class provides a standard interface for rendering surface material effects with GL.
    \ingroup qt3d
    \ingroup qt3d::painting
*/

/*!
    Constructs a new effect object.
*/
QGLAbstractEffect::QGLAbstractEffect()
{
}

/*!
    Destroys this effect object.
*/
QGLAbstractEffect::~QGLAbstractEffect()
{
}

/*!
    \fn QList<QGL::VertexAttribute> QGLAbstractEffect::requiredFields() const

    Returns a list of the vertex array fields that are required
    to be provided via setVertexAttribute().  QGLPainter::draw()
    uses this to warn when the application has not supplied
    sufficient arguments to use the effect.

    \sa setVertexAttribute(), QGLPainter::draw()
*/

/*!
    Returns true if this effect supports object picking; false otherwise.
    The default implementation returns false, which causes QGLPainter
    to use the effect associated with QGL::FlatColor to perform
    object picking.

    Effects that support object picking render fragments with
    QGLPainter::pickColor() when QGLPainter::isPicking() returns true.
    By default, only the effect associated with QGL::FlatColor does this,
    rendering the entire fragment with the flat pick color.

    In some cases, rendering the entire fragment with the pick color
    may not be appropriate.  An alpha-blended icon texture that is
    drawn to the screen as a quad may have an irregular shape smaller
    than the quad.  For picking, the application may not want the
    entire quad to be "active" for object selection as it would appear
    to allow the user to click off the icon to select it.

    This situation can be handled by implementing an icon rendering
    effect that draws the icon normally when QGLPainter::isPicking()
    is false, and draws a mask texture defining the outline of the icon
    with QGLPainter::pickColor() when QGLPainter::isPicking() is true.

    \sa QGLPainter::setPicking()
*/
bool QGLAbstractEffect::supportsPicking() const
{
    return false;
}

/*!
    \fn void QGLAbstractEffect::setActive(QGLPainter *painter, bool flag)

    Activates or deactivates this effect on \a painter,
    according to \a flag, on the current GL context by selecting
    shader programs, setting lighting and material parameters, etc.

    \sa update()
*/

/*!
    Updates the current GL context with information from \a painter
    just prior to the drawing of triangles, quads, etc.

    The \a updates parameter specifies the properties on \a painter
    that have changed since the last call to update() or setActive().

    \sa setActive()
*/
void QGLAbstractEffect::update(QGLPainter *painter, QGLPainter::Updates updates)
{
    Q_UNUSED(painter);
    Q_UNUSED(updates);
}

/*!
    \fn void QGLAbstractEffect::setVertexAttribute(QGL::VertexAttribute attribute, const QGLAttributeValue& value)

    Sets the specified vertex \a attribute to \a value.

    The default implementation sets the attribute on the GL fixed
    function pipeline.  For example, the attribute QGL::Normal will
    be set using \c{glNormalPointer()}.

    OpenGL/ES 2.0 implementations do not have a fixed function
    pipeline, so subclasses must handle all relevant attributes
    themselves on that platform.
*/
// Implemented in qglpainter.cpp.

/*!
    Sets the common normal value for all vertices to \a value and
    disable any active normal arrays.

    The default implementation calls \c{glNormal3f()} to set \a value
    on the GL fixed function pipeline.

    OpenGL/ES 2.0 implementations do not have a fixed function
    pipeline, so subclasses must override this function and
    use QGLShaderProgram::setAttributeValue() to set the normal
    attribute on the shader program to \a value.
*/
void QGLAbstractEffect::setCommonNormal(const QVector3D& value)
{
#if !defined(QT_OPENGL_ES_2)
    glNormal3f(value.x(), value.y(), value.z());
    glDisableClientState(GL_NORMAL_ARRAY);
#else
    Q_UNUSED(value)
#endif
}

#if !defined(QT_OPENGL_ES_2) || defined(Q_QDOC)

/*!
    \fn void QGLAbstractEffect::enableVertexAttribute(QGL::VertexAttribute attribute)

    Enables the vertex array specified by \a attribute on the fixed-function
    pipeline using \c{glEnableClientState()}.

    This function is provided as a convenience for use by subclasses
    that want to implement an effect using the fixed-function pipeline
    rather than shader programs.  It is called from the subclass
    setActive() override.

    This function does not exist on OpenGL/ES 2.0 systems.

    \sa setActive(), setVertexAttribute(), disableVertexAttribute()
*/
// Implemented in qglpainter.cpp.

/*!
    \fn void QGLAbstractEffect::disableVertexAttribute(QGL::VertexAttribute attribute)

    Disables the vertex array specified by \a attribute on the fixed-function
    pipeline using \c{glDisableClientState()}.

    This function is provided as a convenience for use by subclasses
    that want to implement an effect using the fixed-function pipeline
    rather than shader programs.  It is called from the subclass
    setActive() override.

    This function does not exist on OpenGL/ES 2.0 systems.

    \sa setActive(), setVertexAttribute(), enableVertexAttribute()
*/
// Implemented in qglpainter.cpp.

#endif // !QT_OPENGL_ES_2 || Q_QDOC

/*!
    Sets the vertex attribute at \a location on \a program to \a value.
    It is assumed that \a program is bound to the current context.
    Has no effect on systems without shader support.

    This function is provided as a convenience for use by subclasses
    that want to implement an effect using shaders.
*/
void QGLAbstractEffect::setAttributeArray
        (QGLShaderProgram *program, int location,
         const QGLAttributeValue& value)
{
    // This function is working around a wart in the Qt 4.6 QGLShaderProgram
    // API that doesn't allow the vertex attribute type (GL_FLOAT, etc) to
    // be specified via the public API.  XXX - Need to fix this in 4.7.
#if defined(QT_OPENGL_ES_2)
    Q_UNUSED(program);
    glVertexAttribPointer(GLuint(location), value.tupleSize(),
                          GLenum(value.type()), GL_FALSE,
                          value.stride(), value.data());
#elif QT_VERSION >= 0x040700
    program->setAttributeArray
        (location, value.type(), value.data(),
         value.tupleSize(), value.stride());
#elif !defined(QT_OPENGL_ES_1_CL) && !defined(QT_OPENGL_ES_1)
    Q_UNUSED(program);
    const QGLContext *ctx = QGLContext::currentContext();
    if (!ctx)
        return;
    QGLPainterPrivate *extensions =
        QGLPainterPrivateCache::instance()->fromContext(ctx);
    if (!extensions->vertexAttribPointer) {
        extensions->vertexAttribPointer = (q_glVertexAttribPointer)
            ctx->getProcAddress(QLatin1String("glVertexAttribPointer"));
        if (!extensions->vertexAttribPointer)
            return;
    }
    (*extensions->vertexAttribPointer)
        (GLuint(location), value.tupleSize(), GLenum(value.type()), GL_FALSE,
         value.stride(), value.data());
#else
    Q_UNUSED(program);
    Q_UNUSED(location);
    Q_UNUSED(value);
#endif
}

QT_END_NAMESPACE
