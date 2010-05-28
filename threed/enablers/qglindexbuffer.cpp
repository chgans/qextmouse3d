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

#include "qglindexbuffer.h"
#include "qglpainter.h"
#include "qglpainter_p.h"
#include <QtOpenGL/qgl.h>
#include <QtCore/qatomic.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLIndexBuffer
    \brief The QGLIndexBuffer class manages uploading of index arrays into a GL server.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers
*/

#if !defined(QT_OPENGL_ES)
#define QGL_INT_BUFFERS_SUPPORTED 1
#endif

class QGLIndexBufferPrivate
{
public:
    QGLIndexBufferPrivate()
        : indexCount(0)
#ifdef QGL_INT_BUFFERS_SUPPORTED
        , elementType(GL_UNSIGNED_SHORT)
#endif
        , buffer(QGLBuffer::IndexBuffer)
    {
        ref = 1;
    }

    QBasicAtomicInt ref;
    int indexCount;
    QArray<ushort> indicesShort;
#ifdef QGL_INT_BUFFERS_SUPPORTED
    QArray<int> indicesInt;
    GLenum elementType;
#endif
    QGLBuffer buffer;

    void append(const QGLIndexBufferPrivate *other, int offset, int start);
    int headIndex(int posn) const;
    int tailIndex(int posn) const;
};

/*!
    Returns the indices in this buffer.
*/
const QArray<ushort> &QGLIndexBuffer::indices() const
{
    return d_func()->indicesShort;
}

/*!
    Creates a new index buffer.
*/
QGLIndexBuffer::QGLIndexBuffer()
    : d_ptr(new QGLIndexBufferPrivate)
{
}

/*!
    Creates a copy of \a other.  Note that this just copies a reference
    to the index buffer.  Any modifications to the copy will also
    affect the original object.
*/
QGLIndexBuffer::QGLIndexBuffer(const QGLIndexBuffer& other)
    : d_ptr(other.d_ptr)
{
    d_ptr->ref.ref();
}

/*!
    Destroys this index buffer if this object is the last reference to it.
*/
QGLIndexBuffer::~QGLIndexBuffer()
{
    if (!d_ptr->ref.deref())
        delete d_ptr;
}

/*!
    Assigns \a other to this object.  Note that this just assigns a
    reference to the \a other index buffer.  Any modifications to this
    object will also affect \a other.
*/
QGLIndexBuffer& QGLIndexBuffer::operator=(const QGLIndexBuffer& other)
{
    if (d_ptr != other.d_ptr) {
        if (!d_ptr->ref.deref())
            delete d_ptr;
        d_ptr = other.d_ptr;
        d_ptr->ref.ref();
    }
    return *this;
}

/*!
    Returns the usage pattern for this index buffer.
    The default value is QGLBuffer::StaticDraw.

    \sa setUsagePattern()
*/
QGLBuffer::UsagePattern QGLIndexBuffer::usagePattern() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer.usagePattern();
}

/*!
    Sets the usage pattern for this index buffer to \a value.
    This function must be called before upload() for the \a value
    to take effect.

    \sa usagePattern(), upload()
*/
void QGLIndexBuffer::setUsagePattern(QGLBuffer::UsagePattern value)
{
    Q_D(QGLIndexBuffer);
    d->buffer.setUsagePattern(value);
}

/*!
    Sets the index \a values in this index buffer, replacing the
    entire current contents.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    \sa replaceIndices()
*/
void QGLIndexBuffer::setIndices(const QArray<ushort>& values)
{
    Q_D(QGLIndexBuffer);
    if (d->buffer.isCreated()) {
        d->buffer.bind();
        d->buffer.allocate(values.constData(), values.size() * sizeof(ushort));
        d->buffer.release();
#ifdef QGL_INT_BUFFERS_SUPPORTED
        // The element type may have changed from int to ushort.
        d->elementType = GL_UNSIGNED_SHORT;
#endif
    } else {
        d->indicesShort = values;
#ifdef QGL_INT_BUFFERS_SUPPORTED
        d->elementType = GL_UNSIGNED_SHORT;
        d->indicesInt = QArray<int>();
#endif
    }
    d->indexCount = values.size();
}

