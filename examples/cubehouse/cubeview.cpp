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
#include <QtCore/qpropertyanimation.h>
#include <stdio.h>

CubeView::CubeView(QWidget *parent)
    : QGLView(parent), scene(0), cube(0), teapot(0), room(0),
      sensitivity(0.1f),
      showFrameRate(false),
      stereo(false),
      cangle(0.0f),
      prevX(0),
      prevY(0),
      prevZ(0),
      havePrev(false)
{
    setOption(CameraNavigation, false);

    roomCamera = new QGLCamera(this);
    roomCamera->setAdjustForAspectRatio(false);

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(this, "cubeAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(5000);
    animation->setLoopCount(-1);
    animation->start();

    time.start();
}

void CubeView::initializeGL(QGLPainter *painter)
{
    QGLBuilder builder;
    builder << QGL::Faceted << QGLCube(1.0f);
    cube = builder.currentNode();
    builder << QGL::Faceted;
    room = builder.currentNode();
    builder.pushNode();
    QGLSceneNode *back = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, -15.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *left = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, 0.0f));
        quad.appendVertex(QVector3D(-3.0f, -3.0f, 0.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *right = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D(3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D(3.0f, -3.0f, 0.0f));
        quad.appendVertex(QVector3D(3.0f,  3.0f, 0.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *top = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f,  3.0f, -15.0f));
        quad.appendVertex(QVector3D( 3.0f,  3.0f, 0.0f));
        quad.appendVertex(QVector3D(-3.0f,  3.0f, 0.0f));
        builder.addQuads(quad);
    }
    QGLSceneNode *bottom = builder.newNode();
    {
        QGeometryData quad;
        quad.appendVertex(QVector3D(-3.0f, -3.0f, -15.0f));
        quad.appendVertex(QVector3D(-3.0f, -3.0f, 0.0f));
        quad.appendVertex(QVector3D( 3.0f, -3.0f, 0.0f));
        quad.appendVertex(QVector3D( 3.0f, -3.0f, -15.0f));
        builder.addQuads(quad);
    }
    builder.popNode();

    int index;
    QGLMaterialCollection *palette = builder.sceneNode()->palette();

    QGLMaterial *mat1 = new QGLMaterial();
    mat1->setDiffuseColor(QColor(128, 100, 0));
    index = palette->addMaterial(mat1);
    back->setMaterialIndex(index);

    QGLMaterial *mat2 = new QGLMaterial();
    mat2->setDiffuseColor(Qt::cyan);
    index = palette->addMaterial(mat2);
    left->setMaterialIndex(index);
    right->setMaterialIndex(index);

    QGLMaterial *mat3 = new QGLMaterial();
    mat3->setDiffuseColor(Qt::yellow);
    index = palette->addMaterial(mat3);
    top->setMaterialIndex(index);
    bottom->setMaterialIndex(index);

    //qDumpScene(room);

    builder.newSection();
    builder << QGLTeapot();
    teapot = builder.currentNode();
    QGLMaterial *china = new QGLMaterial();
    china->setAmbientColor(QColor(192, 150, 128));
    china->setSpecularColor(QColor(60, 60, 60));
    china->setShininess(128);
    teapot->setMaterial(china);

    scene = builder.finalizedSceneNode();
    scene->setParent(this);

    roomModel = new QGLLightModel(this);
    roomModel->setAmbientSceneColor(Qt::white);
    roomModel->setViewerPosition(QGLLightModel::LocalViewer);

    normalModel = new QGLLightModel(this);

    QGLLightParameters *light = new QGLLightParameters(this);
    light->setPosition(QVector3D(0.0f, 0.0f, 3.0f));
    painter->setMainLight(light);

    QImage textureImage(":/qtlogo.png");
    texture.setImage(textureImage);

    if (stereo) {
        camera()->setEyeSeparation(0.4f);
        roomCamera->setEyeSeparation(0.1f);
    }
}

void CubeView::paintGL(QGLPainter *painter)
{
    if (showFrameRate)
        qWarning("time since last frame: %d ms", time.restart());

    glDisable(GL_BLEND);

    painter->modelViewMatrix().push();
    painter->projectionMatrix().push();

    painter->setStandardEffect(QGL::LitMaterial);
    painter->setCamera(roomCamera);
    painter->setLightModel(roomModel);
    room->draw(painter);

    painter->modelViewMatrix().pop();
    painter->projectionMatrix().pop();

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(-0.8f, -1.5f, -3.0f);
    painter->setLightModel(normalModel);
    painter->setStandardEffect(QGL::LitMaterial);
    teapot->draw(painter);
    painter->modelViewMatrix().pop();

    glEnable(GL_BLEND);

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(1.0f, -0.5f, 0.0f);
    painter->modelViewMatrix().rotate(cangle, 1.0f, 1.0f, 1.0f);
    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0, 120));
    painter->setStandardEffect(QGL::LitDecalTexture2D);
    painter->setTexture(&texture);
    glDisable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    cube->draw(painter);
    glCullFace(GL_BACK);
    cube->draw(painter);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    painter->setTexture((QGLTexture2D *)0);
    painter->modelViewMatrix().pop();
}

void CubeView::setCubeAngle(qreal angle)
{
    cangle = angle;
    accelerometerTimeout();
    queueUpdate();
}

void CubeView::accelerometerTimeout()
{
    QVector3D g = gravity();
    camera()->setMotionAdjustment(g);
    roomCamera->setMotionAdjustment(g);
}

QVector3D CubeView::gravity() const
{
    // Access the raw accelerometer data on the N900.
    FILE *file = fopen("/sys/class/i2c-adapter/i2c-3/3-001d/coord", "r");
    if (!file)
        return QVector3D(0, 0, -1);
    float x = 0;
    float y = 0;
    float z = 0;
    fscanf(file, "%f %f %f", &x, &y, &z);
    fclose(file);

    // Smooth out the reported values.  Large changes are applied as-is,
    // and small jitters smooth to the rest position.
    if (havePrev) {
        qreal xdiff = x - prevX;
        qreal ydiff = y - prevY;
        qreal zdiff = z - prevZ;
        if (qAbs(xdiff) < 20.0f && qAbs(ydiff) < 20.0f && qAbs(zdiff) < 20.0f) {
            x = prevX + xdiff * 0.1f;
            y = prevY + ydiff * 0.1f;
            z = prevZ + zdiff * 0.1f;
        }
    }
    prevX = x;
    prevY = y;
    prevZ = z;
    havePrev = true;

    return QVector3D((x / 1000.0f) * sensitivity,
                     (-y / 1000.0f) * sensitivity, -z / 1000.0f);
}
