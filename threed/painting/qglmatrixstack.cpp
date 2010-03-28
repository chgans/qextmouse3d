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

#include "qglmatrixstack.h"
#include "qglpainter_p.h"
#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLMatrixStack
    \brief The QGLMatrixStack class manages stacks of transformation matrices in GL applications.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::painting

    Transformation matrices are one of the basic building blocks of
    3D applications, allowing object models to be positioned, scaled,
    rotated, and projected onto the screen.

    GL systems support several standard kinds of matrices, particularly
    modelview and projection matrices.  These matrices are typically
    organized into stacks, which allow the current matrix state to be
    saved with push() and restored later with pop().

    QGLMatrixStack assists QGLPainter with the management of matrix
    stacks, providing operations to set and modify transformation
    matrices in each of the standard matrix stacks.

    Three types of matrix stack are provided: ProjectionMatrix,
    ModelViewMatrix, and UserMatrix.  The first two correspond to
    the traditional GL matrix stack types in the GL server.  The third
    enables applications to define additional matrix stacks for
    their own use.  UserMatrix stacks do not have a server-side
    counterpart.

    In the following example, a standard orthographic projection matrix for a
    view is set via the QGLPainter::projectionMatrix() stack, and
    then a modelview matrix is set via the QGLPainter::modelViewMatrix()
    stack to scale and translate an object prior to drawing:

    \code
    QGLPainter painter(this);

    painter.projectionMatrix().setToIdentity();
    painter.projectionMatrix().ortho(window->rect());

    painter.modelViewMatrix().setToIdentity();
    painter.modelViewMatrix().translate(-1.0f, 2.0f, 0.0f);
    painter.modelViewMatrix().scale(0.5f);
    \endcode

    Later, the application can save the current modelview matrix state
    and draw a different object with a different modelview matrix:

    \code
    painter.modelViewMatrix().push();
    painter.modelViewMatrix().setToIdentity();
    painter.modelViewMatrix().scale(2.0f);
    \endcode

    For efficiency, the matrix values are kept client-side until they
    are needed by a QGLPainter::draw() operation.  Until then, changes
    to the matrix will not be reflected in the GL server.  The application
    can force the GL server to update the server with a call to
    QGLPainter::update().

    QGLMatrixStack is supported on all GL platforms, including OpenGL/ES 2.0
    which doesn't support matrix stacks natively.  On that platform, the
    matrix stack is simulated in client memory.  When the application
    selects a shader program to draw under OpenGL/ES 2.0, it calls
    top() to obtain the actual value to be set on the shader program.

    \sa QGLPainter
*/

/*!
    \enum QGLMatrixStack::Type
    This enum defines the type of a QGLMatrixStack.

    \value ProjectionMatrix Projection matrix for the view.
    \value ModelViewMatrix Modelview matrix for the view.
    \value UserMatrix User-defined matrix stack.
*/

class QGLMatrixStackPrivate
{
public:
    QGLMatrixStackPrivate(QGLMatrixStack::Type t);

    QGLMatrixStack::Type type;
    QMatrix4x4 matrix;
#if !defined(QT_OPENGL_ES_2)
    GLenum glType;
    GLenum glFetchType;
    QList<bool> serverStack;
#endif
    QList<QMatrix4x4> stack;
    bool haveMatrix;
    bool isDirty;
};

QGLMatrixStackPrivate::QGLMatrixStackPrivate(QGLMatrixStack::Type t)
{
    type = t;
#if !defined(QT_OPENGL_ES_2)
    switch (t) {
    case QGLMatrixStack::ModelViewMatrix:
        glType = GL_MODELVIEW;
        glFetchType = GL_MODELVIEW_MATRIX;
        haveMatrix = false;
        break;
    case QGLMatrixStack::ProjectionMatrix:
        glType = GL_PROJECTION;
        glFetchType = GL_PROJECTION_MATRIX;
        haveMatrix = false;
        break;
    default:
        glType = 0;
        glFetchType = 0;
        haveMatrix = true;
        break;
    }
#else
    haveMatrix = true;  // We always have the matrix for OpenGL/ES 2.0.
#endif
    isDirty = true;
}

/*!
    \internal
*/
QGLMatrixStack::QGLMatrixStack(QGLMatrixStack::Type type)
{
    d = new QGLMatrixStackPrivate(type);
}

/*!
    Creates a matrix stack with type UserMatrix.

    \sa type()
*/
QGLMatrixStack::QGLMatrixStack()
{
    d = new QGLMatrixStackPrivate(QGLMatrixStack::UserMatrix);
}

/*!
    Destroy this matrix stack.
*/
QGLMatrixStack::~QGLMatrixStack()
{
    delete d;
}

/*!
    Returns the type of matrices being managed by this matrix stack:
    QGL::ModelViewMatrix or QGL::ProjectionMatrix.
*/
QGLMatrixStack::Type QGLMatrixStack::type() const
{
    return d->type;
}

