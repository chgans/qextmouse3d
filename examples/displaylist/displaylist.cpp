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

#include "displaylist.h"
#include "qgldisplaylist.h"
#include "qglmaterialcollection.h"
#include "qgltexture2d.h"
#include "qglmaterialparameters.h"
#include "qglsection.h"

#include <QtCore/qmath.h>

DisplayListView::DisplayListView(QWidget *parent)
    : QGLView(parent)
    , soupCan(new QGLDisplayList(this))
{
    buildGeometry();
}

void DisplayListView::initializeGL(QGLPainter *painter)
{
    //! [0]
    painter->setLightEnabled(0, true);

    QGLMaterialCollection *mats = soupCan->materials();

    QGLMaterialParameters *parms = new QGLMaterialParameters(mats);
    parms->setAmbientColor(QColor(170, 202, 0));
    parms->setDiffuseColor(QColor(170, 202, 0));
    int canMat = mats->addMaterial(parms);

    QImage soupLabel(":/qt-soup.png");
    QGLTexture2D *tex = new QGLTexture2D();
    tex->setImage(soupLabel);
    mats->setTexture(canMat, tex);

    painter->setStandardEffect(QGL::LitDecalTexture2D);
    //! [0]
}

void DisplayListView::paintGL(QGLPainter *painter)
{
    //! [1]
    soupCan->draw(painter);
    //! [1]
}

void DisplayListView::buildGeometry()
{
    //! [2]
    const qreal canRadius = 2.0f;
    const qreal canHeight = 6.0f;
    const int numSlices = 16;

    // definitional data for our can
    QGLDisplayList::VectorArray topCanRim;
    QGLDisplayList::VectorArray bottomCanRim;
    QVector3D canLidCenter;  // center is origin: 0, 0, 0
    QVector3D canExtrudeVec(0.0f, 0.0f, -canHeight);
    QVector3D canBottomCenter = canLidCenter + canExtrudeVec;
    QGLTextureSpecifier texMap(0, 0, 1, 1);

    // do the math for the defining points
    for (int i = 0; i < numSlices; ++i)
    {
        qreal angle = (i * 2.0 * M_PI) / numSlices;
        topCanRim << QVector3D(canRadius * qCos(angle), canRadius * qSin(angle), 0.0f);
    }

    // create the flat top lid of the can
    soupCan->newSection();
    soupCan->addTriangulatedFace(canLidCenter, topCanRim);

    // create the sides of the can, and save the extruded bottom rim
    soupCan->newSection();
    bottomCanRim = soupCan->extrude(topCanRim, canExtrudeVec, texMap);

    // create the flat bottom lid of the can
    soupCan->newSection();
    soupCan->addTriangulatedFace(canBottomCenter, bottomCanRim);
    //! [2]
}
