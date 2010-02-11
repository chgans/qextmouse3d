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

#include "qclcommandqueue.h"
#include "qclerrors.h"
#include "qclbuffer.h"
#include "qclkernel.h"
#include <QtCore/qdebug.h>
#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLCommandQueue
    \brief The QCLCommandQueue class represents an OpenCL command queue on a QCLContext.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

/*!
    \fn QCLCommandQueue::QCLCommandQueue()

    Constructs a null OpenCL command queue object.
*/

/*!
    Constructs an OpenCL command queue object based on the supplied
    native OpenCL \a id.  This constructor will retain a copy of \a id,
    to be released when the object is destroyed.
*/
QCLCommandQueue::QCLCommandQueue(cl_command_queue id)
    : m_id(id)
{
    if (id)
        clRetainCommandQueue(id);
}

/*!
    Constructs a copy of \a other.
*/
QCLCommandQueue::QCLCommandQueue(const QCLCommandQueue& other)
    : m_id(other.m_id)
{
    if (m_id)
        clRetainCommandQueue(m_id);
}

/*!
    Releases this OpenCL command queue.  If this object is the
    last reference, the queue will be destroyed.
*/
QCLCommandQueue::~QCLCommandQueue()
{
    if (m_id)
        clReleaseCommandQueue(m_id);
}

/*!
    Assigns \a other to this object.
*/
QCLCommandQueue& QCLCommandQueue::operator=(const QCLCommandQueue& other)
{
    if (m_id == other.m_id)
        return *this;
    if (m_id)
        clReleaseCommandQueue(m_id);
    m_id = other.m_id;
    if (m_id)
        clRetainCommandQueue(m_id);
    return *this;
}

/*!
    \fn bool QCLCommandQueue::isNull() const

    Returns true if this OpenCL command queue is null.
*/

/*!
    Returns true if this command queue executes commands out of order;
    otherwise false if commands are executed in order.

    \sa setOutOfOrder()
*/
bool QCLCommandQueue::isOutOfOrder() const
{
    if (!m_id)
        return false;
    cl_command_queue_properties props = 0;
    if (clGetCommandQueueInfo(m_id, CL_QUEUE_PROPERTIES,
                              sizeof(props), &props, 0) != CL_SUCCESS)
        return false;
    return (props & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE) != 0;
}

/*!
    Enables or disables out of order execution of commands according
    to \a enable.

    \sa isOutOfOrder()
*/
void QCLCommandQueue::setOutOfOrder(bool enable)
{
    if (!m_id)
        return;
    clSetCommandQueueProperty(m_id, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                              (enable ? CL_TRUE : CL_FALSE), 0);
}

/*!
    \fn cl_command_queue QCLCommandQueue::id() const

    Returns the native OpenCL command queue identifier for this object.
*/

/*!
    Flushes all previously queued commands to the device associated
    with this command queue.  The commands are delivered to the device,
    but no guarantees are given that they will be executed.

    \sa finish()
*/
void QCLCommandQueue::flush()
{
    if (m_id)
        clFlush(m_id);
}

/*!
    Blocks until all previously queued commands have finished execution.

    \sa flush()
*/
void QCLCommandQueue::finish()
{
    if (m_id)
        clFinish(m_id);
}

/*!
    Returns a marker event for this command queue.  The event will
    be signalled when all commands that were queued before this
    point have been completed.

    \sa barrier()
*/
QCLEvent QCLCommandQueue::marker()
{
    cl_event evid;
    if (!m_id)
        return QCLEvent();
    cl_int error = clEnqueueMarker(m_id, &evid);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::marker:" << QCL::errorName(error);
        return QCLEvent();
    }
    return QCLEvent(evid);
}

/*!
    Adds a barrier to this command queue.  All commands that were
    queued before this point must complete before any further
    commands added after this point are executed.

    \sa marker()
*/
void QCLCommandQueue::barrier()
{
    if (!m_id)
        return;
    cl_int error = clEnqueueBarrier(m_id);
    if (error != CL_SUCCESS)
        qWarning() << "QCLCommandQueue::barrier:" << QCL::errorName(error);
}

/*!
    Adds a barrier to this command queue that will prevent future
    commands from being executed until after \a event is signalled.

    \sa marker()
*/
void QCLCommandQueue::barrier(const QCLEvent& event)
{
    cl_event evtid = event.id();
    if (!m_id || !evtid)
        return;
    cl_int error = clEnqueueWaitForEvents(m_id, 1, &evtid);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::barrier(QCLEvent):"
                   << QCL::errorName(error);
    }
}

