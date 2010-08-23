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

#include "sphereview.h"

#include <QImage>
#include <QPainter>
#include <QDir>
#include <QTimer>
#include <QKeyEvent>

#include <math.h>

#include "qglsphere.h"
#include "qplane3d.h"

SphereView::SphereView(QWidget *parent)
    : QGLView(parent)
    , scene(0)
    , pointsImage(1, 1, QImage::Format_ARGB32)
    , textured(true)
    , timer(new QTimer(this))
{
    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    spheres.append(builder.newNode());
    builder << QGLCubeSphere(1, 1);
    spheres.append(builder.newNode());
    builder << QGLCubeSphere(1, 2);
    spheres.append(builder.newNode());
    builder << QGLCubeSphere(1, 3);
    spheres.append(builder.newNode());
    builder << QGLCubeSphere(1, 5);

    spheres.append(builder.newNode());
    builder << QGLSphere(1, 1);
    spheres.append(builder.newNode());
    builder << QGLSphere(1, 2);
    spheres.append(builder.newNode());
    builder << QGLSphere(1, 3);
    spheres.append(builder.newNode());
    builder << QGLSphere(1, 5);

    spheres.append(builder.newNode());
    builder << QGLIcoSphere(1, 1);
    spheres.append(builder.newNode());
    builder << QGLIcoSphere(1, 2);
    spheres.append(builder.newNode());
    builder << QGLIcoSphere(1, 3);
    spheres.append(builder.newNode());
    builder << QGLIcoSphere(1, 5);

    scene = builder.finalizedSceneNode();

    lp = new QGLLightParameters(this);
    lp->setDirection(QVector3D(1.0, 0.0, 0.0));

    mat = new QGLMaterial(this);
    mat->setDiffuseColor(QColor(255, 255, 255));

    mdl = new QGLLightModel(this);
    mdl->setAmbientSceneColor(QColor(196,196,196));

    QImage image;
    if (!image.load(":faceUV.jpg"))
        qDebug("Failed to load cylinder wrap grid texture");
    uvTexture.setImage(image);
    if (!image.load(":faceIco.png"))
        qDebug("Failed to load icosphere grid texture");
    icoTexture.setImage(image);
    if (!image.load(":faceCube.jpg"))
        qDebug("Failed to load cube texture");
    cubeTexture.setImage(image);

    QPainter painter(&pointsImage);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::green);
    painter.drawRect(0,0,1,1);
}

SphereView::~SphereView()
{
    delete timer;
    delete scene;
}

void SphereView::initializeGL(QGLPainter *painter)
{
    painter->setFaceColor(QGL::AllFaces, QColor(196, 128, 128));

    painter->setStandardEffect(QGL::LitModulateTexture2D);

    painter->setLightModel(mdl);

    painter->setMainLight(lp);

    connect(timer, SIGNAL(timeout()), this, SLOT(rotate()));
    timer->start(25);

    glEnable(GL_BLEND);
}

void SphereView::paintGL(QGLPainter *painter)
{
    if (textured)
        painter->setStandardEffect(QGL::LitModulateTexture2D);
    else
        painter->setStandardEffect(QGL::FlatColor);

    painter->setFaceMaterial(QGL::FrontFaces, mat);

    for (int i = 0; i < spheres.size(); ++i)  {
        if (i == 0)
            cubeTexture.bind();
        else if (i == 4)
            uvTexture.bind();
        else if (i == 8)
            icoTexture.bind();

        qreal x = (i % 4) * 1.5f - 2.25f;
        qreal y = (i / 4) * 1.5f - 1.5f;
        painter->modelViewMatrix().push();
        painter->modelViewMatrix().translate(x, y, 0.0f);
        painter->modelViewMatrix().rotate(angle, 0.10f, 1.0f, 0.0f);
        spheres[i]->draw(painter);
        painter->modelViewMatrix().pop();
    }
}

void SphereView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        if (timer->isActive())
            timer->stop();
        else
            timer->start();
    }
}

void SphereView::rotate()
{
    angle = (angle + 2) % 360;
    updateGL();
}
