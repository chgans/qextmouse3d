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

#include "qclcontext.h"
#include "qclerrors.h"
#include <QtCore/qdebug.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qfile.h>

QT_BEGIN_NAMESPACE

/*!
    \class QCLContext
    \brief The QCLContext class represents an OpenCL context.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

class QCLContextPrivate
{
public:
    QCLContextPrivate()
        : id(0)
        , createStatus(CL_INVALID_CONTEXT)
        , lastError(CL_SUCCESS)
    {
    }
    ~QCLContextPrivate()
    {
        // Release the default command queue for the context.
        defaultCommandQueue = QCLCommandQueue();

        // Release the context.
        if (createStatus == CL_SUCCESS)
            clReleaseContext(id);
    }

    cl_context id;
    QCLPlatform platform;
    QCLCommandQueue defaultCommandQueue;
    cl_int createStatus;
    cl_int lastError;

    void setLastError(const char *name, cl_int error);
};

void QCLContextPrivate::setLastError(const char *name, cl_int error)
{
    lastError = error;
    if (error != CL_SUCCESS)
        qWarning() << name << QCL::errorName(error);
}

/*!
    Constructs a new OpenCL context object.  This constructor is
    typically followed by calls to setPlatform() and create().
*/
QCLContext::QCLContext()
    : d_ptr(new QCLContextPrivate())
{
}

/*!
    Destroys this OpenCL context object.  If the underlying
    context id() has been created, then it will be released.
*/
QCLContext::~QCLContext()
{
}

/*!
    Returns true if the underlying OpenCL context id() has been
    created; false otherwise.

    \sa create(), setId()
*/
bool QCLContext::isCreated() const
{
    Q_D(const QCLContext);
    return d->createStatus == CL_SUCCESS;
}

/*!
    Returns the platform that the context will be created within
    when create() is called.  The default is a null QCLPlatform,
    which indicates that no specific platform is required.

    After the context has been created, the actual platform in use
    by the context can be retrieved by calling QCLDevice::platform()
    on defaultDevice().

    \sa setPlatform(), create()
*/
QCLPlatform QCLContext::platform() const
{
    Q_D(const QCLContext);
    return d->platform;
}

/*!
    Sets the \a platform to be used when the context is created
    by create().

    \sa platform(), create()
*/
void QCLContext::setPlatform(const QCLPlatform& platform)
{
    Q_D(QCLContext);
    d->platform = platform;
}

extern "C" {

static void qt_cl_context_notify(const char *errinfo,
                                 const void *private_info,
                                 size_t cb,
                                 void *user_data)
{
    Q_UNUSED(private_info);
    Q_UNUSED(cb);
    Q_UNUSED(user_data);
    qWarning() << "OpenCL context notification: " << errinfo;
}

};

/*!
    Creates a new OpenCL context that matches platform() and
    \a type.  Does nothing if the context has already been created.

    Returns true if the context was created; false otherwise.
    On error, the status can be retrieved by calling createStatus().

    \sa isCreated(), setId(), release()
*/
bool QCLContext::create(QCLDevice::DeviceTypes type)
{
    Q_D(QCLContext);
    if (d->createStatus == CL_SUCCESS)
        return true;
    d->createStatus = CL_INVALID_CONTEXT;
    if (!d->platform.isNull()) {
        cl_context_properties props[] = {
            CL_CONTEXT_PLATFORM,
            intptr_t(d->platform.id()),
            0
        };
        d->id = clCreateContextFromType
            (props, cl_device_type(type),
             qt_cl_context_notify, 0, &(d->createStatus));
    } else {
        d->id = clCreateContextFromType
            (0, cl_device_type(type),
             qt_cl_context_notify, 0, &(d->createStatus));
    }
    bool isCreated = (d->createStatus == CL_SUCCESS);
    if (!isCreated) {
        qWarning() << "QCLContext::create(type:" << int(type) << "):"
                   << QCL::errorName(d->createStatus);
    }
    return isCreated;
}

