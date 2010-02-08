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

#include "qclkernel.h"
#include "qclprogram.h"
#include "qclbuffer.h"
#include "qclcontext.h"
#include "qclerrors.h"
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLKernel
    \brief The QCLKernel class represents an executable entry point function in an OpenCL program.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl

    \sa QCLProgram
*/

/*!
    \fn QCLKernel::QCLKernel()

    Constructs a null OpenCL kernel object.
*/

/*!
    Constructs an OpenCL kernel object from the native identifier \a id,
    and associates it with \a context.
*/
QCLKernel::QCLKernel(QCLContext *context, cl_kernel id)
    : m_context(context), m_id(id)
{
    if (m_id)
        clRetainKernel(m_id);
}

/*!
    Constructs a copy of \a other.
*/
QCLKernel::QCLKernel(const QCLKernel& other)
    : m_context(other.m_context), m_id(other.m_id)
{
    if (m_id)
        clRetainKernel(m_id);
}

/*!
    Releases this OpenCL kernel object.  If this is the last
    reference to the kernel, it will be destroyed.
*/
QCLKernel::~QCLKernel()
{
    if (m_id)
        clReleaseKernel(m_id);
}

/*!
    Assigns \a other to this object.
*/
QCLKernel& QCLKernel::operator=(const QCLKernel& other)
{
    m_context = other.m_context;
    if (m_id == other.m_id)
        return *this;
    if (m_id)
        clReleaseKernel(m_id);
    m_id = other.m_id;
    if (m_id)
        clRetainKernel(m_id);
    return *this;
}

/*!
    \fn bool QCLKernel::isNull() const

    Returns true if this OpenCL kernel object is null; false otherwise.
*/

/*!
    \fn cl_kernel QCLKernel::id() const

    Returns the native OpenCL identifier for this kernel.
*/

/*!
    \fn QCLContext *QCLKernel::context() const

    Returns the OpenCL context that this kernel was created within.
*/

/*!
    Returns the OpenCL program that this kernel is associated with.
*/
QCLProgram QCLKernel::program() const
{
    if (!m_id)
        return QCLProgram();
    cl_program prog = 0;
    if (clGetKernelInfo(m_id, CL_KERNEL_PROGRAM,
                        sizeof(prog), &prog, 0) != CL_SUCCESS)
        return QCLProgram();
    return QCLProgram(m_context, prog);
}

/*!
    Returns the name of this OpenCL kernel's entry point function.
*/
QString QCLKernel::name() const
{
    size_t size = 0;
    if (clGetKernelInfo(m_id, CL_KERNEL_FUNCTION_NAME,
                        0, 0, &size) != CL_SUCCESS || !size)
        return QString();
    QVarLengthArray<char> buf(size);
    if (clGetKernelInfo(m_id, CL_KERNEL_FUNCTION_NAME,
                        size, buf.data(), 0) != CL_SUCCESS)
        return QString();
    return QString::fromLatin1(buf.constData(), size);
}

/*!
    Returns the number of arguments that are expected by
    this OpenCL kernel.

    \sa setArg()
*/
int QCLKernel::argCount() const
{
    cl_uint count = 0;
    if (clGetKernelInfo(m_id, CL_KERNEL_NUM_ARGS, sizeof(count), &count, 0)
            != CL_SUCCESS)
        return 0;
    return int(count);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_int value)
{
    clSetKernelArg(m_id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_uint value)
{
    clSetKernelArg(m_id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_long value)
{
    clSetKernelArg(m_id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, cl_ulong value)
{
    clSetKernelArg(m_id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, const QCLBuffer& value)
{
    cl_mem id = value.id();
    clSetKernelArg(m_id, index, sizeof(id), &id);
}

/*!
    Sets argument \a index to the \a size bytes at \a data.
*/
void QCLKernel::setArg(int index, const void *data, size_t size)
{
    clSetKernelArg(m_id, index, size, data);
}

/*!
    Requests that this kernel be executed on \a workSize items.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    The request is executed on the default command queue for context();
    use QCLCommandQueue::executeKernel() to execute the request on a
    different command queue.

    \sa QCLCommandQueue::executeKernel()
*/
QCLEvent QCLKernel::execute(size_t workSize)
{
    cl_event event;
    size_t sizes[1] = {workSize};
    cl_int error = clEnqueueNDRangeKernel
        (context()->defaultQueue(), m_id, 1, 0, sizes, 0, 0, 0, &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLKernel::execute:" << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

/*!
    \overload

    Requests that this kernel be executed on \a workSize items.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    The request will not start until all of the events in \a after
    have been signalled as completed.

    The request is executed on the default command queue for context();
    use QCLCommandQueue::executeKernel() to execute the request on a
    different command queue.

    \sa QCLCommandQueue::executeKernel()
*/
QCLEvent QCLKernel::execute(size_t workSize, const QVector<QCLEvent>& after)
{
    cl_event event;
    size_t sizes[1] = {workSize};
    cl_int error = clEnqueueNDRangeKernel
        (context()->defaultQueue(), m_id, 1, 0, sizes, 0, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    if (error != CL_SUCCESS) {
        qWarning() << "QCLKernel::execute(after):" << QCL::errorName(error);
        return QCLEvent();
    } else {
        return QCLEvent(event);
    }
}

QT_END_NAMESPACE
