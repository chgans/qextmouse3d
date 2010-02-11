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

#include "qclevent.h"
#include "qclcommandqueue.h"
#include "qclerrors.h"
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLEvent
    \brief The QCLEvent class represents an OpenCL event object.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

/*!
    \fn QCLEvent::QCLEvent()

    Constructs a null OpenCL event object.
*/

/*!
    \fn QCLEvent::QCLEvent(cl_event id)

    Constructs an OpenCL event object from the native identifier \a id.

    This constructor does not call \c{clRetainEvent()}, which makes it
    suitable for wrapping a native identifier that was returned from a
    native OpenCL function.
*/

/*!
    Constructs a copy of \a other.  The \c{clRetainEvent()} function
    will be called to update the reference count on id().
*/
QCLEvent::QCLEvent(const QCLEvent& other)
    : m_id(other.m_id)
{
    if (m_id)
        clRetainEvent(m_id);
}

/*!
    Releases this OpenCL event object by calling \c{clReleaseEvent()}.
*/
QCLEvent::~QCLEvent()
{
    if (m_id)
        clReleaseEvent(m_id);
}

/*!
    Assigns \a other to this OpenCL event object.  The current id() will
    be released with \c{clReleaseEvent()}, and the new id() will be
    retained with \c{clRetainEvent()}.
*/
QCLEvent& QCLEvent::operator=(const QCLEvent& other)
{
    if (m_id != other.m_id) {
        if (m_id)
            clReleaseEvent(m_id);
        m_id = other.m_id;
        if (m_id)
            clRetainEvent(m_id);
    }
    return *this;
}

/*!
    \fn bool QCLEvent::isNull() const

    Returns true if this OpenCL event object is null; false otherwise.
*/

/*!
    \fn cl_event QCLEvent::id() const

    Returns the native OpenCL identifier for this event.
*/

/*!
    \fn bool QCLEvent::isQueued() const

    Returns true if the command associated with this OpenCL event has been
    queued for execution on the host, but has not yet been submitted to
    the device yet.

    \sa isSubmitted(), isRunning(), isComplete(), status()
*/

/*!
    \fn bool QCLEvent::isSubmitted() const

    Returns true if the command associated with this OpenCL event has been
    submitted for execution on the device yet, but is not yet running.

    \sa isQueued(), isRunning(), isComplete(), status()
*/

/*!
    \fn bool QCLEvent::isRunning() const

    Returns true if the command associated with this OpenCL event is
    running on the device, but has not yet completed.

    \sa isQueued(), isSubmitted(), isComplete(), status()
*/

/*!
    \fn bool QCLEvent::isComplete() const

    Returns true if the command associated with this OpenCL event
    has completed execution on the device.

    \sa isQueued(), isSubmitted(), isRunning(), status()
*/

/*!
    Returns the execution status of this event, usually one of
    \c{CL_QUEUED}, \c{CL_SUBMITTED}, \c{CL_RUNNING}, \c{CL_COMPLETE},
    or a negative error code.

    \sa isQueued(), isSubmitted(), isRunning(), isComplete()
*/
int QCLEvent::status() const
{
    if (!m_id)
        return CL_INVALID_EVENT;
    cl_int st, error;
    error = clGetEventInfo(m_id, CL_EVENT_COMMAND_EXECUTION_STATUS,
                           sizeof(st), &st, 0);
    if (error != CL_SUCCESS)
        return error;
    else
        return st;
}

/*!
    Waits for this event to be signalled as complete.  The calling thread
    is blocked until the event is signalled.  This function returns
    immediately if the event is null.

    \sa waitForEvents(), status()
*/
void QCLEvent::wait()
{
    if (m_id) {
        cl_int error = clWaitForEvents(1, &m_id);
        if (error != CL_SUCCESS)
            qWarning() << "QCLEvent::wait:" << QCL::errorName(error);
    }
}

/*!
    Waits for all of the listed \a events to be signalled as complete.
    The calling thread is blocked until all of the \a events are signalled.

    If \a events is empty, then this function returns immediately.

    \sa wait(), status()
*/
void QCLEvent::waitForEvents(const QVector<QCLEvent>& events)
{
    if (events.isEmpty())
        return;
    cl_int error = clWaitForEvents
        (events.size(),
         reinterpret_cast<const cl_event *>(events.constData()));
    if (error != CL_SUCCESS)
        qWarning() << "QCLEvent::waitForEvents:" << QCL::errorName(error);
}

QT_END_NAMESPACE
