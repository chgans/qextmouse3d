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

#include "qglindexarray.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLIndexArray
    \brief The QGLIndexArray class manages arrays of vertex indices.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::enablers

    3D graphical applications need to deal with large numbers of vertices
    that specify the attributes of an object in 3D space.  The QGLIndexArray
    class makes it easier to construct and manage arrays of indices that index
    into such vertex arrays.  Faces, polygons and other 3D constructs are
    defined by sequences of indices in this way.

    QGLIndexArray can store indices as either 16-bit ushort values
    or 32-bit int values.  The 16-bit representation is used on
    OpenGL/ES systems and the 32-bit representation is used on
    desktop OpenGL systems.  The name \c ElementType is a \c typedef alias for the
    type of these indices.

    \sa QGLVertexArray
*/

/*!
    \typedef QGLIndexArray::ElementType

    A typedef for the index type, as defined by the current OpenGL implementation.

    On most OpenGL implementations this will be an \c int.

    Under OpenGL/ES ElementType is a ushort.  This is because OpenGL/ES only supports
    index arrays with elements that are 16-bits in size or less.
*/

/*!
    \fn QGLIndexArray::QGLIndexArray()

    Constructs an empty index array.  This is usually followed
    by calls to append() to add the indices.

    \code
    QGLIndexArray array;
    array.append(0);
    array.append(2);
    array.append(3);
    array.append(1);
    \endcode

    \sa append()
*/

/*!
    \fn QGLIndexArray::QGLIndexArray(const QGLIndexArray& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QGLIndexArray::~QGLIndexArray()

    Destroys this index array.
*/

/*!
    \fn QGLIndexArray& QGLIndexArray::operator=(const QGLIndexArray& other)

    Copies \a other into this index array.
*/

/*!
    \fn int QGLIndexArray::size() const

    Returns the number of indices in this index array.

    \sa isEmpty()
*/

/*!
    \fn bool QGLIndexArray::isEmpty() const

    Returns true if this index array is empty; false otherwise.

    \sa size()
*/

/*!
    Reserves space in this index array for \a size elements.
    If the index array already contains space for \a size elements
    or more, this function does nothing.

    \sa append()
*/
void QGLIndexArray::reserve(int size)
{
    m_data.reserve(size);
}

/*!
    \fn void QGLIndexArray::append(int value)

    Appends \a value to this index array.
*/

/*!
    \fn void QGLIndexArray::append(int value1, int value2)

    Appends \a value1 and \a value2 to this index array.
    This is useful for adding the indices of the two ends of a line
    to an index array.
*/

/*!
    \fn void QGLIndexArray::append(int value1, int value2, int value3)

    Appends \a value1, \a value2, and \a value3 to this index array.
    This is useful for adding the indices of the three points of a triangle
    to an index array.
*/

/*!
    \fn void QGLIndexArray::append(int value1, int value2, int value3, int value4)

    Appends \a value1, \a value2, \a value3, and \a value4 to this index array.
    This is useful for adding the indices of the three corners of a quad
    to an index array.
*/

/*!
    Appends the elements in \a other to this index array.
*/
void QGLIndexArray::append(const QGLIndexArray& other)
{
    detachBuffer();
    m_data.append(other.m_data);
}

/*!
    Replaces the contents of this index array starting at \a index
    with the elements from \a data.

    If there are more elements in \a data than in this index array,
    then additional elements will be appended to this index array.

    This function does nothing if \a index is out of range or
    \a data is empty.

    \sa append()
*/
void QGLIndexArray::replace(int index, const QGLIndexArray& data)
{
    detachBuffer();
    m_data.replace(index, data.m_data.constData(), data.m_data.count());
}

/*!
    \fn int QGLIndexArray::operator[](int index) const

    Returns the element at \a index in this array.
*/

/*!
    \fn ElementType& QGLIndexArray::operator[](int index)

    Returns a reference to the element at \a index in this array.
*/

/*!
    \fn int QGLIndexArray::type() const

    Returns the types of the values in the array returned by data()
    or constData().  This will be either GL_UNSIGNED_SHORT
    or GL_UNSIGNED_INT, and is intended to be passed as the
    third parameter to \c{glDrawElements()}.
*/

/*!
    \fn const QGLIndexArray::ElementType *QGLIndexArray::data() const

    Returns the internal data pointer for this index array, for
    direct reading.  Returns null if the array is empty.
    The return value is intended to be passed as the fourth
    parameter to \c{glDrawElements()}.

    \sa constData(), type()
*/

/*!
    \fn const QGLIndexArray::ElementType *QGLIndexArray::constData() const

    Returns the internal data pointer for this index array, for
    direct reading.  Returns null if the array is empty.
    The return value is intended to be passed as the fourth
    parameter to \c{glDrawElements()}.

    \sa data(), type()
*/

