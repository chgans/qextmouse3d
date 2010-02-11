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

#include "qclprogram.h"
#include "qclerrors.h"
#include <QtCore/qdebug.h>
#include <QtCore/qvarlengtharray.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLProgram
    \brief The QCLProgram class represents an OpenCL program object.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

/*!
    \fn QCLProgram::QCLProgram()

    Constructs a null OpenCL program object.
*/

/*!
    \fn QCLProgram::QCLProgram(QCLContext *context, cl_program id)

    Constructs an OpenCL program object from the native identifier \a id,
    and associates it with \a context.  This class will take over ownership
    of \a id and will release it in the destructor.
*/

/*!
    Constructs a copy of \a other.
*/
QCLProgram::QCLProgram(const QCLProgram& other)
    : m_context(other.m_context), m_id(other.m_id)
{
    if (m_id)
        clRetainProgram(m_id);
}

/*!
    Releases this OpenCL program object.  If this is the last
    reference to the program, it will be destroyed.
*/
QCLProgram::~QCLProgram()
{
    if (m_id)
        clReleaseProgram(m_id);
}

/*!
    Assigns \a other to this object.
*/
QCLProgram& QCLProgram::operator=(const QCLProgram& other)
{
    m_context = other.m_context;
    if (m_id == other.m_id)
        return *this;
    if (m_id)
        clReleaseProgram(m_id);
    m_id = other.m_id;
    if (m_id)
        clRetainProgram(m_id);
    return *this;
}

/*!
    \fn bool QCLProgram::isNull() const

    Returns null if this OpenCL program object is null.
*/

/*!
    \fn cl_program QCLProgram::id() const

    Returns the native OpenCL identifier for this program.
*/

/*!
    \fn QCLContext *QCLProgram::context() const

    Returns the OpenCL context that this program was created within.
*/

/*!
    Builds this program from the sources and binaries that were supplied,
    with the specified compiler \a options.

    Returns true if the program was built; false otherwise.

    \sa log(), createKernel()
*/
bool QCLProgram::build(const QString& options)
{
    return build(QList<QCLDevice>(), options);
}

/*!
    \overload

    Builds this program from the sources and binaries that were supplied,
    with the specified compiler \a options.

    If \a devices is not empty, the program will only be built for devices
    in the specified list.  Otherwise the program will be built for all
    devices on the program's context.

    Returns true if the program was built; false otherwise.

    \sa log(), createKernel()
*/
bool QCLProgram::build(const QList<QCLDevice>& devices, const QString& options)
{
    QVarLengthArray<cl_device_id> devs;
    foreach (QCLDevice dev, devices) {
        if (dev.id())
            devs.append(dev.id());
    }
    cl_int error;
    if (devs.isEmpty()) {
        error = clBuildProgram
            (m_id, 0, 0,
             options.isEmpty() ? 0 : options.toLatin1().constData(), 0, 0);
    } else {
        error = clBuildProgram
            (m_id, devs.size(), devs.constData(),
             options.isEmpty() ? 0 : options.toLatin1().constData(), 0, 0);
    }
    if (error == CL_SUCCESS)
        return true;
    qWarning() << "QCLProgram::build:" << QCL::errorName(error);
    qWarning() << log();
    return false;
}

/*!
    Returns the error log that resulted from the last build().

    \sa build()
*/
QString QCLProgram::log() const
{
    // Get the devices associated with the program so we can
    // retrieve the log for each one.
    cl_uint numDevs = 0;
    if (clGetProgramInfo(m_id, CL_PROGRAM_NUM_DEVICES,
                         sizeof(numDevs), &numDevs, 0) != CL_SUCCESS)
        return QString();
    if (!numDevs)
        return QString();
    QVarLengthArray<cl_device_id> devs(numDevs);
    if (clGetProgramInfo(m_id, CL_PROGRAM_NUM_DEVICES,
                         numDevs * sizeof(cl_device_id),
                         devs.data(), 0) != CL_SUCCESS)
        return QString();

    // Retrieve the device logs and concatenate them.
    QString log;
    for (cl_uint index = 0; index < numDevs; ++index) {
        size_t size = 0;
        if (clGetProgramBuildInfo
                (m_id, devs[index], CL_PROGRAM_BUILD_LOG,
                 0, 0, &size) != CL_SUCCESS || !size)
            continue;
        QVarLengthArray<char> buf(size);
        if (clGetProgramBuildInfo
                (m_id, devs[index], CL_PROGRAM_BUILD_LOG,
                 size, buf.data(), 0) != CL_SUCCESS || !size)
            continue;
        log += QString::fromLatin1(buf.constData(), size);
    }
    return log;
}

/*!
    Creates a kernel for the entry point associated with \a name
    in this program.

    \sa build()
*/
QCLKernel QCLProgram::createKernel(const char *name) const
{
    cl_kernel kernel = clCreateKernel(m_id, name, 0);
    if (kernel)
        return QCLKernel(m_context, kernel);
    else
        return QCLKernel();
}

/*!
    \overload
*/
QCLKernel QCLProgram::createKernel(const QByteArray& name) const
{
    return createKernel(name.constData());
}

/*!
    \overload
*/
QCLKernel QCLProgram::createKernel(const QString& name) const
{
    return createKernel(name.toLatin1().constData());
}

/*!
    Creates a list of kernels for all of the entry points in this program.
*/
QList<QCLKernel> QCLProgram::createKernels() const
{
    QList<QCLKernel> list;
    cl_uint numKernels = 0;
    if (clCreateKernelsInProgram(m_id, 0, 0, &numKernels) != CL_SUCCESS)
        return list;
    QVarLengthArray<cl_kernel> kerns(numKernels);
    if (clCreateKernelsInProgram
            (m_id, numKernels, kerns.data(), 0) != CL_SUCCESS)
        return list;
    for (cl_uint index = 0; index < numKernels; ++index)
        list.append(QCLKernel(m_context, kerns[index]));
    return list;
}

/*!
    Releases the resources associated with the OpenCL compiler.
*/
void QCLProgram::unloadCompiler()
{
    clUnloadCompiler();
}

QT_END_NAMESPACE
