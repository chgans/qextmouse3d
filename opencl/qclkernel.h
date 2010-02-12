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
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class QCLContext;
class QCLProgram;
class QCLMemoryObject;
class QCLSampler;
class QCLDevice;

class QCLKernelPrivate;

class Q_CL_EXPORT QCLKernel
{
public:
    QCLKernel();
    QCLKernel(QCLContext *context, cl_kernel id);
    QCLKernel(const QCLKernel& other);
    ~QCLKernel();

    QCLKernel& operator=(const QCLKernel& other);

    bool isNull() const;

    cl_kernel id() const;
    QCLContext *context() const;

    QCLProgram program() const;
    QString name() const;
    int argCount() const;

    QCLWorkSize declaredWorkGroupSize() const;
    QCLWorkSize declaredWorkGroupSize(const QCLDevice& device) const;

    QCLWorkSize globalWorkSize() const;
    void setGlobalWorkSize(const QCLWorkSize& size);

    QCLWorkSize localWorkSize() const;
    void setLocalWorkSize(const QCLWorkSize& size);

    QVector<QCLEvent> dependentEvents() const;
    void setDependentEvents(const QVector<QCLEvent>& events);

    void setArg(int index, cl_int value);
    void setArg(int index, cl_uint value);
    void setArg(int index, cl_long value);
    void setArg(int index, cl_ulong value);
    void setArg(int index, float value);
    void setArg(int index, const QCLMemoryObject& value);
    void setArg(int index, const QCLSampler& value);
    void setArg(int index, const void *data, size_t size);

    QCLEvent execute();

    inline QCLEvent operator()() { return execute(); }

    template <typename T1>
    inline QCLEvent operator()(const T1& arg1)
    {
        setArg(0, arg1);
        return execute();
    }

    template <typename T1, typename T2>
    inline QCLEvent operator()(const T1& arg1, const T2& arg2)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        return execute();
    }

    template <typename T1, typename T2, typename T3>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        return execute();
    }

    template <typename T1, typename T2, typename T3, typename T4>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        setArg(3, arg4);
        return execute();
    }

    template <typename T1, typename T2, typename T3, typename T4,
              typename T5>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
         const T5& arg5)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        setArg(3, arg4);
        setArg(4, arg5);
        return execute();
    }

    template <typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
         const T5& arg5, const T6& arg6)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        setArg(3, arg4);
        setArg(4, arg5);
        setArg(5, arg6);
        return execute();
    }

    template <typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
         const T5& arg5, const T6& arg6, const T7& arg7)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        setArg(3, arg4);
        setArg(4, arg5);
        setArg(5, arg6);
        setArg(6, arg7);
        return execute();
    }

    template <typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename T8>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
         const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        setArg(3, arg4);
        setArg(4, arg5);
        setArg(5, arg6);
        setArg(6, arg7);
        setArg(7, arg8);
        return execute();
    }

    template <typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename T8,
              typename T9>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
         const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8,
         const T9& arg9)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        setArg(3, arg4);
        setArg(4, arg5);
        setArg(5, arg6);
        setArg(6, arg7);
        setArg(7, arg8);
        setArg(8, arg9);
        return execute();
    }

    template <typename T1, typename T2, typename T3, typename T4,
              typename T5, typename T6, typename T7, typename T8,
              typename T9, typename T10>
    inline QCLEvent operator()
        (const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4,
         const T5& arg5, const T6& arg6, const T7& arg7, const T8& arg8,
         const T9& arg9, const T10& arg10)
    {
        setArg(0, arg1);
        setArg(1, arg2);
        setArg(2, arg3);
        setArg(3, arg4);
        setArg(4, arg5);
        setArg(5, arg6);
        setArg(6, arg7);
        setArg(7, arg8);
        setArg(8, arg9);
        setArg(9, arg10);
        return execute();
    }

private:
    QScopedPointer<QCLKernelPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QCLKernel)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