/*!
    Resets the application's notion of the top-most element in this
    matrix stack to the value from the GL server.

    This function is used to synchronize the state of the application
    with the GL server after the execution of raw GL commands that may
    have altered the matrix in the server.

    An alternative is to call operator=() or setToIdentity() to forcibly
    set the matrix in the GL server to a known value.

    \sa top(), operator=(), setToIdentity()
*/
void QGLMatrixStack::reset()
{
#if !defined(QT_OPENGL_ES_2)
    if (d->type != UserMatrix)
        d->haveMatrix = false;
#endif
    d->isDirty = true;
}

/*!
    Pushes the current matrix onto the matrix stack.  The matrix can
    be restored with pop().  The new top of stack will have the
    same value as the previous top of stack.

    The depths of the traditional \c{GL_MODELVIEW} and \c{GL_PROJECTION}
    matrix stacks in the GL server are system-dependent and easy to
    overflow in nested rendering code using \c{glPushMatrix()}.

    The push() function provides an arbitrary-sized stack in client memory.
    If it knows the matrix value, it will push it onto the client stack.
    If it does not know the matrix value yet, then push() will call
    \c{glPushMatrix()} instead.  To ensure that push() knows the
    matrix value, it should be forcibly set with a call to operator=()
    or setToIdentity().
    
    \sa pop(), top()
*/
void QGLMatrixStack::push()
{
#if !defined(QT_OPENGL_ES_2)
    // If we don't have the matrix value, then make the GL server save it.
    if (!d->haveMatrix && d->glType) {
        glMatrixMode(d->glType);
        glPushMatrix();
        d->serverStack.append(true);
    } else {
        d->serverStack.append(false);
    }
#endif
    d->stack.append(d->matrix);
}

/*!
    Pops the top-most matrix from this matrix stack and sets the
    current matrix to the next value down.

    \sa push()
*/
void QGLMatrixStack::pop()
{
#if !defined(QT_OPENGL_ES_2)
    if (d->stack.isEmpty() && d->glType) {
        glMatrixMode(d->glType);
        glPopMatrix();
        d->haveMatrix = false;
    } else if (!d->stack.isEmpty()) {
        bool inServer = d->serverStack.takeLast();
        d->matrix = d->stack.takeLast();
        if (inServer) {
            glMatrixMode(d->glType);
            glPopMatrix();
            d->haveMatrix = false;
        } else {
            d->haveMatrix = true;
        }
    }
#else
    if (!d->stack.isEmpty())
        d->matrix = d->stack.takeLast();
#endif
    d->isDirty = true;
}

/*!
    Set the matrix at the top of this matrix stack to the identity matrix.

    \sa operator=()
*/
void QGLMatrixStack::setToIdentity()
{
    d->matrix.setToIdentity();
    d->haveMatrix = true;
    d->isDirty = true;
}

/*!
    Returns the current matrix at the top of this matrix stack.
    This is typically used to fetch the matrix so it can be set on
    user-defined shader programs.

    Performance note: if the matrix has not been set previously,
    then this function may need to perform a round trip to the GL server
    to obtain the current state of the matrix.  The round-trip can be
    avoided by calling operator=() or setToIdentity() to set the GL server's
    matrix to a known value.

    Matrix stacks of type UserMatrix never need to perform a round-trip
    to the GL server because they are implemented purely client-side.

    \sa operator=(), setToIdentity()
*/
QMatrix4x4 QGLMatrixStack::top() const
{
#if !defined(QT_OPENGL_ES_2)
    if (!(d->haveMatrix)) {
        // We need to retrieve the current matrix from the GL server
        // because we have no way to know what state it is currently in.
        d->haveMatrix = true;
        if (sizeof(qreal) == sizeof(GLfloat)) {
            glGetFloatv(d->glFetchType, reinterpret_cast<GLfloat *>
                                            (d->matrix.data()));
            d->matrix.optimize();
        } else {
            GLfloat mat[16];
            glGetFloatv(d->glFetchType, mat);
            qreal *m = d->matrix.data();
            for (int index = 0; index < 16; ++index)
                m[index] = mat[index];
            d->matrix.optimize();
        }
    }
#endif
    return d->matrix;
}

/*!
    \fn QGLMatrixStack::operator QMatrix4x4() const

    Returns the current matrix at the top of this matrix stack.

    \sa top()
*/

/*!
    Assigns \a matrix to the matrix at the top of this matrix stack.

    \sa top()
*/
QGLMatrixStack& QGLMatrixStack::operator=(const QMatrix4x4& matrix)
{
    d->matrix = matrix;
    d->haveMatrix = true;
    d->isDirty = true;
    return *this;
}

