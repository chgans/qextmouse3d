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

#ifndef QCLMEMORYOBJECT_H
#define QCLMEMORYOBJECT_H

#include "qclglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class QCLContext;

class Q_CL_EXPORT QCLMemoryObject
{
protected:
    QCLMemoryObject(QCLContext *context = 0) : m_context(context), m_id(0) {}
    QCLMemoryObject(QCLContext *context, cl_mem id)
        : m_context(context), m_id(id) {}

public:
    virtual ~QCLMemoryObject();

    enum MemoryFlag
    {
        ReadWrite           = 0x0001,
        WriteOnly           = 0x0002,
        ReadOnly            = 0x0004,
        UseHostPointer      = 0x0008,
        AllocateHostPointer = 0x0010,
        CopyHostPointer     = 0x0020
    };
    Q_DECLARE_FLAGS(MemoryFlags, MemoryFlag)

    bool isNull() const { return m_id == 0; }

    cl_mem id() const { return m_id; }
    QCLContext *context() const { return m_context; }

    QCLMemoryObject::MemoryFlags flags() const;
    void *hostPointer() const;
    size_t size() const;

protected:
    void setId(QCLContext *context, cl_mem id);

private:
    QCLContext *m_context;
    cl_mem m_id;

    Q_DISABLE_COPY(QCLMemoryObject)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QCLMemoryObject::MemoryFlags)

QT_END_NAMESPACE

QT_END_HEADER

#endif
