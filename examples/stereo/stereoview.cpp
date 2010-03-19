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

#include "stereoview.h"
#include "qglteapot.h"
#include "qglcube.h"
#include "qglmaterial.h"

void StereoView::initializeGL(QGLPainter *painter)
{
    teapot << QGLTeapot();
    teapot.finalize();

    cube.newSection(QGL::Faceted);
    cube << QGLCube(1.0f);
    cube.finalize();

    QGLMaterial *china = new QGLMaterial(this);
    china->setAmbientColor(QColor(192, 150, 128));
    china->setSpecularColor(QColor(60, 60, 60));
    china->setShininess(128);

    painter->setLightEnabled(0, true);
    painter->setFaceMaterial(QGL::AllFaces, china);
    painter->setStandardEffect(QGL::LitMaterial);

    camera()->setEye(QVector3D(0.0f, 0.0f, 15.0f));
    camera()->setEyeSeparation(0.2f);
}

void StereoView::paintGL(QGLPainter *painter)
{
    teapot.draw(painter);

    painter->modelViewMatrix().translate(-1.0f, 0.0f, 2.0f);
    cube.draw(painter);
}
