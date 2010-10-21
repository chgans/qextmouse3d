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

#include "tankview.h"
#include "tank.h"
#include "quadplane.h"

#include "qglscenenode.h"
#include "qray3d.h"
#include "qplane3d.h"

#include <QMouseEvent>

TankView::TankView(QWidget *parent)
    : QGLView(parent)
    , m_tankScene(new QGLSceneNode)
    , m_count(0)
{
    m_tankScene->setParent(this);
    m_tankScene->setPalette(new QGLMaterialCollection(this));
    m_tankScene->setEffect(QGL::LitDecalTexture2D);
    m_tankScene->addNode(new QuadPlane(0, QSizeF(25, 25)));
    Tank *tank = addTank();
    connect(tank, SIGNAL(scaleChanged()), this, SLOT(queueUpdate()));

    qDumpScene(m_tankScene);

    setToolTip(tr("Double-click to add more tanks"));
    setWindowTitle(tr("Double-click Me!"));
}

TankView::~TankView()
{
}

Tank *TankView::addTank()
{
    Tank *tank = new Tank(m_tankScene);
    tank->setObjectName(QString("Tank %1").arg(m_count));
    ++m_count;
    connect(tank, SIGNAL(scaleChanged()), this, SLOT(queueUpdate()));
    return tank;
}

void TankView::paintGL(QGLPainter *painter)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_tankScene->draw(painter);
}

void TankView::initializeGL(QGLPainter *painter)
{
    painter->setClearColor(Qt::gray);
    camera()->setEye(camera()->eye() + QVector3D(0.0f, 4.0f, 0.0f));
    glEnable(GL_BLEND);
}

void TankView::wheelEvent(QWheelEvent *e)
{
    e->accept();
    QVector3D viewVec = camera()->eye() - camera()->center();
    qreal zoomMag = viewVec.length();
    qreal inc = float(e->delta()) / 50.0f;
    if (!qFuzzyIsNull(inc))
    {
        zoomMag += inc;
        if (zoomMag < 2.0f)
            zoomMag = 2.0f;
        QRay3D viewLine(camera()->center(), viewVec.normalized());
        camera()->setEye(viewLine.point(zoomMag));
        update();
    }
}

void TankView::mouseDoubleClickEvent(QMouseEvent *e)
{
    Tank *t = addTank();
    QVector3D pick = mapPoint(e->pos());

    // find the origin of the near plane
    QRay3D eyeline(camera()->eye(), (camera()->center() - camera()->eye()).normalized());
    QVector3D nearPlaneOrigin = eyeline.point(camera()->nearPlane());

    // from the near plane origin move up and across by the pick's XY to find the point
    // on the near plane
    QRay3D up(nearPlaneOrigin, camera()->upVector());
    QVector3D sideDir = QVector3D::crossProduct(camera()->upVector(), -eyeline.direction());
    QRay3D side(up.point(pick.y()), sideDir.normalized());
    QVector3D v = side.point(pick.x());

    // intersect the ray thru the picked point on the near plane with the floor
    QRay3D ray(camera()->eye(), v - camera()->eye());
    QPlane3D floorPlane(QVector3D(), QVector3D(0, 1, 0));
    QVector3D pointOnFloor = ray.point(floorPlane.intersection(ray));
    t->setPosition(pointOnFloor);
}