/*!
    Replaces the elements of this index buffer, starting at \a index,
    with the contents of \a values.  All other elements keep their
    current values.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    The index buffer must have been originally created with the
    ushort element type.

    \sa setIndices()
*/
void QGLIndexBuffer::replaceIndices(int index, const QArray<ushort>& values)
{
    Q_D(QGLIndexBuffer);
#ifdef QGL_INT_BUFFERS_SUPPORTED
    Q_ASSERT_X(d->elementType == GL_UNSIGNED_SHORT,
               "QGLIndexBuffer::replaceIndices()",
               "buffer created with int element type, replacing with ushort");
    if (d->elementType != GL_UNSIGNED_SHORT)
        return;
#endif
    if (d->buffer.isCreated()) {
        d->buffer.bind();
        d->buffer.write(index * sizeof(ushort),
                        values.constData(), values.size() * sizeof(ushort));
        d->buffer.release();
    } else {
        d->indicesShort.replace(index, values.constData(), values.size());
        d->indexCount = d->indicesShort.size();
    }
}

#if defined(QGL_INT_BUFFERS_SUPPORTED) || defined(qdoc)

/*!
    \overload

    Sets the index \a values in this index buffer, replacing the
    entire current contents.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    This overload is not present on OpenGL/ES systems because they
    don't support 32-bit index values.

    \sa replaceIndices()
*/
void QGLIndexBuffer::setIndices(const QArray<int>& values)
{
    Q_D(QGLIndexBuffer);
    if (d->buffer.isCreated()) {
        d->buffer.bind();
        d->buffer.allocate(values.constData(), values.size() * sizeof(int));
        d->buffer.release();
        // The element type may have changed from ushort to int.
        d->elementType = GL_UNSIGNED_INT;
    } else {
        d->indicesInt = values;
        d->elementType = GL_UNSIGNED_INT;
        d->indicesShort = QArray<ushort>();
    }
    d->indexCount = values.size();
}

/*!
    \overload

    Replaces the elements of this index buffer, starting at \a index,
    with the contents of \a values.  All other elements keep their
    current values.

    If the index buffer has been uploaded to the GL server, then this
    function must be called with a current GL context that is compatible
    with the uploaded buffer.

    The index buffer must have been originally created with the
    int element type.

    This overload is not present on OpenGL/ES systems because they
    don't support 32-bit index values.

    \sa setIndices()
*/
void QGLIndexBuffer::replaceIndices(int index, const QArray<int>& values)
{
    Q_D(QGLIndexBuffer);
    Q_ASSERT_X(d->elementType == GL_UNSIGNED_INT,
               "QGLIndexBuffer::replaceIndices()",
               "buffer created with ushort element type, replacing with int");
    if (d->elementType != GL_UNSIGNED_INT)
        return;
    if (d->buffer.isCreated()) {
        d->buffer.bind();
        d->buffer.write(index * sizeof(int),
                        values.constData(), values.size() * sizeof(int));
        d->buffer.release();
    } else {
        d->indicesInt.replace(index, values.constData(), values.size());
        d->indexCount = d->indicesInt.size();
    }
}

#endif

/*!
    Returns the element type for this index buffer, \c{GL_UNSIGNED_SHORT}
    or \c{GL_UNSIGNED_INT}.
*/
GLenum QGLIndexBuffer::elementType() const
{
#ifdef QGL_INT_BUFFERS_SUPPORTED
    Q_D(const QGLIndexBuffer);
    return d->elementType;
#else
    return GL_UNSIGNED_SHORT;
#endif
}

/*!
    Returns the number of indices in this index buffer.
*/
int QGLIndexBuffer::indexCount() const
{
    Q_D(const QGLIndexBuffer);
    return d->indexCount;
}

/*!
    \fn bool QGLIndexBuffer::isEmpty() const

    Returns true if indexCount() is zero; false otherwise.
*/

