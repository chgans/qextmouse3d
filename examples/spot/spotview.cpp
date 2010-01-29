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