/*!
    Adds a barrier to this command queue that will prevent future
    commands from being executed until after all members of \a events
    have been signalled.

    \sa marker()
*/
void QCLCommandQueue::barrier(const QVector<QCLEvent>& events)
{
    if (!m_id || events.isEmpty())
        return;
    cl_int error = clEnqueueWaitForEvents
        (m_id, events.size(),
         reinterpret_cast<const cl_event *>(events.constData()));
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::barrier(QVector<QCLEvent>):"
                   << QCL::errorName(error);
    }
}

/*!
    Reads \a size bytes from \a buffer, starting at \a offset,
    into the supplied \a data array.  Returns true if the read
    was successful; false otherwise.

    This function will block until the request completes.

    \sa readBufferAsync(), QCLBuffer::read(), writeBuffer()
*/
bool QCLCommandQueue::readBuffer
    (const QCLBuffer& buffer, size_t offset, void *data, size_t size)
{
    cl_int error = clEnqueueReadBuffer
        (m_id, buffer.id(), CL_TRUE, offset, size, data, 0, 0, 0);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::readBuffer:"
                   << QCL::errorName(error);
    }
    return error == CL_SUCCESS;
}

/*!
    \overload

    Reads \a size bytes from \a buffer, starting at \a offset,
    into the supplied \a data array.  Returns true if the read
    was successful; false otherwise.

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.

    \sa readBufferAsync(), QCLBuffer::read(), writeBuffer()
*/
bool QCLCommandQueue::readBuffer
    (const QCLBuffer& buffer, size_t offset, void *data, size_t size,
     const QVector<QCLEvent>& after)
{
    cl_int error = clEnqueueReadBuffer
        (m_id, buffer.id(), CL_TRUE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::readBuffer(after):"
                   << QCL::errorName(error);
    }
    return error == CL_SUCCESS;
}