/*!
    Creates a new OpenCL context that matches platform() and
    \a devices.  Does nothing if the context has already been created.

    Returns true if the context was created; false otherwise.
    On error, the status can be retrieved by calling createStatus().

    \sa isCreated(), setId(), release()
*/
bool QCLContext::create(const QList<QCLDevice>& devices)
{
    Q_D(QCLContext);
    if (d->createStatus == CL_SUCCESS)
        return true;
    d->createStatus = CL_INVALID_CONTEXT;
    QVector<cl_device_id> devs;
    foreach (QCLDevice dev, devices)
        devs.append(dev.id());
    if (!d->platform.isNull()) {
        cl_context_properties props[] = {
            CL_CONTEXT_PLATFORM,
            intptr_t(d->platform.id()),
            0
        };
        d->id = clCreateContext
            (props, devs.size(), devs.constData(),
             qt_cl_context_notify, 0, &(d->createStatus));
    } else {
        d->id = clCreateContext
            (0, devs.size(), devs.constData(),
             qt_cl_context_notify, 0, &(d->createStatus));
    }
    bool isCreated = (d->createStatus == CL_SUCCESS);
    if (!isCreated)
        qWarning() << "QCLContext::create:" << QCL::errorName(d->createStatus);
    return isCreated;
}

/*!
    Releases this context; destroying it if the reference count is zero.
    Does nothing if the context has not been created or is already released.

    \sa create()
*/
void QCLContext::release()
{
    Q_D(QCLContext);
    if (d->createStatus == CL_SUCCESS) {
        d->defaultCommandQueue = QCLCommandQueue();
        clReleaseContext(d->id);
        d->id = 0;
        d->createStatus = CL_INVALID_CONTEXT;
    }
}

/*!
    Returns the native OpenCL context identifier associated
    with this object.

    \sa setId()
*/
cl_context QCLContext::id() const
{
    Q_D(const QCLContext);
    return d->id;
}

/*!
    Sets the native OpenCL context identifier associated with this
    object to \a id.

    This function will call \c{clRetainContext()} to increase the
    reference count on \a id.  If the identifier was previously set
    to something else, then \c{clReleaseContext()} will be called
    on the previous value.

    \sa id(), create()
*/
void QCLContext::setId(cl_context id)
{
    Q_D(QCLContext);
    if (d->id == id)
        return;
    release();
    clRetainContext(id);
    d->id = id;
    d->createStatus = CL_SUCCESS;
}

/*!
    Returns the status of the last create() call, which will be
    \c{CL_SUCCESS} if the context was created, or an error code
    otherwise.

    If the context has not yet been created, or release() has been
    called, then createStatus() will return \c{CL_INVALID_CONTEXT}.

    \sa create(), release()
*/
int QCLContext::createStatus() const
{
    Q_D(const QCLContext);
    return d->createStatus;
}

/*!
    Returns the list of devices that are in use by this context.
    If the context has not been created, returns an empty list.

    \sa defaultDevice()
*/
QList<QCLDevice> QCLContext::devices() const
{
    Q_D(const QCLContext);
    QList<QCLDevice> devs;
    if (d->createStatus == CL_SUCCESS) {
        size_t size = 0;
        if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES, 0, 0, &size)
                == CL_SUCCESS && size > 0) {
            QVarLengthArray<cl_device_id> buf(size);
            if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES,
                                 size, buf.data(), 0) == CL_SUCCESS) {
                for (size_t index = 0; index < size; ++index)
                    devs.append(QCLDevice(buf[index]));
            }
        }
    }
    return devs;
}

