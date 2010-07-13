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
      planet1Posn(2.5f, 0.0f, 0.0f),
      planet2Posn(4.0f, 0.0f, 0.0f),
      planet3Posn(6.0f, 0.0f, 0.0f)
{
    qreal radius = 2.5f;

    animPlanet1 = new QSphericalAnimation(this, "planet1Pos", this);
    animPlanet1->setStartValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet1->setKeyValueAt(0.25f, qVariantFromValue(QVector3D(0.0f, 0.0f, radius)));
    animPlanet1->setKeyValueAt(0.5f, qVariantFromValue(QVector3D(-radius, 0.0f, 0.0f)));
    animPlanet1->setKeyValueAt(0.75f, qVariantFromValue(QVector3D(0.0f, 0.0f, -radius)));
    animPlanet1->setEndValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet1->setDuration(3000);
    animPlanet1->setLoopCount(-1);
    animPlanet1->start();

    radius = 4.0f;

    animPlanet2 = new QSphericalAnimation(this, "planet2Pos", this);
    animPlanet2->setStartValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet2->setKeyValueAt(0.25f, qVariantFromValue(QVector3D(0.0f, 0.0f, radius)));
    animPlanet2->setKeyValueAt(0.5f, qVariantFromValue(QVector3D(-radius, 0.0f, 0.0f)));
    animPlanet2->setKeyValueAt(0.75f, qVariantFromValue(QVector3D(0.0f, 0.0f, -radius)));
    animPlanet2->setEndValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet2->setDuration(7000);
    animPlanet2->setLoopCount(-1);
    animPlanet2->start();

    radius = 6.0f;

    animPlanet3 = new QSphericalAnimation(this, "planet3Pos", this);
    animPlanet3->setStartValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet3->setKeyValueAt(0.25f, qVariantFromValue(QVector3D(0.0f, 0.0f, radius)));
    animPlanet3->setKeyValueAt(0.5f, qVariantFromValue(QVector3D(-radius, 0.0f, 0.0f)));
    animPlanet3->setKeyValueAt(0.75f, qVariantFromValue(QVector3D(0.0f, 0.0f, -radius)));
    animPlanet3->setEndValue(qVariantFromValue(QVector3D(radius, 0.0f, 0.0f)));
    animPlanet3->setDuration(5000);
    animPlanet3->setLoopCount(-1);
    animPlanet3->start();

    camera()->setEye(QVector3D(0.0f, 0.0f, 25.0f));
}

void PlanetView::initializeGL(QGLPainter *painter)
{
    painter->setStandardEffect(QGL::LitMaterial);

    list.newSection(QGL::Faceted);
    sun = list.newNode();
    list << QGLSphere(2.0f, 4);
    planet1 = list.newNode();
    list << QGLSphere(0.5f, 3);
    planet2 = list.newNode();
    list << QGLSphere(0.4f, 3);
    planet3 = list.newNode();
    list << QGLSphere(0.2f, 3);

    list.finalize();
}

void PlanetView::paintGL(QGLPainter *painter)
{
    painter->setFaceColor(QGL::AllFaces, Qt::yellow);
    sun->draw(painter);

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(planet1Posn);
    painter->setFaceColor(QGL::AllFaces, QColor(0, 192, 192));
    planet1->draw(painter);
    painter->modelViewMatrix().pop();

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(planet2Posn);
    painter->setFaceColor(QGL::AllFaces, QColor(192, 0, 0));
    planet2->draw(painter);
    painter->modelViewMatrix().pop();

    painter->modelViewMatrix().push();
    painter->modelViewMatrix().rotate(-10.f, 1.0f, 0.0f, 0.0f);
    painter->modelViewMatrix().translate(planet3Posn);
    painter->setFaceColor(QGL::AllFaces, QColor(255, 255, 255));
    planet3->draw(painter);
    painter->modelViewMatrix().pop();
}
