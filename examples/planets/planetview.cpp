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

#include "planetview.h"
#include "qglsphere.h"

PlanetView::PlanetView(QWidget *parent)
    : QGLView(parent),
      planet1Posn(3.0f, 0.0f, 0.0f)
{
    qreal radius = 3.0f;

    animPlanet1 = new QSphericalAnimation(this, "planet1Pos", this);
    animPlanet1->setStartValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet1->setKeyValueAt(0.25f, qVariantFromValue(QVector3D(0.0f, radius, 0.0f)));
    animPlanet1->setKeyValueAt(0.5f, qVariantFromValue(QVector3D(-radius, 0.0f, 0.0f)));
    animPlanet1->setKeyValueAt(0.75f, qVariantFromValue(QVector3D(0.0f, -radius, 0.0f)));
    animPlanet1->setEndValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet1->setDuration(3000);
    animPlanet1->setLoopCount(-1);
    animPlanet1->start();

    camera()->setEye(QVector3D(0.0f, 0.0f, 20.0f));
}

void PlanetView::initializeGL(QGLPainter *painter)
{
    painter->setLightEnabled(0, true);
    painter->setStandardEffect(QGL::LitMaterial);

    sun = list.newNode();
    list << QGLSphere(2.0f, 4);
    planet1 = list.newNode();
    list << QGLSphere(0.5f, 3);

    list.finalize();
}

void PlanetView::paintGL(QGLPainter *painter)
{
    painter->setFaceColor(QGL::AllFaces, Qt::yellow);
    sun->draw(painter);
    painter->modelViewMatrix().translate(planet1Posn);
    painter->setFaceColor(QGL::AllFaces, QColor(0, 192, 192));
    planet1->draw(painter);
}