/*!
    Returns the default device in use by this context, which is the
    first element of the devices() list; or a null QCLDevice if the
    context has not been created yet.

    \sa devices()
*/
QCLDevice QCLContext::defaultDevice() const
{
    Q_D(const QCLContext);
    if (d->createStatus == CL_SUCCESS) {
        size_t size = 0;
        if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES, 0, 0, &size)
                == CL_SUCCESS && size > 0) {
            QVarLengthArray<cl_device_id> buf(size);
            if (clGetContextInfo(d->id, CL_CONTEXT_DEVICES,
                                 size, buf.data(), 0) == CL_SUCCESS) {
                return QCLDevice(buf[0]);
            }
        }
    }
    return QCLDevice();
}

/*!
    Returns the last error that occurred while creating a command
    queue, buffer, image, or program.  Returns CL_SUCCESS if the
    last operation succeeded.
*/
int QCLContext::lastError() const
{
    Q_D(const QCLContext);
    return d->lastError;
}

/*!
    Returns the default command queue for defaultDevice().  If the queue
    has not been created, it will be created with the default properties
    of in-order execution of commands.

    Out of order execution can be set on the default command queue with
    QCLCommandQueue::setOutOfOrder().

    \sa createCommandQueue(), lastError()
*/
QCLCommandQueue QCLContext::defaultCommandQueue()
{
    Q_D(QCLContext);
    defaultQueue();
    return d->defaultCommandQueue;
}

// Returns the default queue handle without incurring retain/release overhead.
cl_command_queue QCLContext::defaultQueue()
{
    Q_D(QCLContext);
    if (d->defaultCommandQueue.isNull()) {
        if (d->createStatus != CL_SUCCESS)
            return 0;
        QCLDevice dev = defaultDevice();
        if (dev.isNull())
            return 0;
        cl_command_queue queue;
        cl_int error = CL_INVALID_VALUE;
        queue = clCreateCommandQueue(d->id, dev.id(), 0, &error);
        d->lastError = error;
        if (!queue) {
            qWarning() << "QCLContext::defaultCommandQueue:"
                       << QCL::errorName(error);
            return 0;
        }
        d->defaultCommandQueue.m_id = queue;
    }
    return d->defaultCommandQueue.id();
}

/*!
    Creates a new command queue on this context for \a device and
    \a properties.

    Unlike defaultCommandQueue(), this function will create a new queue
    every time it is called.  The queue will be deleted when the last
    reference to the returned object is removed.

    \sa defaultCommandQueue(), lastError()
*/
QCLCommandQueue QCLContext::createCommandQueue
    (const QCLDevice& device, cl_command_queue_properties properties)
{
    Q_D(QCLContext);
    QCLCommandQueue result;
    cl_command_queue queue;
    cl_int error = CL_INVALID_VALUE;
    queue = clCreateCommandQueue(d->id, device.id(), properties, &error);
    d->setLastError("QCLContext::createCommandQueue:", error);
    d->lastError = error;
    if (queue)
        result.m_id = queue;
    return result;
}

/*!
    Creates an OpenCL memory buffer of \a size bytes in length,
    with the specified \a flags.  The default \a flags creates a
    read-write memory object that allocates space from host memory.

    Returns the new OpenCL memory buffer object.

    \sa lastError()
*/
QCLBuffer QCLContext::createBuffer
    (size_t size, QCLMemoryObject::MemoryFlags flags)
{
    Q_D(QCLContext);
    QCLBuffer buffer;
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem mem = clCreateBuffer
        (d->id, cl_mem_flags(flags), size, 0, &error);
    d->setLastError("QCLContext::createBuffer(alloc):", error);
    if (mem) {
        buffer.m_context = this;
        buffer.m_id = mem;
    }
    return buffer;
}

