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

#include "qclbuffer.h"
#include "qclcontext.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLBuffer
    \brief The QCLBuffer class represents an OpenCL buffer object.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

/*!
    \fn QCLBuffer::QCLBuffer()

    Constructs a null OpenCL buffer object.
*/

/*!
    \fn QCLBuffer::QCLBuffer(QCLContext *context, cl_mem id)

    Constructs an OpenCL buffer object that is initialized with the
    native OpenCL identifier \a id, and associates it with \a context.
    This class will take over ownership of \a id and will release
    it in the destructor.
*/

/*!
    \fn QCLBuffer::QCLBuffer(const QCLBuffer& other)

    Constructs a copy of \a other.
*/

/*!
    \fn QCLBuffer& QCLBuffer::operator=(const QCLBuffer& other)

    Assigns \a other to this object.
*/

/*!
    Reads \a size bytes from this buffer, starting at \a offset,
    into the supplied \a data array.  Returns true if the read
    was successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa readAsync(), write()
*/
bool QCLBuffer::read(size_t offset, void *data, size_t size)
{
    cl_int error = clEnqueueReadBuffer
        (context()->activeQueue(), id(),
         CL_TRUE, offset, size, data, 0, 0, 0);
    context()->reportError("QCLBuffer::read:", error);
    return error == CL_SUCCESS;
}

/*!
    \overload

    Reads \a size bytes from this buffer, starting at \a offset,
    into the supplied \a data array.  Returns true if the read
    was successful; false otherwise.

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.  The request is executed on
    the active command queue for context().

    \sa readAsync(), write()
*/
bool QCLBuffer::read(size_t offset, void *data, size_t size,
                     const QVector<QCLEvent>& after)
{
    cl_int error = clEnqueueReadBuffer
        (context()->activeQueue(), id(),
         CL_TRUE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0);
    context()->reportError("QCLBuffer::read(after):", error);
    return error == CL_SUCCESS;
}

