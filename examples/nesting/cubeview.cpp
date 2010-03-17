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

CubeView::CubeView(QWidget *parent)
    : QGLView(parent)
    , fbo(0)
    , tangle(0.0f)
    , cangle(0.0f)
    , needsUpdate(true)
{
    innerCamera = new QGLCamera(this);

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
}

CubeView::~CubeView()
{
    delete fbo;
}

void CubeView::setTeapotAngle(qreal angle)
{
    tangle = angle;
    if (!needsUpdate) {
        needsUpdate = true;
        update();
    }
}

void CubeView::setCubeAngle(qreal angle)
{
    cangle = angle;
    if (!needsUpdate) {
        needsUpdate = true;
        update();
    }
}

void CubeView::initializeGL(QGLPainter *)
{
    cube.newSection(QGL::Faceted);
    cube << QGLCube(2.0f);
    cube.finalize();

    teapot << QGLTeapot();
    teapot.finalize();

    fbo = new QGLFramebufferObject(512, 512, QGLFramebufferObject::Depth);
}

void CubeView::paintGL(QGLPainter *painter)
{
    needsUpdate = false;

    painter->modelViewMatrix().push();
    painter->projectionMatrix().push();
    painter->pushSurface(fbo);
    painter->setViewport(painter->surfaceSize());

    innerCamera->apply(painter, painter->surfaceSize());
    painter->modelViewMatrix().rotate(tangle, 0.0f, 1.0f, 0.0f);

    painter->setLightEnabled(0, true);
    painter->setFaceColor(QGL::AllFaces, QColor(170, 202, 0));
    painter->setStandardEffect(QGL::LitMaterial);

    painter->clear();
    teapot.draw(painter);

    painter->popSurface();
    painter->projectionMatrix().pop();
    painter->modelViewMatrix().pop();
    painter->setViewport(painter->surfaceSize());

    painter->setFaceColor(QGL::AllFaces, QColor(0, 160, 202));
    painter->setStandardEffect(QGL::LitDecalTexture2D);
    glBindTexture(GL_TEXTURE_2D, fbo->texture());
    glEnable(GL_TEXTURE_2D);

    painter->modelViewMatrix().rotate(cangle, 1.0f, 1.0f, 1.0f);
    cube.draw(painter);

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}
