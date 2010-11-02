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

#ifndef QGLSPHERE_H
#define QGLSPHERE_H

#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLBuilder;
class QVector2D;

class Q_QT3D_EXPORT QGLSphere
{
public:
    explicit QGLSphere(qreal diameter = 1.0f, int depth = 3)
        : m_diameter(diameter), m_subdivisionDepth(depth) {}
    virtual ~QGLSphere();

    qreal diameter() const { return m_diameter; }
    void setDiameter(qreal diameter) { m_diameter = diameter; }

    int subdivisionDepth() const { return m_subdivisionDepth; }
    void setSubdivisionDepth(int depth) { m_subdivisionDepth = depth; }

private:
    qreal m_diameter;
    int m_subdivisionDepth;
};

class Q_QT3D_EXPORT QGLIcoSphere : public QGLSphere
{
public:
    explicit QGLIcoSphere(qreal diameter = 1.0f, int depth = 3)
        : QGLSphere(diameter, depth) {}

    virtual void faceTextureCoords
        (int face, QVector2D *t0, QVector2D *t1, QVector2D *t2) const;
};

class Q_QT3D_EXPORT QGLCubeSphere : public QGLSphere
{
public:
    explicit QGLCubeSphere(qreal diameter = 1.0f, int depth = 3)
        : QGLSphere(diameter, depth) {}

    virtual void faceTextureCoords
        (int face, QVector2D *t0, QVector2D *t1,
         QVector2D *t2, QVector2D *t3) const;
};

Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLSphere& sphere);
Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLIcoSphere& sphere);
Q_QT3D_EXPORT QGLBuilder& operator<<(QGLBuilder& builder, const QGLCubeSphere& sphere);

QT_END_NAMESPACE

QT_END_HEADER

#endif
