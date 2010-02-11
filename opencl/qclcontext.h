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

#ifndef QCLCONTEXT_H
#define QCLCONTEXT_H

#include "qcldevice.h"
#include "qclcommandqueue.h"
#include "qclbuffer.h"
#include "qclimage.h"
#include "qclprogram.h"
#include <QtCore/qscopedpointer.h>
#include <QtCore/qsize.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class QCLContextPrivate;
class QCLKernel;

class Q_CL_EXPORT QCLContext
{
public:
    QCLContext();
    ~QCLContext();

    bool isCreated() const;

    QCLPlatform platform() const;
    void setPlatform(const QCLPlatform& platform);

    bool create(QCLDevice::DeviceTypes type = QCLDevice::GPU);
    bool create(const QList<QCLDevice>& devices);
    void release();

    cl_context id() const;
    void setId(cl_context id);

    int createStatus() const;

    QList<QCLDevice> devices() const;
    QCLDevice defaultDevice() const;

    int lastError() const;

    QCLCommandQueue defaultCommandQueue();
    QCLCommandQueue createCommandQueue
        (const QCLDevice& device, cl_command_queue_properties properties);

    QCLBuffer createBuffer(size_t size, QCLMemoryObject::MemoryFlags flags = QCLMemoryObject::ReadWrite | QCLMemoryObject::AllocateHostPointer);
    QCLBuffer createBuffer(void *hostPointer, size_t size, QCLMemoryObject::MemoryFlags flags = QCLMemoryObject::ReadWrite | QCLMemoryObject::UseHostPointer);

    QCLProgram createProgramFromSourceCode(const char *sourceCode);
    QCLProgram createProgramFromSourceCode(const QByteArray& sourceCode);
    QCLProgram createProgramFromSourceFile(const QString& fileName);

    QCLProgram buildProgramFromSourceCode(const char *sourceCode);
    QCLProgram buildProgramFromSourceCode(const QByteArray& sourceCode);
    QCLProgram buildProgramFromSourceFile(const QString& fileName);

    QList<QCLImageFormat> supportedImage2DFormats(QCLMemoryObject::MemoryFlags flags) const;
    QList<QCLImageFormat> supportedImage3DFormats(QCLMemoryObject::MemoryFlags flags) const;

private:
    QScopedPointer<QCLContextPrivate> d_ptr;

    Q_DISABLE_COPY(QCLContext)
    Q_DECLARE_PRIVATE(QCLContext)

    cl_command_queue defaultQueue(); // For quicker access from friends.

    friend class QCLBuffer;
    friend class QCLKernel;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif