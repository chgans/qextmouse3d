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
#include <QtCore/qvarlengtharray.h>

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
    \fn QCLKernel::QCLKernel(QCLContext *context, cl_kernel id)

    Constructs an OpenCL kernel object from the native identifier \a id,
    and associates it with \a context.  This class will take over
    ownership of \a id and release it in the destructor.
*/

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
    Returns the work group size that was declared in the kernel's
    source code using a \c{reqd_work_group_size} qualifier.
    Returns (0, 0, 0) if the size is not declared.

    The default device for context() is used to retrieve the
    work group size.
*/
QCLWorkSize QCLKernel::declaredWorkGroupSize() const
{
    size_t sizes[3];
    if (clGetKernelWorkGroupInfo
            (m_id, context()->defaultDevice().id(),
             CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
             sizeof(sizes), sizes, 0) != CL_SUCCESS)
        return QCLWorkSize(0, 0, 0);
    else
        return QCLWorkSize(sizes[0], sizes[1], sizes[2]);
}

/*!
    \overload

    Returns the work group size that was declared in the kernel's
    source code using a \c{reqd_work_group_size} qualifier.
    Returns (0, 0, 0) if the size is not declared.

    The specified \a device is used to retrieve the work group size.
*/
QCLWorkSize QCLKernel::declaredWorkGroupSize(const QCLDevice& device) const
{
    size_t sizes[3];
    if (clGetKernelWorkGroupInfo
            (m_id, device.id(),
             CL_KERNEL_COMPILE_WORK_GROUP_SIZE,
             sizeof(sizes), sizes, 0) != CL_SUCCESS)
        return QCLWorkSize(0, 0, 0);
    else
        return QCLWorkSize(sizes[0], sizes[1], sizes[2]);
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
void QCLKernel::setArg(int index, float value)
{
    clSetKernelArg(m_id, index, sizeof(value), &value);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, const QCLMemoryObject& value)
{
    cl_mem id = value.id();
    clSetKernelArg(m_id, index, sizeof(id), &id);
}

/*!
    Sets argument \a index for this kernel to \a value.
*/
void QCLKernel::setArg(int index, const QCLSampler& value)
{
    cl_sampler id = value.id();
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
    Requests that this kernel be executed on \a globalWorkSize items.
    Returns an event object that can be use to wait for the kernel
    to finish execution.  The request is executed on the active
    command queue for context().
*/
QCLEvent QCLKernel::execute(const QCLWorkSize& globalWorkSize)
{
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (context()->activeQueue(), m_id, globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), 0, 0, 0, &event);
    context()->reportError("QCLKernel::execute:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    Requests that this kernel be executed on \a globalWorkSize items,
    which are subdivided into local work items of \a localWorkSize in size.
    Returns an event object that can be use to wait for the kernel
    to finish execution.  The request is executed on the active
    command queue for context().

    The \a globalWorkSize must have the same number of dimensions as
    \a localWorkSize, and be evenly divisible by \a localWorkSize.
*/
QCLEvent QCLKernel::execute
    (const QCLWorkSize& globalWorkSize, const QCLWorkSize& localWorkSize)
{
    Q_ASSERT(globalWorkSize.dimensions() == localWorkSize.dimensions());
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (context()->activeQueue(), m_id, globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), localWorkSize.sizes(), 0, 0, &event);
    context()->reportError("QCLKernel::execute:", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    \overload

    Requests that this kernel be executed on \a globalWorkSize items.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().
*/
QCLEvent QCLKernel::execute
    (const QCLWorkSize& globalWorkSize, const QVector<QCLEvent>& after)
{
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (context()->activeQueue(), m_id, globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), 0, after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLKernel::execute(after):", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

/*!
    \overload

    Requests that this kernel be executed on \a globalWorkSize items,
    which are subdivided into local work items of \a localWorkSize in size.
    Returns an event object that can be use to wait for the kernel
    to finish execution.

    The request will not start until all of the events in \a after
    have been signalled as completed.  The request is executed on
    the active command queue for context().

    The \a globalWorkSize must have the same number of dimensions as
    \a localWorkSize, and be evenly divisible by \a localWorkSize.
*/
QCLEvent QCLKernel::execute
    (const QCLWorkSize& globalWorkSize, const QCLWorkSize& localWorkSize,
     const QVector<QCLEvent>& after)
{
    Q_ASSERT(globalWorkSize.dimensions() == localWorkSize.dimensions());
    cl_event event;
    cl_int error = clEnqueueNDRangeKernel
        (context()->activeQueue(), m_id, globalWorkSize.dimensions(),
         0, globalWorkSize.sizes(), localWorkSize.sizes(), after.size(),
         reinterpret_cast<const cl_event *>(after.constData()), &event);
    context()->reportError("QCLKernel::execute(after):", error);
    if (error != CL_SUCCESS)
        return QCLEvent();
    else
        return QCLEvent(event);
}

QT_END_NAMESPACE