/*!
    Uploads the index data specified by a previous setIndices()
    call into the GL server as an index buffer object.

    Returns true if the data could be uploaded; false if index buffer
    objects are not supported or there is insufficient memory to complete
    the request.  Returns true if the data was already uploaded.

    Once the index data has been uploaded, the client-side copies of
    the data arrays will be released.  If the index data could not be
    uploaded, then it is retained client-side.  This way, regardless of
    whether the data could be uploaded or not, QGLPainter::draw() can
    be used to support drawing of primitives using this object.

    \sa isUploaded(), setIndices(), QGLPainter::draw()
*/
bool QGLIndexBuffer::upload()
{
    Q_D(QGLIndexBuffer);
    if (d->buffer.isCreated())
        return true;
    if (!d->buffer.create())
        return false;
    d->buffer.bind();
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (d->elementType == GL_UNSIGNED_SHORT) {
        d->buffer.allocate(d->indicesShort.constData(),
                           d->indicesShort.size() * sizeof(ushort));
        d->indicesShort = QArray<ushort>();
    } else {
        d->buffer.allocate(d->indicesInt.constData(),
                           d->indicesInt.size() * sizeof(int));
        d->indicesInt = QArray<int>();
    }
#else
    d->buffer.allocate(d->indicesShort.constData(),
                       d->indicesShort.size() * sizeof(ushort));
    d->indicesShort = QArray<ushort>();
#endif
    d->buffer.release();
    return true;
}

/*!
    Returns true if the index data specified by previous a setIndices()
    call has been uploaded into the GL server; false otherwise.

    \sa upload(), setIndices()
*/
bool QGLIndexBuffer::isUploaded() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer.isCreated();
}

/*!
    Returns the QGLBuffer in use by this index buffer object,
    so that its properties or contents can be modified directly.

    \sa isUploaded()
*/
QGLBuffer QGLIndexBuffer::buffer() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer;
}

/*!
    Binds this index buffer to the current GL context.  Returns false if
    binding was not possible, usually because upload() has not been called.

    The buffer must be bound to the same QGLContext current when upload()
    was called, or to another QGLContext that is sharing with it.
    Otherwise, false will be returned from this function.

    \sa release(), upload()
*/
bool QGLIndexBuffer::bind() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer.bind();
}

/*!
    Releases this index buffer from the current GL context.

    This function must be called with the same QGLContext current
    as when bind() was called on the index buffer.

    \sa bind()
*/
void QGLIndexBuffer::release() const
{
    Q_D(const QGLIndexBuffer);
    d->buffer.release();
}

void QGLIndexBufferPrivate::append
    (const QGLIndexBufferPrivate *other, int offset, int start)
{
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (elementType == GL_UNSIGNED_SHORT &&
            other->elementType == GL_UNSIGNED_SHORT) {
        // Both buffers are ushort.
        const ushort *data = other->indicesShort.constData() + start;
        int count = other->indicesShort.count() - start;
        indicesShort.reserve(indicesShort.count() + count);
        indexCount += count;
        while (count-- > 0)
            indicesShort.append(ushort(*data++ + offset));
    } if (elementType == GL_UNSIGNED_SHORT) {
        // Only first buffer is ushort: convert it to int first.
        const ushort *indices = indicesShort.constData();
        int count = indicesShort.count();
        indicesInt.reserve(count + other->indicesInt.count());
        while (count-- > 0)
            indicesInt.append(*indices++);
        indicesShort = QArray<ushort>();
        elementType = GL_UNSIGNED_INT;
        const int *data = other->indicesInt.constData() + start;
        count = other->indicesInt.count() - start;
        indexCount += count;
        while (count-- > 0)
            indicesInt.append(*data++ + offset);
    } if (other->elementType == GL_UNSIGNED_SHORT) {
        // Only second buffer is ushort.
        const ushort *data = other->indicesShort.constData() + start;
        int count = other->indicesShort.count() - start;
        indicesInt.reserve(indicesInt.count() + count);
        indexCount += count;
        while (count-- > 0)
            indicesInt.append(*data++ + offset);
    } else {
        // Neither buffer is ushort.
        const int *data = other->indicesInt.constData() + start;
        int count = other->indicesInt.count() - start;
        indicesInt.reserve(indicesInt.count() + count);
        indexCount += count;
        while (count-- > 0)
            indicesInt.append(*data++ + offset);
    }
#else
    const ushort *data = other->indicesShort.constData();
    int count = other->indicesShort.count();
    indicesShort.reserve(indicesShort.count() + count);
    indexCount += count;
    while (count-- > 0)
        indicesShort.append(ushort(*data++ + offset));
#endif
}

