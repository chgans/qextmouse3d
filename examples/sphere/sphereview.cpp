/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "sphereview.h"

#include <QImage>
#include <QPainter>
#include <QDir>
#include <QPropertyAnimation>
#include <QKeyEvent>

#include <math.h>

#include "qglsphere.h"
#include "qplane3d.h"

SphereView::SphereView(QWidget *parent)
    : QGLView(parent)
    , scene(0)
    , pointsImage(1, 1, QImage::Format_ARGB32)
    , textured(true)
    , angle(0)
{
    animation = new QPropertyAnimation(this, "rotateAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(4500);
    animation->setLoopCount(-1);

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
    if (!image.load(QLatin1String(":faceUV.jpg")))
        qDebug("Failed to load cylinder wrap grid texture");
    uvTexture.setImage(image);
    if (!image.load(QLatin1String(":faceIco.png")))
        qDebug("Failed to load icosphere grid texture");
    icoTexture.setImage(image);
    if (!image.load(QLatin1String(":faceCube.jpg")))
        qDebug("Failed to load cube texture");
    cubeTexture.setImage(image);

    QPainter painter(&pointsImage);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::green);
    painter.drawRect(0,0,1,1);
}

SphereView::~SphereView()
{
    delete scene;
}

void SphereView::initializeGL(QGLPainter *painter)
{
    painter->setFaceColor(QGL::AllFaces, QColor(196, 128, 128));

    painter->setStandardEffect(QGL::LitModulateTexture2D);

    painter->setLightModel(mdl);

    painter->setMainLight(lp);

    animation->start();

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
        if (animation->state() == QAbstractAnimation::Running)
            animation->stop();
        else
            animation->start();
    }
}
