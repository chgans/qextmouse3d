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

#include "spotview.h"
#include "qplane3d.h"
#include "triplane.h"
#include "spotlight.h"
#include "qglabstracteffect.h"
#include "qglcamera.h"

#include <QtCore/qtimer.h>
#include <QtGui/qcolor.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef TWO_PI
#define TWO_PI 6.283185307179586477
#endif

SpotView::SpotView(QWidget *parent)
    : QGLWidget(parent)
    , floor(new TriPlane(QPlane3D(QVector3D(0.0f, -1.0f, 0.0f),
                                  QVector3D(0.0f, 0.8f, 0.1f)),
                         QVector3D(0.0f, 0.0f, 1.0f),
                         QSizeF(10.0f, 10.0f), 1))
    , timer(new QTimer(this))
    , spin(0)
{
    lp = new QGLLightParameters(this);
    lp->setAmbientColor(QColor(32, 64, 32));
    lp->setDiffuseColor(QColor(196, 255, 196));
    lp->setDirection(QVector3D(0.0f, -1.0f, -1.0f));

    lights[0] = new Spotlight(QColor(255, 0, 0));
    lights[0]->setArcIncr(QVector2D(TWO_PI/ 70.0f, TWO_PI / 140.f));
    lights[1] = new Spotlight(QColor(0, 255, 0));
    lights[1]->setArcIncr(QVector2D(TWO_PI / 120.0f, TWO_PI / 60.0f));
    lights[2] = new Spotlight(QColor(0, 0, 255));
    lights[2]->setArcIncr(QVector2D(TWO_PI / 50.0f, TWO_PI / 100.0f));

    model = new QGLLightModel(this);
    model->setAmbientSceneColor(QColor(24, 24, 24));
    //model->setViewerPosition(QGLLightModel::LocalViewer);
}

SpotView::~SpotView()
{
    delete floor;
    delete timer;
    for (int i = 0; i < 3; ++i)
        delete lights[i];
}

void SpotView::initializeGL()
{
    QGLPainter painter(this);
    painter.clear();
    initializeGL(&painter);
}

void SpotView::initializeGL(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::LitMaterial);
    //painter->setStandardEffect(QGL::FlatColor);
    painter->setClearColor(QColor(32, 32, 128));
    painter->setDepthTestingEnabled(true);

    painter->setLightModel(model);

    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(25);
}

void SpotView::paintGL()
{
    QGLPainter painter(this);
    //painter.begin();
    painter.clear();

    // Set perspective transformation and position model-view matrix
    QGLCamera camera;
    QVector3D sceneOrigin = floor->boundingBox().center();
    camera.setCenter(sceneOrigin);
    camera.setEye(sceneOrigin + QVector3D(0.0f, 5.0f, 20.0f));
    camera.apply(&painter);

    paintGL(&painter);
}

void SpotView::paintGL(QGLPainter *painter)
{
    painter->clear(QGL::ClearColorBuffer);

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().rotate(spin, 0.0f, 1.0f, 0.0f);

    painter->setLightParameters(0, lp);
    painter->setLightEnabled(0, true);

    for (int i = 0; i < NumLights; ++i)
        lights[i]->position(painter);

    floor->draw(painter);
    painter->modelViewMatrix().pop();

    painter->setStandardEffect(QGL::FlatColor);
    for (int i = 0; i < NumLights; ++i)
        lights[i]->draw(painter);
    painter->setStandardEffect(QGL::LitMaterial);
    //painter->effect()->setActive(true);
}

void SpotView::animate()
{
    static bool once = false;
    spin = (spin + 1) % 360;
    for (int i = 0; i < NumLights; ++i)
        lights[i]->increment();
    if (once)
        this->close();
    //once = true;
    updateGL();
}
