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

#include "qclmemoryobject.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCLMemoryObject
    \brief The QCLMemoryObject class represents an OpenCL memory object.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::opencl
*/

/*!
    \enum QCLMemoryObject::MemoryFlag
    This enum defines the flags to use when creating OpenCL memory objects.

    \value ReadWrite The memory object will be read and written by a kernel.
    \value WriteOnly The memory object will be written, but not read,
           by a kernel.
    \value ReadOnly The memory object will be read, but not written,
           by a kernel.
    \value UseHostPointer Use the host's memory pointer as the storage
           for the memory object.
    \value AllocateHostPointer Allocate the memory object from host
           accessible memory.  UseHostPointer and AllocHostPointer
           are mutually exclusive.
    \value CopyHostPointer Copy the data at the host pointer to another
           location to initialize the memory object.
*/

/*!
    \fn QCLMemoryObject::QCLMemoryObject(QCLContext *context)

    Constructs a null OpenCL memory object and associates it with \a context.
*/

/*!
    Constructs an OpenCL memory object from the native identifier \a id,
    and associates it with \a context.
*/
QCLMemoryObject::QCLMemoryObject(QCLContext *context, cl_mem id)
    : m_context(context), m_id(id)
{
    if (m_id)
        clRetainMemObject(m_id);
}

/*!
    Releases this OpenCL memory object.  If this is the last reference
    to the memory object, it will be destroyed.
*/
QCLMemoryObject::~QCLMemoryObject()
{
    if (m_id)
        clReleaseMemObject(m_id);
}

/*!
    \fn bool QCLMemoryObject::isNull() const

    Returns true if this OpenCL memory object is null; false otherwise.
*/

/*!
    \fn cl_mem QCLMemoryObject::id() const

    Returns the native OpenCL identifier for this memory object.
*/

/*!
    \fn QCLContext *QCLMemoryObject::context() const

    Returns the OpenCL context that created this memory object.
*/

/*!
    \internal
*/
void QCLMemoryObject::setId(QCLContext *context, cl_mem id)
{
    m_context = context;
    if (m_id == id)
        return;
    if (m_id)
        clReleaseMemObject(m_id);
    m_id = id;
    if (m_id)
        clRetainMemObject(m_id);
}

QT_END_NAMESPACE
