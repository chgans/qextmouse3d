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
#include <QtCore/qstack.h>

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
#endif
    QStack<QMatrix4x4> stack;
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
        break;
    case QGLMatrixStack::ProjectionMatrix:
        glType = GL_PROJECTION;
        glFetchType = GL_PROJECTION_MATRIX;
        break;
    default:
        glType = 0;
        glFetchType = 0;
        break;
    }
#endif
    isDirty = true;
}

/*!
    \internal
*/
QGLMatrixStack::QGLMatrixStack(QGLMatrixStack::Type type)
    : d_ptr(new QGLMatrixStackPrivate(type))
{
}

/*!
    Creates a matrix stack with type UserMatrix.

    \sa type()
*/
QGLMatrixStack::QGLMatrixStack()
    : d_ptr(new QGLMatrixStackPrivate(UserMatrix))
{
}

/*!
    Destroy this matrix stack.
*/
QGLMatrixStack::~QGLMatrixStack()
{
}

/*!
    Returns the type of matrices being managed by this matrix stack:
    ModelViewMatrix, ProjectionMatrix, or UserMatrix.
*/
QGLMatrixStack::Type QGLMatrixStack::type() const
{
    Q_D(const QGLMatrixStack);
    return d->type;
}

/*!
    Pushes the current matrix onto the matrix stack.  The matrix can
    be restored with pop().  The new top of stack will have the
    same value as the previous top of stack.

    The depths of the traditional \c{GL_MODELVIEW} and \c{GL_PROJECTION}
    matrix stacks in the GL server are system-dependent and easy to
    overflow in nested rendering code using \c{glPushMatrix()}.
    By contrast, the push() function provides an arbitrary-sized stack
    in client memory.

    \sa pop(), top()
*/
void QGLMatrixStack::push()
{
    Q_D(QGLMatrixStack);
    d->stack.push(d->matrix);
}

/*!
    Pops the top-most matrix from this matrix stack and sets the
    current matrix to the next value down.  Does nothing if the
    matrix stack contains a single entry.

    \sa push()
*/
void QGLMatrixStack::pop()
{
    Q_D(QGLMatrixStack);
    if (!d->stack.isEmpty())
        d->matrix = d->stack.pop();
    d->isDirty = true;
}

/*!
    Set the matrix at the top of this matrix stack to the identity matrix.

    \sa operator=()
*/
void QGLMatrixStack::setToIdentity()
{
    Q_D(QGLMatrixStack);
    d->matrix.setToIdentity();
    d->isDirty = true;
}

/*!
    Returns a const reference to the current matrix at the top of this
    matrix stack.  This is typically used to fetch the matrix so it can
    be set on user-defined shader programs.

    \sa operator=(), readServerMatrix()
*/
const QMatrix4x4 &QGLMatrixStack::top() const
{
    Q_D(const QGLMatrixStack);
    return d->matrix;
}

/*!
    \fn QGLMatrixStack::operator const QMatrix4x4 &() const

    Returns a const reference to the current matrix at the top of
    this matrix stack.

    \sa top()
*/

/*!
    Assigns \a matrix to the matrix at the top of this matrix stack.

    \sa top()
*/
QGLMatrixStack& QGLMatrixStack::operator=(const QMatrix4x4& matrix)
{
    Q_D(QGLMatrixStack);
    d->matrix = matrix;
    d->isDirty = true;
    return *this;
}

/*!
    Multiplies the matrix at the top of this matrix stack by \a matrix.

    \sa top()
*/
QGLMatrixStack& QGLMatrixStack::operator*=(const QMatrix4x4& matrix)
{
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
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
    Q_D(QGLMatrixStack);
    d->matrix.rotate(quaternion);
    d->isDirty = true;
}

/*!
    Reads the matrix in the GL server that corresponds to \a type in
    the current GL context and returns it.

    This function ignores the contents of the matrix stack in the
    client and reads the actual value in the GL server.  This can
    be used to read back a matrix value that was set by raw GL code:

    \code
    painter.modelViewMatrix() =
        QGLMatrixStack::readerServerMatrix(QGLMatrixStack::ModelViewMatrix);
    \endcode

    Under OpenGL/ES 2.0, this function will return the identity matrix
    because server-side matrices are not supported.  If \a type is
    UserMatrix, then the return value will also be the identity matrix.

    \sa top()
*/
QMatrix4x4 QGLMatrixStack::readServerMatrix(QGLMatrixStack::Type type)
{
    QMatrix4x4 matrix;
#if !defined(QT_OPENGL_ES_2)
    GLenum fetchType;
    if (type == ModelViewMatrix)
        fetchType = GL_MODELVIEW_MATRIX;
    else if (type == ProjectionMatrix)
        fetchType = GL_PROJECTION_MATRIX;
    else
        return matrix;
    if (sizeof(qreal) == sizeof(GLfloat)) {
        glGetFloatv(fetchType, reinterpret_cast<GLfloat *>(matrix.data()));
    } else {
        GLfloat mat[16];
        glGetFloatv(fetchType, mat);
        qreal *m = matrix.data();
        for (int index = 0; index < 16; ++index)
            m[index] = mat[index];
    }
    matrix.optimize();
#endif
    return matrix;
}

void QGLMatrixStack::markDirty()
{
    Q_D(QGLMatrixStack);
    d->isDirty = true;
}

bool QGLMatrixStack::needsUpdate()
{
    Q_D(QGLMatrixStack);
    bool dirty = d->isDirty;
    d->isDirty = false;
    return dirty;
}

void QGLMatrixStack::updateServer()
{
#if !defined(QT_OPENGL_ES_2)
    Q_D(QGLMatrixStack);
    if (d->type != UserMatrix) {
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
}

QMatrix4x4 QGLPainter::combinedMatrix() const
{
    const QGLPainterPrivate *d = d_func();
    if (!d)
        return QMatrix4x4();
    const QGLMatrixStackPrivate *proj = d->projectionMatrix.d_func();
    const QGLMatrixStackPrivate *mv = d->modelViewMatrix.d_func();
    return proj->matrix * mv->matrix;
}

QMatrix3x3 QGLPainter::normalMatrix() const
{
    const QGLPainterPrivate *d = d_func();
    if (!d)
        return QMatrix3x3();
    const QGLMatrixStackPrivate *mv = d->modelViewMatrix.d_func();
    return mv->matrix.normalMatrix();
}

QT_END_NAMESPACE
