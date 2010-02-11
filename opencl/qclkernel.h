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

#ifndef QCLKERNEL_H
#define QCLKERNEL_H

#include "qclglobal.h"
#include "qclevent.h"
#include "qclworksize.h"
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class QCLContext;
class QCLProgram;
class QCLBuffer;
class QCLDevice;

class Q_CL_EXPORT QCLKernel
{
public:
    QCLKernel() : m_context(0), m_id(0) {}
    QCLKernel(QCLContext *context, cl_kernel id);
    QCLKernel(const QCLKernel& other);
    ~QCLKernel();

    QCLKernel& operator=(const QCLKernel& other);

    bool isNull() const { return m_id == 0; }

    cl_kernel id() const { return m_id; }
    QCLContext *context() const { return m_context; }

    QCLProgram program() const;
    QString name() const;
    int argCount() const;

    QCLWorkSize declaredWorkGroupSize() const;
    QCLWorkSize declaredWorkGroupSize(const QCLDevice& device) const;

    void setArg(int index, cl_int value);
    void setArg(int index, cl_uint value);
    void setArg(int index, cl_long value);
    void setArg(int index, cl_ulong value);
    void setArg(int index, float value);
    void setArg(int index, const QCLBuffer& value);
    void setArg(int index, const void *data, size_t size);

    QCLEvent execute
        (const QCLWorkSize& globalWorkSize);
    QCLEvent execute
        (const QCLWorkSize& globalWorkSize, const QCLWorkSize& localWorkSize);
    QCLEvent execute
        (const QCLWorkSize& globalWorkSize, const QVector<QCLEvent>& after);
    QCLEvent execute
        (const QCLWorkSize& globalWorkSize, const QCLWorkSize& localWorkSize,
         const QVector<QCLEvent>& after);

private:
    QCLContext *m_context;
    cl_kernel m_id;

    friend class QCLProgram;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