int QGLIndexBufferPrivate::headIndex(int posn) const
{
    if (indexCount <= posn)
        return -1;
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (elementType == GL_UNSIGNED_SHORT)
        return indicesShort[posn];
    else
        return indicesInt[posn];
#else
    return indicesInt[posn];
#endif
}

int QGLIndexBufferPrivate::tailIndex(int posn) const
{
    if (indexCount <= posn)
        return -1;
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (elementType == GL_UNSIGNED_SHORT)
        return indicesShort[indexCount - posn - 1];
    else
        return indicesInt[indexCount - posn - 1];
#else
    return indicesInt[indexCount - posn - 1];
#endif
}

/*!
    Appends the contents of \a buffer to this index buffer and adds
    \a offset to all of the entries in \a buffer.

    This function is typically used to combine multiple geometry meshes
    into a single mesh that can be bound as a single buffer.

    The request is ignored if this index buffer or \a buffer have already
    been uploaded, or \a buffer is this index buffer.

    \sa isUploaded(), setIndices()
*/
void QGLIndexBuffer::append(const QGLIndexBuffer &buffer, int offset)
{
    Q_D(QGLIndexBuffer);
    const QGLIndexBufferPrivate *dbuf = buffer.d_ptr;

    // Bail out if the buffers are uploaded or identical.
    if (d->buffer.isCreated() || dbuf->buffer.isCreated())
        return;
    if (d == dbuf)
        return;

    // Append the two index arrays.
    d->append(dbuf, offset, 0);
}

/*!
    Appends the contents of \a buffer to this index buffer and adds
    \a offset to all of the entries in \a buffer.

    The two buffers will be merged at the join point according to
    \a combineMode.  For example, if \a combineMode is QGL::TriangleStrip,
    then the result will be a single triangle strip.  Indices are
    dropped from the front of \a buffer as necessary to correctly
    merge the buffers.

    This function is typically used to combine multiple geometry meshes
    into a single mesh that can be bound as a single buffer.

    The request is ignored if this index buffer or \a buffer have already
    been uploaded, or \a buffer is this index buffer.

    \sa isUploaded(), setIndices()
*/
void QGLIndexBuffer::append
    (const QGLIndexBuffer &buffer, int offset, QGL::DrawingMode combineMode)
{
    Q_D(QGLIndexBuffer);
    const QGLIndexBufferPrivate *dbuf = buffer.d_ptr;

    // Bail out if the buffers are uploaded or identical.
    if (d->buffer.isCreated() || dbuf->buffer.isCreated())
        return;
    if (d == dbuf)
        return;

    // Determine how to combine the buffers.
    switch (int(combineMode)) {
    case QGL::Points:
    case QGL::Lines:
    case QGL::Triangles:
    case QGL::LinesAdjacency:
    case QGL::TrianglesAdjacency:
    case 0x0007:            // GL_QUADS
        // These can be done by just appending the raw data with no changes.
        d->append(dbuf, offset, 0);
        break;

    case QGL::LineLoop:
    case QGL::LineStrip:
    case 0x0009:            // GL_POLYGON
        // Join the last index of the first buffer to the first
        // index of the second buffer to continue the loop or strip.
        if (d->tailIndex(0) == (dbuf->headIndex(0) + offset))
            d->append(dbuf, offset, 1);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::TriangleStrip:
        // Join the last two indexes of the first buffer to the first
        // two indexes of the second buffer to continue the strip.
        // It is possible that the first two indexes of the second
        // buffer may be reversed for strip continuation depending
        // upon whether the first strip is odd or even in length.
        if (d->tailIndex(1) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(1) + offset))
            d->append(dbuf, offset, 2);
        else if (d->tailIndex(1) == (dbuf->headIndex(1) + offset) &&
                 d->tailIndex(0) == (dbuf->headIndex(0) + offset))
            d->append(dbuf, offset, 2);
        else
            d->append(dbuf, offset, 0);
        break;

    case 0x0008:            // GL_QUAD_STRIP
        // Join the last two indexes of the first buffer to the first
        // two indexes of the second buffer to continue the strip.
        if (d->tailIndex(1) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(1) + offset))
            d->append(dbuf, offset, 2);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::TriangleFan:
        // The first index of both buffers should be the same, and the
        // last index of the first buffer should be the same as the second
        // index of the second buffer.
        if (d->headIndex(0) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(1) + offset))
            d->append(dbuf, offset, 2);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::LineStripAdjacency:
        // Join the last three indexes of the first buffer to the first
        // three indexes of the second buffer to continue the strip.
        if (d->tailIndex(2) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(1) == (dbuf->headIndex(1) + offset) &&
                d->tailIndex(0) == (dbuf->headIndex(2) + offset))
            d->append(dbuf, offset, 3);
        else
            d->append(dbuf, offset, 0);
        break;

    case QGL::TriangleStripAdjacency:
        // Fourth last and second last of first buffer need to be the
        // same as the first and third of the second buffer.
        // Also handle the reversed case for odd/even strip lengths.
        // FIXME: may still not be quite right yet.
        if (d->tailIndex(3) == (dbuf->headIndex(0) + offset) &&
                d->tailIndex(1) == (dbuf->headIndex(2) + offset))
            d->append(dbuf, offset, 4);
        else if (d->tailIndex(3) == (dbuf->headIndex(2) + offset) &&
                 d->tailIndex(1) == (dbuf->headIndex(0) + offset))
            d->append(dbuf, offset, 4);
        else
            d->append(dbuf, offset, 0);
        break;

    default:
        qWarning("QGLIndexBuffer::append: unknown drawing mode %d",
                 int(combineMode));
        break;
    }
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexAttribute().  The type of primitive to draw is
    specified by \a mode.

    This operation will consume all of the elements of \a indices,
    which are used to index into the enabled arrays.

    If \a indices has not been uploaded to the GL server as an index
    buffer, then this function will draw using a client-side array.

    \sa update(), QGLIndexBuffer::upload()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const QGLIndexBuffer& indices)
{
    const QGLIndexBufferPrivate *d = indices.d_func();
    update();
#ifndef QT_NO_DEBUG
    checkRequiredFields();
#endif
    GLuint id = d->buffer.bufferId();
    if (id != d_ptr->boundIndexBuffer) {
        if (id)
            d->buffer.bind();
        else
            QGLBuffer::release(QGLBuffer::IndexBuffer);
        d_ptr->boundIndexBuffer = id;
    }
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (id) {
        glDrawElements(GLenum(mode), d->indexCount, d->elementType, 0);
    } else if (d->elementType == GL_UNSIGNED_SHORT) {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData());
    } else {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_INT,
                       d->indicesInt.constData());
    }
