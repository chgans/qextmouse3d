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

#ifndef QCLWORKSIZE_H
#define QCLWORKSIZE_H

#include "qclglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(CL)

class Q_CL_EXPORT QCLWorkSize
{
public:
    QCLWorkSize()
        : m_dim(1) { m_sizes[0] = 1; m_sizes[1] = 0; m_sizes[2] = 0; }
    QCLWorkSize(size_t size)
        : m_dim(1) { m_sizes[0] = size; m_sizes[1] = 0; m_sizes[2] = 0; }
    QCLWorkSize(size_t width, size_t height)
        : m_dim(2) { m_sizes[0] = width; m_sizes[1] = height; m_sizes[2] = 0; }
    QCLWorkSize(size_t width, size_t height, size_t depth)
        : m_dim(3)
        { m_sizes[0] = width; m_sizes[1] = height; m_sizes[2] = depth; }

    size_t dimensions() const { return m_dim; }
    size_t width() const { return m_sizes[0]; }
    size_t height() const { return m_sizes[1]; }
    size_t depth() const { return m_sizes[2]; }

    const size_t *sizes() const { return m_sizes; }

private:
    size_t m_dim;
    size_t m_sizes[3];
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