/*!
    Reads \a size bytes from this buffer, starting at \a offset,
    into the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.  The request is executed on the active
    command queue for context().

    \sa read(), writeAsync()
*/
QCLEvent QCLBuffer::readAsync(size_t offset, void *data, size_t size)
{
    cl_event event;
    cl_int error = clEnqueueReadBuffer
        (context()->activeQueue(), id(),
         CL_FALSE, offset, size, data, 0, 0, &event);
    context()->reportError("QCLBuffer::readAsync:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    \overload

    Reads \a size bytes from this buffer, starting at \a offset,
    into the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa read(), writeAsync()
*/
QCLEvent QCLBuffer::readAsync(size_t offset, void *data, size_t size,
                              const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueReadBuffer
        (context()->activeQueue(), id(),
         CL_FALSE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLBuffer::readAsync(after):", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    Writes \a size bytes to this buffer, starting at \a offset,
    from the supplied \a data array.  Returns true if the write
    was successful; false otherwise.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa writeAsync(), read()
*/
bool QCLBuffer::write(size_t offset, const void *data, size_t size)
{
    cl_int error = clEnqueueWriteBuffer
        (context()->activeQueue(), id(),
         CL_TRUE, offset, size, data, 0, 0, 0);
    context()->reportError("QCLBuffer::write:", error);
    return error == CL_SUCCESS;
}

/*!
    \overload

    Writes \a size bytes to this buffer, starting at \a offset,
    from the supplied \a data array.  Returns true if the write
    was successful; false otherwise.

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.  The request is executed on
    the active command queue for context().

    \sa writeAsync(), read()
*/
bool QCLBuffer::write(size_t offset, const void *data, size_t size,
                      const QVector<QCLEvent>& after)
{
    cl_int error = clEnqueueWriteBuffer
        (context()->activeQueue(), id(),
         CL_TRUE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0);
    context()->reportError("QCLBuffer::write(after):", error);
    return error == CL_SUCCESS;
}

/*!
    Writes \a size bytes to this buffer, starting at \a offset,
    from the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.  The request is executed on the active
    command queue for context().

    \sa write(), readAsync()
*/
QCLEvent QCLBuffer::writeAsync(size_t offset, const void *data, size_t size)
{
    cl_event event;
    cl_int error = clEnqueueWriteBuffer
        (context()->activeQueue(), id(),
         CL_FALSE, offset, size, data, 0, 0, &event);
    context()->reportError("QCLBuffer::writeAsync:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    \overload

    Writes \a size bytes to this buffer, starting at \a offset,
    from the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa write(), readAsync()
*/
QCLEvent QCLBuffer::writeAsync(size_t offset, const void *data, size_t size,
                               const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueWriteBuffer
        (context()->activeQueue(), id(),
         CL_FALSE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLBuffer::writeAsync(after):", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    Requests that the \a size bytes at \a offset in this buffer
    be copied to \a destOffset in the buffer \a dest.  Returns an
    event object that can be used to wait for the request to complete.
    The request is executed on the active command queue for context().
*/
QCLEvent QCLBuffer::copyTo(size_t offset, size_t size,
                           const QCLBuffer& dest, size_t destOffset)
{
    cl_event event;
    cl_int error = clEnqueueCopyBuffer
        (context()->activeQueue(), id(), dest.id(),
         offset, destOffset, size, 0, 0, &event);
    context()->reportError("QCLBuffer::copyTo:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    \overload

    Requests that the \a size bytes at \a offset in this buffer
    be copied to \a destOffset in the buffer \a dest.  Returns an
    event object that can be used to wait for the request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().
*/
QCLEvent QCLBuffer::copyTo(size_t offset, size_t size,
                           const QCLBuffer& dest, size_t destOffset,
                           const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueCopyBuffer
        (context()->activeQueue(), id(), dest.id(),
         offset, destOffset, size, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLBuffer::copyTo(after):", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    Maps the \a size bytes starting at \a offset in this buffer
    into host memory for the specified \a access mode.  Returns a
    pointer to the mapped region.

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa mapAsync(), unmap()
*/
void *QCLBuffer::map
    (size_t offset, size_t size, QCLMemoryObject::MapAccess access)
{
    cl_int error;
    void *data = clEnqueueMapBuffer
        (context()->activeQueue(), id(), CL_TRUE,
         cl_map_flags(access), offset, size, 0, 0, 0, &error);
    context()->reportError("QCLBuffer::map:", error);
    return data;
}

/*!
    Maps the \a size bytes starting at \a offset in this buffer
    into host memory for the specified \a access mode.  Returns a
    pointer to the mapped region.

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.  The request is executed on
    the active command queue for context().

    \sa mapAsync(), unmap()
*/
void *QCLBuffer::map
    (size_t offset, size_t size, QCLMemoryObject::MapAccess access,
     const QVector<QCLEvent>& after)
{
    cl_int error;
    void *data = clEnqueueMapBuffer
        (context()->activeQueue(), id(), CL_TRUE,
         cl_map_flags(access), offset, size, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0, &error);
    context()->reportError("QCLBuffer::map(after):", error);
    return data;
}

/*!
    Maps the \a size bytes starting at \a offset in this buffer
    into host memory for the specified \a access mode.  Returns a
    pointer to the mapped region in \a ptr, which will be valid
    only after the request completes.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.  The request is executed on the active
    command queue for context().

    \sa map(), unmapAsync()
*/
QCLEvent QCLBuffer::mapAsync
    (void **ptr, size_t offset, size_t size,
     QCLMemoryObject::MapAccess access)
{
    cl_int error;
    cl_event event;
    *ptr = clEnqueueMapBuffer
        (context()->activeQueue(), id(), CL_TRUE,
         cl_map_flags(access), offset, size, 0, 0, &event, &error);
    context()->reportError("QCLBuffer::mapAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Maps the \a size bytes starting at \a offset in this buffer
    into host memory for the specified \a access mode.  Returns a
    pointer to the mapped region in \a ptr, which will be valid
    only after the request completes.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa map(), unmapAsync()
*/
QCLEvent QCLBuffer::mapAsync
    (void **ptr, size_t offset, size_t size,
     QCLMemoryObject::MapAccess access, const QVector<QCLEvent>& after)
{
    cl_int error;
    cl_event event;
    *ptr = clEnqueueMapBuffer
        (context()->activeQueue(), id(), CL_TRUE,
         cl_map_flags(access), offset, size, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()),
         &event, &error);
    context()->reportError("QCLBuffer::mapAsync(after):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Unmaps the region at \a ptr that was previously returned from
    a call to map() or mapAsync().

    This function will block until the request completes.
    The request is executed on the active command queue for context().

    \sa unmapAsync(), map()
*/
void QCLBuffer::unmap(void *ptr)
{
    cl_int error = clEnqueueUnmapMemObject
        (context()->activeQueue(), id(), ptr, 0, 0, 0);
    context()->reportError("QCLBuffer::unmap:", error);
}

/*!
    Unmaps the region at \a ptr that was previously returned from
    a call to map() or mapAsync().

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.  The request is executed on
    the active command queue for context().

    \sa unmapAsync(), map()
*/
void QCLBuffer::unmap(void *ptr, const QVector<QCLEvent>& after)
{
    cl_int error = clEnqueueUnmapMemObject
        (context()->activeQueue(), id(), ptr, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0);
    context()->reportError("QCLBuffer::unmap(after):", error);
}

/*!
    Unmaps the region at \a ptr that was previously returned from
    a call to map() or mapAsync().

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.  The request is executed on the active
    command queue for context().

    \sa unmap(), mapAsync()
*/
QCLEvent QCLBuffer::unmapAsync(void *ptr)
{
    cl_event event;
    cl_int error = clEnqueueUnmapMemObject
        (context()->activeQueue(), id(), ptr, 0, 0, &event);
    context()->reportError("QCLBuffer::unmapAsync:", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

/*!
    Unmaps the region at \a ptr that was previously returned from
    a call to map() or mapAsync().

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    \sa unmap(), mapAsync()
*/
QCLEvent QCLBuffer::unmapAsync(void *ptr, const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueUnmapMemObject
        (context()->activeQueue(), id(), ptr, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLBuffer::unmapAsync(after):", error);
    if (error == CL_SUCCESS)
        return QCLEvent(event);
    else
        return QCLEvent();
}

QT_END_NAMESPACE