#else
    if (id) {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_SHORT, 0);
    } else {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData());
    }
#endif
}

/*!
    \overload

    Draws primitives using vertices from the arrays specified by
    setVertexAttribute().  The type of primitive to draw is
    specified by \a mode.

    This operation will consume \a count elements of \a indices,
    starting at \a offset, which are used to index into the enabled arrays.

    If \a indices has not been uploaded to the GL server as an index
    buffer, then this function will draw using a client-side array.

    \sa update(), QGLIndexBuffer::upload()
*/
void QGLPainter::draw(QGL::DrawingMode mode, const QGLIndexBuffer& indices, int offset, int count)
{
    const QGLIndexBufferPrivate *d = indices.d_func();
    update();
#ifndef QT_NO_DEBUG
    checkRequiredFields();
#endif
    GLuint id = d->buffer.bufferId();
    if (id != d_ptr->boundIndexBuffer) {
        if (id)
            d->buffer.bind();
        else
            QGLBuffer::release(QGLBuffer::IndexBuffer);
        d_ptr->boundIndexBuffer = id;
    }
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (id) {
        if (d->elementType == GL_UNSIGNED_SHORT) {
            glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       reinterpret_cast<const void *>(offset * sizeof(ushort)));
        } else {
            glDrawElements(GLenum(mode), count, GL_UNSIGNED_INT,
                       reinterpret_cast<const void *>(offset * sizeof(int)));
        }
    } else if (d->elementType == GL_UNSIGNED_SHORT) {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData() + offset);
    } else {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_INT,
                       d->indicesInt.constData() + offset);
    }
#else
    if (id) {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       reinterpret_cast<const void *>(offset * sizeof(ushort)));
    } else {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData() + offset);
    }
#endif
}

QT_END_NAMESPACE
