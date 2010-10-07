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


#include "cylinderview.h"
#include "qglbuilder.h"
#include "qglcylinder.h"
#include <QtCore/qurl.h>

CylinderView::CylinderView(QWidget *parent)
    : QGLView(parent)
{
    QGLBuilder builder;
    builder << QGL::Smooth << QGLCylinder(2.0, 1.5, 2.0, 36, 3);
    

    QGLMaterial *matLid = new QGLMaterial;
    matLid->setAmbientColor(Qt::gray);
    matLid->setDiffuseColor(Qt::gray);
    QUrl urlLid;
    urlLid.setPath(QLatin1String(":/latte.png"));
    urlLid.setScheme(QLatin1String("file"));
    matLid->setTextureUrl(urlLid);

    QGLMaterial *matSides = new QGLMaterial;
    matSides->setColor(QColor(170, 202, 0));
    QUrl urlSides;
    urlSides.setPath(QLatin1String(":/cupTexture.png"));
    urlSides.setScheme(QLatin1String("file"));
    matSides->setTextureUrl(urlSides);
        
    QGLSceneNode *root = builder.sceneNode();

    QGLSceneNode *lid = root->findChild<QGLSceneNode *>("Cylinder Top");
    int lidMat = root->palette()->addMaterial(matLid);
    lid->setMaterialIndex(lidMat);
    lid->setEffect(QGL::LitDecalTexture2D);

    QGLSceneNode *sides = root->findChild<QGLSceneNode *>("Cylinder Sides");
    int sideMat = root->palette()->addMaterial(matSides);
    sides->setMaterialIndex(sideMat);
    sides->setEffect(QGL::LitDecalTexture2D);


    cylinder = builder.finalizedSceneNode();
    
    QGLMaterial *mat = new QGLMaterial;
    mat->setAmbientColor(Qt::gray);
    mat->setDiffuseColor(Qt::gray);
    cylinder->setMaterial(mat);
    cylinder->setEffect(QGL::LitMaterial);
}

CylinderView::~CylinderView()
{
    delete cylinder;
}

void CylinderView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().translate(0.0f, -1.0f, 0.0f);
    painter->modelViewMatrix().rotate(-75.0f, 1.0f, 0.0f, 0.0f);    
    cylinder->draw(painter);
}

