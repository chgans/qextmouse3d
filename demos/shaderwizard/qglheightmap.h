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

#ifndef QGLHEIGHTMAP_H
#define QGLHEIGHTMAP_H

#include "qglbuilder.h"

class QGLHeightMap
{
public:
    explicit QGLHeightMap(int xPoints, int yPoints, float xSize = 1.0f, float ySize = 1.0f, QGL::Smoothing smoothing = QGL::Smooth);
    virtual ~QGLHeightMap();
    QGL::Smoothing smoothing() const;
    int xPoints() const;
    int yPoints() const;
    int xSize() const;
    int ySize() const;

    virtual float height(float s, float t) const;
    virtual QVector2D texCoords(float s, float t) const;

private:
    float m_xPoints;
    float m_yPoints;
    float m_xSize;
    float m_ySize;
    QGL::Smoothing m_Smoothing;
};

QGLBuilder& operator<<(QGLBuilder& list, const QGLHeightMap& cube);

#endif // QGLHEIGHTMAP_H
