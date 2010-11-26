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

#ifndef QGLHEMISPHERE_H
#define QGLHEMISPHERE_H

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLBuilder;

class Q_QT3D_EXPORT QGLHemiSphere
{
public:
    explicit QGLHemiSphere(qreal diameter = 1.0f, int depth = 3, bool baseEnabled = true)
        : m_diameter(diameter), m_subdivisionDepth(depth), m_baseEnabled(baseEnabled) {}
    virtual ~QGLHemiSphere();

    qreal diameter() const { return m_diameter; }
    void setDiameter(qreal diameter) { m_diameter = diameter; }

    int subdivisionDepth() const { return m_subdivisionDepth; }
    void setSubdivisionDepth(int depth) { m_subdivisionDepth = depth; }

    bool baseEnabled() const {return m_baseEnabled; }
    void setBaseEnabled(bool baseEnabled) {m_baseEnabled = baseEnabled;}

private:
    qreal m_diameter;
    int m_subdivisionDepth;
    bool m_baseEnabled;
};

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLHemiSphere& hemisphere);

QT_END_NAMESPACE

QT_END_HEADER

#endif