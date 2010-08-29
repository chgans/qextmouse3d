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
#include <QtOpenGL/qglframebufferobject.h>
#include <QtCore/qpropertyanimation.h>

//! [1]
CubeView::CubeView(QWidget *parent)
    : QGLView(parent)
    , fbo(0)
    , tangle(0.0f)
    , cangle(0.0f)
    , oangle(0.0f)
{
    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    builder << QGLCube(1.5f);
    cube = builder.currentNode();
    cube->setObjectName("Cube");

    builder.newSection();
    builder << QGLTeapot();
    teapot = builder.currentNode();
    teapot->setObjectName("Teapot");

    scene = builder.finalizedSceneNode();
    scene->setParent(this);
//! [1]

//! [2]
    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(this, "teapotAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(1000);
    animation->setLoopCount(-1);
    animation->start();

    animation = new QPropertyAnimation(this, "cubeAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(5000);
    animation->setLoopCount(-1);
    animation->start();

    animation = new QPropertyAnimation(this, "orbitAngle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(5000);
    animation->setLoopCount(-1);
    animation->start();
//! [2]

//! [3]
    innerCamera = new QGLCamera(this);
}
//! [3]

CubeView::~CubeView()
{
    delete fbo;
}

//! [4]
void CubeView::initializeGL(QGLPainter *)
{
    fbo = new QGLFramebufferObject(512, 512, QGLFramebufferObject::Depth);
    fboSurface.setFramebufferObject(fbo);
//! [4]

//! [5]
    QImage textureImage(":/qtlogo.png");
    qtlogo.setImage(textureImage);

    glEnable(GL_BLEND);
}
//! [5]

//! [6]
void CubeView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().push();
    painter->projectionMatrix().push();
    painter->pushSurface(&fboSurface);
//! [6]

//! [7]
    painter->setCamera(innerCamera);
    painter->modelViewMatrix().rotate(tangle, 0.0f, 1.0f, 0.0f);

    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));
    painter->setStandardEffect(QGL::LitMaterial);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    teapot->draw(painter);
//! [7]

//! [8]
    painter->popSurface();
    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();
//! [8]

//! [9]
    painter->modelViewMatrix().rotate(oangle, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 m = painter->modelViewMatrix();
    QVector3D cube1pos(-1.5f, 0.0f, 0.0f);
    QVector3D cube2pos(1.5f, 0.0f, 0.0f);

    if (m.map(cube1pos).z() < m.map(cube2pos).z()) {
        drawCube1(painter, cube1pos);
        drawCube2(painter, cube2pos);
    } else {
        drawCube2(painter, cube2pos);
        drawCube1(painter, cube1pos);
    }
}
//! [9]

//! [10]
void CubeView::drawCube1(QGLPainter *painter, const QVector3D &posn)
{
    painter->modelViewMatrix().push();

    qtlogo.bind();
    painter->setFaceColor(QGL::AllFaces, QColor(202, 100, 0, 150));
    painter->setStandardEffect(QGL::LitDecalTexture2D);

    painter->modelViewMatrix().translate(posn);
    painter->modelViewMatrix().rotate(cangle, 1.0f, -1.0f, 1.0f);

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    cube->draw(painter);
    glCullFace(GL_BACK);
    cube->draw(painter);
    glDisable(GL_CULL_FACE);

    painter->modelViewMatrix().pop();
}
//! [10]

//! [11]
void CubeView::drawCube2(QGLPainter *painter, const QVector3D &posn)
{
    painter->modelViewMatrix().push();

    painter->setFaceColor(QGL::AllFaces, QColor(0, 160, 202, 125));
    painter->setStandardEffect(QGL::LitDecalTexture2D);
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glEnable(GL_TEXTURE_2D);

    painter->modelViewMatrix().translate(posn);
    painter->modelViewMatrix().rotate(cangle, 1.0f, 1.0f, 1.0f);

    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
    cube->draw(painter);
    glCullFace(GL_BACK);
    cube->draw(painter);
    glDisable(GL_CULL_FACE);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    painter->modelViewMatrix().pop();
}
//! [11]
