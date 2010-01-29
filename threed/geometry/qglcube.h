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

#ifndef QGLCUBE_H
#define QGLCUBE_H

#include "qglgeometry.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QGLCube : public QGLGeometry
{
    Q_OBJECT
public:
    explicit QGLCube(QObject *parent = 0);
    explicit QGLCube(qreal size, QObject *parent = 0);
    QGLCube(qreal width, qreal height, qreal depth, QObject *parent = 0);
    virtual ~QGLCube();

    void setTextureCoords(const QVector2D& c1,
                          const QVector2D& c2,
                          const QVector2D& c3,
                          const QVector2D& c4,
                          int face = -1);

    void drawFace(QGLPainter *painter, int face);

private:
    void init(qreal width, qreal height, qreal depth);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