/*!
    Creates an OpenCL memory buffer of \a size bytes in length,
    with the specified \a flags.  The default \a flags creates a
    read-write memory object that uses \a hostPointer as its storage.

    Returns the new OpenCL memory buffer object.

    \sa lastError()
*/
QCLBuffer QCLContext::createBuffer
    (void *hostPointer, size_t size, QCLMemoryObject::MemoryFlags flags)
{
    Q_D(QCLContext);
    QCLBuffer buffer;
    cl_int error = CL_INVALID_CONTEXT;
    cl_mem mem = clCreateBuffer
        (d->id, cl_mem_flags(flags), size, hostPointer, &error);
    d->setLastError("QCLContext::createBuffer(hostptr):", error);
    if (mem) {
        buffer.m_context = this;
        buffer.m_id = mem;
    }
    return buffer;
}

/*!
    Creates an OpenCL program object from the supplied \a sourceCode.

    \sa createProgramFromSourceFile(), buildProgramFromSourceCode()
*/
QCLProgram QCLContext::createProgramFromSourceCode(const char *sourceCode)
{
    Q_D(QCLContext);
    Q_ASSERT(sourceCode);
    QCLProgram program;
    cl_int error = CL_INVALID_CONTEXT;
    cl_program prog = clCreateProgramWithSource
        (d->id, 1, &sourceCode, 0, &error);
    d->setLastError("QCLContext::createProgramFromSourceCode:", error);
    if (prog) {
        program.m_context = this;
        program.m_id = prog;
    }
    return program;
}

/*!
    \overload

    Creates an OpenCL program object from the supplied \a sourceCode.

    \sa createProgramFromSourceFile(), buildProgramFromSourceCode()
*/
QCLProgram QCLContext::createProgramFromSourceCode(const QByteArray& sourceCode)
{
    Q_D(QCLContext);
    Q_ASSERT(!sourceCode.isEmpty());
    QCLProgram program;
    cl_int error = CL_INVALID_CONTEXT;
    const char *code = sourceCode.constData();
    size_t length = sourceCode.size();
    cl_program prog = clCreateProgramWithSource
        (d->id, 1, &code, &length, &error);
    d->setLastError("QCLContext::createProgramFromSourceCode(QByteArray):", error);
    if (prog) {
        program.m_context = this;
        program.m_id = prog;
    }
    return program;
}

/*!
    Creates an OpenCL program object from the contents of the specified
    \a fileName.

    \sa createProgramFromSourceCode(), buildProgramFromSourceFile()
*/
QCLProgram QCLContext::createProgramFromSourceFile(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "QCLContext::createProgramFromSourceFile: Unable to open file" << fileName;
        return QCLProgram();
    }
    QByteArray contents = file.readAll();
    return createProgramFromSourceCode(contents.constData());
}

/*!
    Creates an OpenCL program object from the supplied \a sourceCode
    and then builds it.  Returns a null QCLProgram if the program
    could not be built.

    \sa createProgramFromSourceCode(), buildProgramFromSourceFile()
*/
QCLProgram QCLContext::buildProgramFromSourceCode(const char *sourceCode)
{
    QCLProgram program = createProgramFromSourceCode(sourceCode);
    if (program.isNull() || program.build())
        return program;
    return QCLProgram();
}

/*!
    \overload

    Creates an OpenCL program object from the supplied \a sourceCode
    and then builds it.  Returns a null QCLProgram if the program
    could not be built.

    \sa createProgramFromSourceCode(), buildProgramFromSourceFile()
*/
QCLProgram QCLContext::buildProgramFromSourceCode(const QByteArray& sourceCode)
{
    QCLProgram program = createProgramFromSourceCode(sourceCode);
    if (program.isNull() || program.build())
        return program;
    return QCLProgram();
}

/*!
    Creates an OpenCL program object from the contents of the supplied
    \a fileName and then builds it.  Returns a null QCLProgram if the
    program could not be built.

    \sa createProgramFromSourceFile(), buildProgramFromSourceCode()
*/
QCLProgram QCLContext::buildProgramFromSourceFile(const QString& fileName)
{
    QCLProgram program = createProgramFromSourceFile(fileName);
    if (program.isNull() || program.build())
        return program;
    return QCLProgram();
}

QT_END_NAMESPACE