/*!
    Multiplies the matrix at the top of this matrix stack by \a matrix.

    \sa top()
*/
QGLMatrixStack& QGLMatrixStack::operator*=(const QMatrix4x4& matrix)
{
    if (!d->haveMatrix)
        top();
    d->matrix *= matrix;
    d->isDirty = true;
    return *this;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that applies an orthographic projection for a window with boundaries
    specified by \a rect, and clipping planes specified by \a nearPlane and \a farPlane.
*/
void QGLMatrixStack::ortho(const QRect& rect, qreal nearPlane, qreal farPlane)
{
    // Note: rect.right() and rect.bottom() subtract 1 in QRect,
    // which gives the location of a pixel within the rectangle,
    // instead of the extent of the rectangle.  We want the extent.
    // QRectF expresses the extent properly.
    if (!d->haveMatrix)
        this->top();
    d->matrix.ortho(rect.x(), rect.x() + rect.width(),
                    rect.y() + rect.height(), rect.y(), nearPlane, farPlane);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that translates coordinates by (\a x, \a y, \a z).  The following example
    translates the modelview matrix by (1, -3, 0):

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().translate(1.0f, -3.0f, 0.0f);
    \endcode

    \sa scale(), rotate()
*/
void QGLMatrixStack::translate(qreal x, qreal y, qreal z)
{
    if (!d->haveMatrix)
        top();
    d->matrix.translate(x, y, z);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix statck by another
    that translates coordinates by the components of \a vector.

    \sa scale(), rotate()
*/
void QGLMatrixStack::translate(const QVector3D& vector)
{
    if (!d->haveMatrix)
        top();
    d->matrix.translate(vector);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that scales coordinates by the components \a x, \a y, and \a z.
    The following example scales the modelview matrix by (1, 2, 1):

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().scale(1.0f, 2.0f, 1.0f);
    \endcode

    \sa translate(), rotate()
*/
void QGLMatrixStack::scale(qreal x, qreal y, qreal z)
{
    if (!d->haveMatrix)
        top();
    d->matrix.scale(x, y, z);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that scales coordinates by the given \a factor.  The following example
    scales the modelview matrix by a factor of 2:

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().scale(2.0f);
    \endcode

    \sa translate(), rotate()
*/
void QGLMatrixStack::scale(qreal factor)
{
    if (!d->haveMatrix)
        top();
    d->matrix.scale(factor);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that scales coordinates by the components of \a vector.

    \sa translate(), rotate()
*/
void QGLMatrixStack::scale(const QVector3D& vector)
{
    if (!d->haveMatrix)
        top();
    d->matrix.scale(vector);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that rotates coordinates through \a angle degrees about the vector
    (\a x, \a y, \a z).  The following example rotates the modelview
    matrix by 45 degress about the vector (1, -3, 0):

    \code
    QGLPainter painter(this);
    painter.modelViewMatrix().rotate(45.0f, 1.0f, -3.0f, 0.0f);
    \endcode

    \sa scale(), translate()
*/
void QGLMatrixStack::rotate(qreal angle, qreal x, qreal y, qreal z)
{
    if (!d->haveMatrix)
        top();
    d->matrix.rotate(angle, x, y, z);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by another
    that rotates coordinates through \a angle degrees about \a vector.

    \sa scale(), translate()
*/
void QGLMatrixStack::rotate(qreal angle, const QVector3D& vector)
{
    if (!d->haveMatrix)
        top();
    d->matrix.rotate(angle, vector);
    d->isDirty = true;
}

/*!
    Multiplies the current matrix at the top of this matrix stack by the
    \a quaternion.  Thus \c {painter->modelViewMatrix().rotate(quaternion)}
    is equivalent to the following code:
    \code
    QMatrix4x4 mat;
    mat.rotate(quaternion);
    painter->modelViewMatrix() *= mat;
    \endcode
    which rotates coordinates according to the given \a quaternion.

    \sa scale(), translate()
*/
void QGLMatrixStack::rotate(const QQuaternion &quaternion)
{
    if (!d->haveMatrix)
        top();
    d->matrix.rotate(quaternion);
    d->isDirty = true;
}

bool QGLMatrixStack::updateServer()
{
    bool dirty = d->isDirty;
#if !defined(QT_OPENGL_ES_2)
    if (dirty && d->type != UserMatrix && d->haveMatrix) {
        glMatrixMode(d->glType);
        if (sizeof(qreal) == sizeof(GLfloat)) {
            glLoadMatrixf(reinterpret_cast<const GLfloat *>
                (d->matrix.constData()));
        } else {
            GLfloat mat[16];
            const qreal *m = d->matrix.constData();
            for (int index = 0; index < 16; ++index)
                mat[index] = m[index];
            glLoadMatrixf(mat);
        }
    }
#endif
    d->isDirty = false;
    return dirty;
}

QMatrix4x4 QGLPainter::combinedMatrix() const
{
    const QGLPainterPrivate *d = d_func();
    if (!d)
        return QMatrix4x4();
    QGLMatrixStackPrivate *proj = d->projectionMatrix.d;
    QGLMatrixStackPrivate *mv = d->modelViewMatrix.d;
    if (!proj->haveMatrix)
        d->projectionMatrix.top();
    if (!mv->haveMatrix)
        d->modelViewMatrix.top();
    return proj->matrix * mv->matrix;
}

QT_END_NAMESPACE
