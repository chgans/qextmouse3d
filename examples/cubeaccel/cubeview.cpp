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
#include "qglcube.h"
#include "qglteapot.h"
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>
#include <stdio.h>

CubeView::CubeView(QWidget *parent)
    : QGLView(parent),
      sensitivity(0.1f)
{
    setOption(CameraNavigation, false);

    roomCamera = new QGLCamera(this);
    roomCamera->setAdjustForAspectRatio(false);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(accelerometerTimeout()));
    timer->start(100);
}

void CubeView::initializeGL(QGLPainter *painter)
{
    cube.newSection(QGL::Faceted);
    cube << QGLCube(1.0f);

    room.newSection(QGL::Faceted);

    QGLSceneNode *back = room.newNode();
    room.begin(QGL::QUAD);
    room.addVertex(-3.0f, -3.0f, -15.0f);
    room.addVertex( 3.0f, -3.0f, -15.0f);
    room.addVertex( 3.0f,  3.0f, -15.0f);
    room.addVertex(-3.0f,  3.0f, -15.0f);
    room.end();

    QGLSceneNode *left = room.newNode();
    room.begin(QGL::QUAD);
    room.addVertex(-3.0f, -3.0f, -15.0f);
    room.addVertex(-3.0f,  3.0f, -15.0f);
    room.addVertex(-3.0f,  3.0f, 0.0f);
    room.addVertex(-3.0f, -3.0f, 0.0f);
    room.end();

    QGLSceneNode *right = room.newNode();
    room.begin(QGL::QUAD);
    room.addVertex(3.0f,  3.0f, -15.0f);
    room.addVertex(3.0f, -3.0f, -15.0f);
    room.addVertex(3.0f, -3.0f, 0.0f);
    room.addVertex(3.0f,  3.0f, 0.0f);
    room.end();

    QGLSceneNode *top = room.newNode();
    room.begin(QGL::QUAD);
    room.addVertex(-3.0f,  3.0f, -15.0f);
    room.addVertex( 3.0f,  3.0f, -15.0f);
    room.addVertex( 3.0f,  3.0f, 0.0f);
    room.addVertex(-3.0f,  3.0f, 0.0f);
    room.end();

    QGLSceneNode *bottom = room.newNode();
    room.begin(QGL::QUAD);
    room.addVertex(-3.0f, -3.0f, -15.0f);
    room.addVertex(-3.0f, -3.0f, 0.0f);
    room.addVertex( 3.0f, -3.0f, 0.0f);
    room.addVertex( 3.0f, -3.0f, -15.0f);
    room.end();

    int index;

    QGLMaterial *mat1 = new QGLMaterial();
    mat1->setDiffuseColor(QColor(128, 100, 0));
    index = room.palette()->addMaterial(mat1);
    back->setMaterial(index);

    QGLMaterial *mat2 = new QGLMaterial();
    mat2->setDiffuseColor(Qt::cyan);
    index = room.palette()->addMaterial(mat2);
    left->setMaterial(index);
    right->setMaterial(index);

    QGLMaterial *mat3 = new QGLMaterial();
    mat3->setDiffuseColor(Qt::yellow);
    index = room.palette()->addMaterial(mat3);
    top->setMaterial(index);
    bottom->setMaterial(index);

    room.finalize();
    //qDumpScene(&room);

    teapot << QGLTeapot();
    QGLMaterial *china = new QGLMaterial();
    china->setAmbientColor(QColor(192, 150, 128));
    china->setSpecularColor(QColor(60, 60, 60));
    china->setShininess(128);
    teapot.currentNode()->setMaterial(teapot.palette()->addMaterial(china));
    teapot.finalize();

    roomModel = new QGLLightModel(this);
    roomModel->setAmbientSceneColor(Qt::white);
    roomModel->setViewerPosition(QGLLightModel::LocalViewer);

    normalModel = new QGLLightModel(this);

    painter->setLightEnabled(0, true);
    QGLLightParameters *light = new QGLLightParameters(this);
    light->setPosition(QVector3D(0.0f, 0.0f, 3.0f));
    painter->setLightParameters(0, light);

    QImage textureImage(":/qtlogo.png");
    texture.setImage(textureImage);
}

void CubeView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().push();
    painter->projectionMatrix().push();

    painter->setFaceColor(QGL::AllFaces, QColor(255, 0, 0));
    painter->setStandardEffect(QGL::LitMaterial);
    roomCamera->apply(painter);
    painter->setLightModel(roomModel);
    room.draw(painter);

    painter->modelViewMatrix().pop();
    painter->projectionMatrix().pop();

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(1.0f, -0.5f, 0.0f);
    painter->modelViewMatrix().rotate(45.0f, 1.0f, 1.0f, 1.0f);
    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));
    painter->setStandardEffect(QGL::LitDecalTexture2D);
    painter->setTexture(&texture);
    painter->setLightModel(normalModel);
    cube.draw(painter);
    painter->setTexture((QGLTexture2D *)0);

    painter->modelViewMatrix().pop();
    painter->modelViewMatrix().translate(-0.8f, -1.5f, -3.0f);
    painter->setStandardEffect(QGL::LitMaterial);
    teapot.draw(painter);
}

void CubeView::accelerometerTimeout()
{
    QVector3D g = gravity();
    //qDebug() << g;
    camera()->setMotionAdjustment(g);
    roomCamera->setMotionAdjustment(g);
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