/*!
    Reads \a size bytes from \a buffer, starting at \a offset,
    into the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    \sa readBuffer(), QCLBuffer::readAsync(), writeBufferAsync()
*/
QCLEvent QCLCommandQueue::readBufferAsync
    (const QCLBuffer& buffer, size_t offset, void *data, size_t size)
{
    cl_event event;
    cl_int error = clEnqueueReadBuffer
        (m_id, buffer.id(), CL_FALSE, offset, size, data, 0, 0, &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::readBufferAsync:"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    \overload

    Reads \a size bytes from \a buffer, starting at \a offset,
    into the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.

    \sa readBuffer(), QCLBuffer::readAsync(), writeBufferAsync()
*/
QCLEvent QCLCommandQueue::readBufferAsync
    (const QCLBuffer& buffer, size_t offset, void *data,
     size_t size, const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueReadBuffer
        (m_id, buffer.id(), CL_FALSE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::readBufferAsync(after):"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    Writes \a size bytes to \a buffer, starting at \a offset,
    from the supplied \a data array.  Returns true if the write
    was successful; false otherwise.

    This function will block until the request completes.

    \sa writeBufferAsync(), QCLBuffer::write(), readBuffer()
*/
bool QCLCommandQueue::writeBuffer
    (const QCLBuffer& buffer, size_t offset, const void *data, size_t size)
{
    cl_int error = clEnqueueWriteBuffer
        (m_id, buffer.id(), CL_TRUE, offset, size, data, 0, 0, 0);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::writeBuffer:"
                   << QCL::errorName(error);
    }
    return error == CL_SUCCESS;
}

/*!
    \overload

    Writes \a size bytes to \a buffer, starting at \a offset,
    from the supplied \a data array.  Returns true if the write
    was successful; false otherwise.

    The request will not start until all of the events in \a after
    have been signalled as completed, and then this function will
    block until the request completes.

    \sa writeBufferAsync(), QCLBuffer::write(), readBuffer()
*/
bool QCLCommandQueue::writeBuffer
    (const QCLBuffer& buffer, size_t offset, const void *data,
     size_t size, const QVector<QCLEvent>& after)
{
    cl_int error = clEnqueueWriteBuffer
        (m_id, buffer.id(), CL_TRUE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), 0);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::writeBuffer(after):"
                   << QCL::errorName(error);
    }
    return error == CL_SUCCESS;
}

/*!
    Writes \a size bytes to \a buffer, starting at \a offset,
    from the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    \sa writeBuffer(), QCLBuffer::writeAsync(), readBufferAsync()
*/
QCLEvent QCLCommandQueue::writeBufferAsync
    (const QCLBuffer& buffer, size_t offset, const void *data, size_t size)
{
    cl_event event;
    cl_int error = clEnqueueWriteBuffer
        (m_id, buffer.id(), CL_FALSE, offset, size, data, 0, 0, &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::writeBufferAsync:"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    \overload

    Writes \a size bytes to a\ buffer, starting at \a offset,
    from the supplied \a data array.

    This function will queue the request and return immediately.
    Returns an event object that can be used to wait for the
    request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.

    \sa writeBuffer(), QCLBuffer::writeAsync(), readBufferAsync()
*/
QCLEvent QCLCommandQueue::writeBufferAsync
    (const QCLBuffer& buffer, size_t offset, const void *data,
     size_t size, const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueWriteBuffer
        (m_id, buffer.id(), CL_FALSE, offset, size, data, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::writeBufferAsync(after):"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    Requests that the \a size bytes at \a offset in the buffer \a src
    be copied to \a destOffset in the buffer \a dest.  Returns an
    event object that can be used to wait for the request to complete.

    \sa QCLBuffer::copyTo()
*/
QCLEvent QCLCommandQueue::copyBufferToBuffer
    (const QCLBuffer& src, size_t offset, size_t size,
     const QCLBuffer& dest, size_t destOffset)
{
    cl_event event;
    cl_int error = clEnqueueCopyBuffer
        (m_id, src.id(), dest.id(), offset, destOffset, size, 0, 0, &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::copyBufferToBuffer:"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    \overload

    Requests that the \a size bytes at \a offset in the buffer \a src
    be copied to \a destOffset in the buffer \a dest.  Returns an
    event object that can be used to wait for the request to complete.

    The request will not start until all of the events in \a after
    have been signalled as completed.

    \sa QCLBuffer::copyTo()
*/
QCLEvent QCLCommandQueue::copyBufferToBuffer
    (const QCLBuffer& src, size_t offset, size_t size,
     const QCLBuffer& dest, size_t destOffset,
     const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueCopyBuffer
        (m_id, src.id(), dest.id(), offset, destOffset, size, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::copyBufferToBuffer(after):"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    Requests that \a kernel be executed on \a globalWorkSize items.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    \sa QCLKernel::execute()
*/
QCLEvent QCLCommandQueue::executeKernel
    (const QCLKernel& kernel, const QCLWorkSize& globalWorkSize)
{
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (m_id, kernel.id(), globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), 0, 0, 0, &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::executeKernel:"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    Requests that \a kernel be executed on \a globalWorkSize items,
    which are subdivided into local work items of \a localWorkSize in size.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    The \a globalWorkSize must have the same number of dimensions as
    \a localWorkSize, and be evenly divisible by \a localWorkSize.

    \sa QCLKernel::execute()
*/
QCLEvent QCLCommandQueue::executeKernel
    (const QCLKernel& kernel, const QCLWorkSize& globalWorkSize,
     const QCLWorkSize& localWorkSize)
{
    Q_ASSERT(globalWorkSize.dimensions() == localWorkSize.dimensions());
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (m_id, kernel.id(), globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), localWorkSize.sizes(), 0, 0, &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::executeKernel:"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    \overload

    Requests that \a kernel be executed on \a globalWorkSize items.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    The request will not start until all of the events in \a after
    have been signalled as completed.

    \sa QCLKernel::execute()
*/
QCLEvent QCLCommandQueue::executeKernel
    (const QCLKernel& kernel, const QCLWorkSize& globalWorkSize,
     const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (m_id, kernel.id(), globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), 0, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::execute(after):"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    \overload

    Requests that \a kernel be executed on \a globalWorkSize items,
    which are subdivided into local work items of \a localWorkSize in size.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    The request will not start until all of the events in \a after
    have been signalled as completed.

    The \a globalWorkSize must have the same number of dimensions as
    \a localWorkSize, and be evenly divisible by \a localWorkSize.

    \sa QCLKernel::execute()
*/
QCLEvent QCLCommandQueue::executeKernel
    (const QCLKernel& kernel, const QCLWorkSize& globalWorkSize,
     const QCLWorkSize& localWorkSize, const QVector<QCLEvent>& after)
{
    Q_ASSERT(globalWorkSize.dimensions() == localWorkSize.dimensions());
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (m_id, kernel.id(), globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), localWorkSize.sizes(), after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLCommandQueue::execute(after):"
                   << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

QT_END_NAMESPACE