/*!
    \fn QGLIndexArray& QGLIndexArray::operator+=(int value)

    Appends \a value to this index array.

    \sa append(), operator<<()
*/

/*!
    \fn QGLIndexArray& QGLIndexArray::operator<<(int value)

    Appends \a value to this index array.

    \sa append(), operator+=()
*/

/*!
    Returns an index array that consists of the \a count elements
    from the raw array \a data.  The \a data pointer is copied by
    this function and must remain valid until the index array
    is modified or discarded.

    Note: this function will be inefficient on desktop OpenGL systems,
    because it must convert \a data to an array of int values.
*/
QGLIndexArray QGLIndexArray::fromRawData(const ushort *data, int count)
{
    QGLIndexArray array;
    if (data && count > 0) {
#if defined(QT_OPENGL_ES)
        array.m_data = QArray<ElementType>::fromRawData(data, count);
#else
        while (count-- > 0)
            array.append(*data++);
#endif
    }
    return array;
}

/*!
    \overload

    Returns an index array that consists of the \a count elements
    from the raw array \a data.  The \a data pointer is copied by
    this function and must remain valid until the index array
    is modified or discarded.

    Note: this function will be inefficient on OpenGL/ES systems,
    because it must convert \a data to an array of ushort values.
*/
QGLIndexArray QGLIndexArray::fromRawData(const int *data, int count)
{
    QGLIndexArray array;
    if (data && count > 0) {
#if defined(QT_OPENGL_ES)
        while (count-- > 0)
            array.append(*data++);
#else
        array.m_data = QArray<ElementType>::fromRawData(data, count);
#endif
    }
    return array;
}

/*!
    \fn QGLIndexArray QGLIndexArray::fromRawData(const uint *data, int count)
    \overload

    Returns an index array that consists of the \a count elements
    from the raw array \a data.  The \a data pointer is copied by
    this function and must remain valid until the index array
    is modified or discarded.

    Note: this function will be inefficient on OpenGL/ES systems,
    because it must convert \a data to an array of ushort values.
*/

/*!
    \fn bool QGLIndexArray::operator ==(const QGLIndexArray &other) const

    Returns true if the \a other QGLIndexArray is equal to this one.
    Equal here means that the other and this are the same object, or they have
    the same contents.
*/

/*!
    Binds this array to an index buffer in the GL server for the
    current GL context.

    If the array has not been uploaded to the GL server, or it has
    been modified since the last upload, the index buffer will be
    re-uploaded.  The first element in the array is uploaded as
    offset 0 in the index buffer.

    Returns true if the index buffer was bound, or false otherwise.

    \sa release(), upload()
*/
bool QGLIndexArray::bind() const
{
    if (!upload())
        return false;
    return m_buffer->bind();
}

/*!
    Releases the index buffer for this array from the current GL context.

    This function must be called with the same QGLContext current
    that was current when bind() was called.

    \sa bind()
*/
void QGLIndexArray::release() const
{
    if (m_buffer)
        m_buffer->release();
}

/*!
    Uploads this array to the GL server as an index buffer if it
    has not been uploaded before or it has been modified since the
    last upload.  Does nothing if the array is already uploaded.

    Returns true if the array was uploaded; false if uploading
    was not possible (usually because index buffers are not
    supported by the GL server).

    \sa bind(), isUploaded()
*/
bool QGLIndexArray::upload() const
{
    if (!m_buffer) {
        m_buffer = new QGLReferencedBuffer(QGLReferencedBuffer::IndexBuffer);
        if (!m_buffer->create()) {
            delete m_buffer;
            m_buffer = 0;
            return false;
        }
        if (m_buffer->bind()) {
            m_buffer->allocate(constData(), size() * sizeof(ElementType));
            m_buffer->release();
        } else {
            delete m_buffer;
            m_buffer = 0;
            return false;
        }
    }
    return true;
}

/*!
    \fn bool QGLIndexArray::isUploaded() const

    Returns true if this index array has been uploaded into an index
    buffer in the GL server by upload() or bind(); false otherwise.

    \sa upload(), bind()
*/

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<(QDebug dbg, const QGLIndexArray &indices)
{
    dbg.nospace() << "QGLIndexArray(\n";
    int mode = 3;
    if (indices.size() % 3)
        mode = 4;
    for (int i = 0; i < indices.size() / mode; ++ i)
    {
        dbg << i << " :  \t";
        for (int j = 0; j < mode; ++j)
            dbg << '\t' << indices[i * mode + j];
        dbg << '\n';
    }
    dbg << ")\n";
    return dbg.space();
}

#endif

QT_END_NAMESPACE
