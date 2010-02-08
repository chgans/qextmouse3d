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

#include "qclerrors.h"

QT_BEGIN_NAMESPACE

namespace QCL
{

QString errorName(int code)
{
#define CLERR(name) case name: return QLatin1String(#name);
    switch (code) {
    CLERR(CL_SUCCESS)
    CLERR(CL_DEVICE_NOT_FOUND)
    CLERR(CL_DEVICE_NOT_AVAILABLE)
    CLERR(CL_COMPILER_NOT_AVAILABLE)
    CLERR(CL_MEM_OBJECT_ALLOCATION_FAILURE)
    CLERR(CL_OUT_OF_RESOURCES)
    CLERR(CL_OUT_OF_HOST_MEMORY)
    CLERR(CL_PROFILING_INFO_NOT_AVAILABLE)
    CLERR(CL_MEM_COPY_OVERLAP)
    CLERR(CL_IMAGE_FORMAT_MISMATCH)
    CLERR(CL_IMAGE_FORMAT_NOT_SUPPORTED)
    CLERR(CL_BUILD_PROGRAM_FAILURE)
    CLERR(CL_MAP_FAILURE)

    CLERR(CL_INVALID_VALUE)
    CLERR(CL_INVALID_DEVICE_TYPE)
    CLERR(CL_INVALID_PLATFORM)
    CLERR(CL_INVALID_DEVICE)
    CLERR(CL_INVALID_CONTEXT)
    CLERR(CL_INVALID_QUEUE_PROPERTIES)
    CLERR(CL_INVALID_COMMAND_QUEUE)
    CLERR(CL_INVALID_HOST_PTR)
    CLERR(CL_INVALID_MEM_OBJECT)
    CLERR(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR)
    CLERR(CL_INVALID_IMAGE_SIZE)
    CLERR(CL_INVALID_SAMPLER)
    CLERR(CL_INVALID_BINARY)
    CLERR(CL_INVALID_BUILD_OPTIONS)
    CLERR(CL_INVALID_PROGRAM)
    CLERR(CL_INVALID_PROGRAM_EXECUTABLE)
    CLERR(CL_INVALID_KERNEL_NAME)
    CLERR(CL_INVALID_KERNEL_DEFINITION)
    CLERR(CL_INVALID_KERNEL)
    CLERR(CL_INVALID_ARG_INDEX)
    CLERR(CL_INVALID_ARG_VALUE)
    CLERR(CL_INVALID_ARG_SIZE)
    CLERR(CL_INVALID_KERNEL_ARGS)
    CLERR(CL_INVALID_WORK_DIMENSION)
    CLERR(CL_INVALID_WORK_GROUP_SIZE)
    CLERR(CL_INVALID_WORK_ITEM_SIZE)
    CLERR(CL_INVALID_GLOBAL_OFFSET)
    CLERR(CL_INVALID_EVENT_WAIT_LIST)
    CLERR(CL_INVALID_EVENT)
    CLERR(CL_INVALID_OPERATION)
    CLERR(CL_INVALID_GL_OBJECT)
    CLERR(CL_INVALID_BUFFER_SIZE)
    CLERR(CL_INVALID_MIP_LEVEL)
#ifdef CL_INVALID_GLOBAL_WORK_SIZE
    CLERR(CL_INVALID_GLOBAL_WORK_SIZE)
#endif

    default: break;
    }
    return QLatin1String("Error ") + QString::number(code);
#undef CLERR
}

}; // namespace QCL

QT_END_NAMESPACE
