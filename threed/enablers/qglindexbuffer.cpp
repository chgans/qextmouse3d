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
        : usagePattern(QGLBuffer::StaticDraw)
        , indexCount(0)
#ifdef QGL_INT_BUFFERS_SUPPORTED
        , elementType(GL_UNSIGNED_SHORT)
#endif
        , buffer(0)
    {
        ref = 1;
    }
    ~QGLIndexBufferPrivate()
    {
        delete buffer;
    }

    QBasicAtomicInt ref;
    QGLBuffer::UsagePattern usagePattern;
    int indexCount;
    QArray<ushort> indicesShort;
#ifdef QGL_INT_BUFFERS_SUPPORTED
    QArray<int> indicesInt;
    GLenum elementType;
#endif
    QGLBuffer *buffer;
};

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
    return d->usagePattern;
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
    d->usagePattern = value;
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
    if (d->buffer) {
        d->buffer->bind();
        d->buffer->allocate(values.constData(), values.size() * sizeof(ushort));
        d->buffer->release();
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
    if (d->buffer) {
        d->buffer->bind();
        d->buffer->write(index * sizeof(ushort),
                         values.constData(), values.size() * sizeof(ushort));
        d->buffer->release();
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
    if (d->buffer) {
        d->buffer->bind();
        d->buffer->allocate(values.constData(), values.size() * sizeof(int));
        d->buffer->release();
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
    if (d->buffer) {
        d->buffer->bind();
        d->buffer->write(index * sizeof(int),
                         values.constData(), values.size() * sizeof(int));
        d->buffer->release();
    } else {
        d->indicesInt.replace(index, values.constData(), values.size());
        d->indexCount = d->indicesInt.size();
    }
}

#endif

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
    whether the data could be uploaded or not, draw() can be used to
    support drawing of primitives using this object.

    \sa isUploaded(), setIndices(), draw()
*/
bool QGLIndexBuffer::upload()
{
    Q_D(QGLIndexBuffer);
    if (d->buffer)
        return true;
    d->buffer = new QGLBuffer(QGLBuffer::IndexBuffer);
    d->buffer->setUsagePattern(d->usagePattern);
    if (!d->buffer->create()) {
        delete d->buffer;
        d->buffer = 0;
        return false;
    }
    d->buffer->bind();
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (d->elementType == GL_UNSIGNED_SHORT) {
        d->buffer->allocate(d->indicesShort.constData(),
                            d->indicesShort.size() * sizeof(ushort));
        d->indicesShort = QArray<ushort>();
    } else {
        d->buffer->allocate(d->indicesInt.constData(),
                            d->indicesInt.size() * sizeof(int));
        d->indicesInt = QArray<int>();
    }
#else
    d->buffer->allocate(d->indicesShort.constData(),
                        d->indicesShort.size() * sizeof(ushort));
    d->indicesShort = QArray<ushort>();
#endif
    d->buffer->release();
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
    return d->buffer != 0;
}

/*!
    Returns the QGLBuffer in use by this index buffer object,
    so that its properties or contents can be modified directly.
    Returns null if the index buffer has not been uploaded.

    \sa isUploaded()
*/
QGLBuffer *QGLIndexBuffer::buffer() const
{
    Q_D(const QGLIndexBuffer);
    return d->buffer;
}

/*!
    Draws all elements in this index buffer according to \a mode.

    If the index buffer has not been uploaded to the GL server,
    this will draw using a client-side array.
*/
void QGLIndexBuffer::draw(QGL::DrawingMode mode)
{
    Q_D(QGLIndexBuffer);
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (d->buffer) {
        d->buffer->bind();
        glDrawElements(GLenum(mode), d->indexCount, d->elementType, 0);
        d->buffer->release();
    } else if (d->elementType == GL_UNSIGNED_SHORT) {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData());
    } else {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_INT,
                       d->indicesInt.constData());
    }
#else
    if (d->buffer) {
        d->buffer->bind();
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_SHORT, 0);
        d->buffer->release();
    } else {
        glDrawElements(GLenum(mode), d->indexCount, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData());
    }
#endif
}

/*!
    \overload

    Draws the \a count elements starting at \a offset in this index
    buffer according to \a mode.

    If the index buffer has not been uploaded to the GL server,
    this will draw using a client-side array.
*/
void QGLIndexBuffer::draw(QGL::DrawingMode mode, int offset, int count)
{
    Q_D(QGLIndexBuffer);
#ifdef QGL_INT_BUFFERS_SUPPORTED
    if (d->buffer) {
        d->buffer->bind();
        if (d->elementType == GL_UNSIGNED_SHORT) {
            glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       reinterpret_cast<const void *>(offset * sizeof(ushort)));
        } else {
            glDrawElements(GLenum(mode), count, GL_UNSIGNED_INT,
                       reinterpret_cast<const void *>(offset * sizeof(int)));
        }
        d->buffer->release();
    } else if (d->elementType == GL_UNSIGNED_SHORT) {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData() + offset);
    } else {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_INT,
                       d->indicesInt.constData() + offset);
    }
#else
    if (d->buffer) {
        d->buffer->bind();
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       reinterpret_cast<const void *>(offset * sizeof(ushort)));
        d->buffer->release();
    } else {
        glDrawElements(GLenum(mode), count, GL_UNSIGNED_SHORT,
                       d->indicesShort.constData() + offset);
    }
#endif
}

QT_END_NAMESPACE
