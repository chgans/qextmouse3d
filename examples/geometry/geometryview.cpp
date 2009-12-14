/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "geometryview.h"

#include <QImage>
#include <QPainter>
#include <QDir>
#include <QTimer>

#include <math.h>

#include "geometry.h"
#include "quadplane.h"
#include "qplane3d.h"
#include "qglmaterialcollection.h"

GeometryView::GeometryView(QWidget *parent)
    : QGLView(parent)
    , timer(new QTimer(this))
{
    lp = new QGLLightParameters(this);
    lp->setPosition(QVector3D(-3.0, 1.0, 15.0));
    lp->setSpotAngle(15.0);
    lp->setSpotExponent(24);
    lp->setSpotDirection(QVector3D(0.0, -1.0, -2.0));

    palette = new QGLMaterialCollection(this);

    // first set up the geometry - an icosahedron
    // this one handles all its own colors and textures
    icosahedron = new Geometry(this, palette);

    // now a generic flat floor plane which will be painted grey
    // and textured the same as the icosahedron
    floor = new QuadPlane(this, palette);
    QGLMaterialParameters *parms = new QGLMaterialParameters;
    parms->setAmbientColor(Qt::darkGray);
    parms->setDiffuseColor(Qt::gray);
    int m = palette->addMaterial(parms);
    palette->setTexture(m, palette->texture(icosahedron->material()));
    floor->setMaterial(m);
    QMatrix4x4 mat;
    mat.translate(0.0f, 0.0f, -5.0f);
    floor->setLocalTransform(mat);

    mdl = new QGLLightModel(this);
    mdl->setAmbientSceneColor(QColor(196,196,196));
}

GeometryView::~GeometryView()
{
    delete icosahedron;
    delete floor;
    delete timer;
}

void GeometryView::initializeGL(QGLPainter *painter)
{
    painter->setLightModel(mdl);

    painter->setLightParameters(0, lp);
    painter->setLightEnabled(0, true);

    connect(timer, SIGNAL(timeout()), this, SLOT(rotate()));
    timer->start(25);
}

void GeometryView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().scale(0.7f);
    floor->draw(painter);
    painter->modelViewMatrix().rotate(angle, 0.10f, 1.0f, 0.0f);
    icosahedron->draw(painter);
}

void GeometryView::rotate()
{
    angle = (angle + 2) % 360;
    updateGL();
}
