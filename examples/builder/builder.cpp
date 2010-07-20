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

#include "builder.h"
#include "qglbuilder.h"
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
#include "qglmaterial.h"
#include "qglscenenode.h"
#include "qgllightmodel.h"

#include <QtGui/qmatrix4x4.h>

#include <QtCore/qmath.h>

BuilderView::BuilderView(QWidget *parent)
    : QGLView(parent)
    , canScene(new QGLSceneNode(this))
{
    //! [0]
    QGLSceneNode *can = buildGeometry();
    qDumpScene(can);
    can->setParent(canScene);
    {
        // rotate the can around so its label shows; and down
        // so the base is facing down
        QMatrix4x4 mat;
        QQuaternion q1 = QQuaternion::fromAxisAndAngle(1.0f, 0.0f, 0.0f, 270.0f);
        QQuaternion q2 = QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, 100.0f);
        mat.rotate(q2 * q1);
        can->setLocalTransform(mat);
    }

    // display a copy of the can to the left
    QGLSceneNode *node = new QGLSceneNode(canScene);
    node->addNode(can);
    {
        QMatrix4x4 mat;
        mat.translate(-2.0f, 0.0f, -2.0f);
        node->setLocalTransform(mat);
    }

    // display a copy of the can to the right
    node = new QGLSceneNode(canScene);
    node->addNode(can);
    {
        QMatrix4x4 mat;
        mat.translate(2.0f, 0.0f, -2.0f);
        node->setLocalTransform(mat);
    }
    //! [0]

    // rotate the whole scene about x-axis so that
    // can tops are visible when scene is first displayed
    {
        QMatrix4x4 mat;
        mat.rotate(1.0f, 0.0f, 0.0f, -30.0f);
        canScene->setLocalTransform(mat);
    }
}

void BuilderView::initializeGL(QGLPainter *painter)
{
    QGLLightParameters *light0 = new QGLLightParameters(this);
    light0->setAmbientColor(Qt::white);
    light0->setDiffuseColor(Qt::white);
    light0->setDirection(QVector3D(0.0f, 0.2f, 2.0f));
    painter->setMainLight(light0);
    QGLLightModel *model = new QGLLightModel(this);
    model->setAmbientSceneColor(Qt::white);
    painter->setLightModel(model);
}

//! [1]
void BuilderView::paintGL(QGLPainter *painter)
{
    canScene->draw(painter);
}
//! [1]

QGLSceneNode *BuilderView::buildGeometry()
{
    //! [2]
    QGLBuilder builder;
    QGLSceneNode *root = builder.sceneNode();

    QGLMaterial *mat = new QGLMaterial;
    mat->setAmbientColor(Qt::lightGray);
    mat->setDiffuseColor(Qt::lightGray);
    QUrl url;
    url.setPath(QLatin1String(":/images/qt-soup.png"));
    url.setScheme(QLatin1String("file"));
    mat->setTextureUrl(url);
    int canMat = root->palette()->addMaterial(mat);
    root->setMaterialIndex(canMat);
    root->setEffect(QGL::LitMaterial);
    //! [2]

    // size data for can
    const qreal canRadius = 1.0f;
    const qreal canHeight = 2.5f;
    const int numSlices = 32;

    QGeometryData canRim;
    QVector3D canExtrudeVec(0.0f, 0.0f, -canHeight);

    // do the math for the defining points
    for (int i = 0; i < numSlices; ++i)
    {
        qreal angle = (qreal(i) * 2.0 * M_PI) / numSlices;
        canRim.appendVertex(QVector3D(canRadius * qCos(angle),
                                      canRadius * qSin(angle),
                                      canHeight / 2.0f));
    }

    //! [3]
    // create the flat top lid of the can
    builder.newSection();
    builder.currentNode()->setObjectName("CanTop");
    QGeometryData top;
    top.appendVertex(canRim.center());
    top.appendVertexArray(canRim.vertices());
    builder.addTriangulatedFace(top);

    // create the sides of the can
    builder.newSection();
    builder.currentNode()->setObjectName("CanSides");
    builder.currentNode()->setMaterialIndex(canMat);
    builder.currentNode()->setEffect(QGL::LitModulateTexture2D);
    QGeometryData canTop = canRim;
    canTop.appendVertex(canTop.vertex(0));       // doubled vert for texture seam
    canTop.generateTextureCoordinates();            // generate x texture coords
    QGeometryData canBase = canTop.translated(canExtrudeVec);  // base has tex.y == 0
    for (int i = 0; i < canTop.count(); ++i)
        canTop.texCoord(i).setY(1.0);                      // top has tex.y == 1
    builder.addQuadsInterleaved(canTop, canBase);

    // create the flat bottom lid of the can
    builder.newSection();
    builder.currentNode()->setObjectName("CanBottom");
    builder.currentNode()->setEffect(QGL::LitMaterial);
    QGeometryData rimReversed = canRim.translated(canExtrudeVec).reversed();
    QGeometryData canBottom;
    canBottom.appendVertex(rimReversed.center());
    canBottom.appendVertexArray(rimReversed.vertices());
    builder.addTriangulatedFace(canBottom);

    return builder.finalizedSceneNode();
    //! [3]
}
