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

#include "cubeview.h"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <stdio.h>

CubeView::CubeView(QWidget *parent)
    : QGLView(parent),
      sensitivity(0.333f)
{
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(accelerometerTimeout()));
    timer->start(100);
}

void CubeView::initializeGL(QGLPainter *painter)
{
    painter->setLightEnabled(0, true);
    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));

    QImage textureImage(":/qtlogo.png");
    texture.setImage(textureImage);

    painter->setStandardEffect(QGL::LitDecalTexture2D);
    painter->setTexture(&texture);
}

void CubeView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().scale(1.5f);
    cube.draw(painter);
}

void CubeView::accelerometerTimeout()
{
    QVector3D g = gravity();
    //qDebug() << g;
    camera()->setMotionAdjustment(g);
}

QVector3D CubeView::gravity() const
{
    // Access the raw accelerometer data on the N900.
    FILE *file = fopen("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "r");
    if (!file)
        return QVector3D(0, 0, -1);
    int x = 0;
    int y = 0;
    int z = 0;
    fscanf(file, "%d %d %d", &x, &y, &z);
    fclose(file);

    // Note: x seems to be wired in reverse on the N900.
    return QVector3D((-x / 1000.0f) * sensitivity,
                     (y / 1000.0f) * sensitivity, z / 1000.0f);
}
