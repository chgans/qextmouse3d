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

#include "pancontroller.h"
#include "qglview.h"
#include "qglcamera.h"
#include <qmath.h>

#include <QtGlobal>
#include <QTime>

class PanControllerPrivate
{
public:
    PanControllerPrivate()
        : speed(0.0f)
        , prevSpeed(0.0f)
        , arrowDirection(Qt::NoArrow)
        , view(0)
        , defaultZ(-1.0f)
        , reset(true)
        , maxSpeed(4.0f)
        , animating(false)
    {}

    void calculateValues();

    qreal speed;
    qreal prevSpeed;
    Qt::ArrowType arrowDirection;
    QGLView *view;
    QVector3D targetEye;
    QVector3D targetCenter;
    QVector3D sourceEye;
    QVector3D sourceCenter;
    qreal defaultZ;
    bool reset;
    QTime panTime;
    qreal maxSpeed;
    bool animating;
};

PanController::PanController(QObject *parent)
    : QObject(parent)
    , d(new PanControllerPrivate)
{
    QGLView *v = qobject_cast<QGLView*>(parent);
    if (v)
        d->view = v;
}

PanController::~PanController()
{
    delete d;
}

void PanController::setMaxSpeed(qreal maxSpeed)
{
    d->maxSpeed = maxSpeed;
}

qreal PanController::maxSpeed() const
{
    return d->maxSpeed;
}

void PanController::setSpeed(qreal speed)
{
    d->animating = true;
    qDebug() << "PanController::setSpeed" << speed << "prevSpeed:" << d->prevSpeed;
    if (d->speed != speed)
    {
        d->prevSpeed = d->speed;
        d->speed = speed;
        d->calculateValues();
        if (qIsNull(d->speed) || d->speed == 1.0)
        {
            d->animating = false;
            d->reset = true;
            qDebug() << "reset by speed == null";
        }
        emit speedChanged();
    }
    qDebug() << "animating:" << d->animating;
}

void PanController::pan()
{
    QGLCamera *cam = d->view->camera();
    Q_ASSERT(cam);
    qreal t = d->panTime.restart();
    if (d->animating)
    {
        cam->setCenter(d->sourceCenter + ((d->targetCenter - d->sourceCenter) * d->speed));
        cam->setEye(d->sourceEye + ((d->targetEye - d->sourceEye) * d->speed));
        qDebug() << "PanController::pan() - animating - center:" << cam->center()
                    << "eye:" << cam->eye();
    }
    else
    {
        if (d->speed > 0.99)
        {
            qreal distance = t * (d->maxSpeed * d->speed) / 1000.0f;
            if (d->arrowDirection == Qt::LeftArrow)
                distance = -distance;
            QVector3D e = cam->eye();
            QVector3D c = cam->center();
            e.setX(e.x() + distance);
            c.setX(c.x() + distance);
            cam->setEye(e);
            cam->setCenter(c);
            qDebug() << "PanController::pan() - panning - center:" << cam->center()
                        << "eye:" << cam->eye();
        }
    }
}

void PanControllerPrivate::calculateValues()
{
    if (view)
    {
        QGLCamera *cam = view->camera();
        Q_ASSERT(cam);
        if (reset)
        {
            if (defaultZ < 0.0)
                defaultZ = sourceEye.z();

            sourceCenter = cam->center();
            sourceEye = cam->eye();

            qreal desiredPanViewAngle = 70.0f;
            targetEye = sourceEye;
            qreal q = cam->nearPlane() * 2.0f;
            targetEye.setZ(q);
            targetEye.setY(0.0);

            targetCenter = targetEye;
            qreal xoff = q * qTan(desiredPanViewAngle);
            if (arrowDirection == Qt::LeftArrow)
            {
                xoff = -xoff;
                qDebug() << "left arrow - offset:" << xoff;
            }
            else if (arrowDirection == Qt::NoArrow)
            {
                xoff = 0.0;
                targetEye.setZ(defaultZ);
                qDebug() << "no arrow - offset:" << xoff;
            }
            targetCenter.setX(targetCenter.x() + xoff);
            targetCenter.setZ(sourceCenter.z());

            qDebug() << "resetting --- pan from:" << sourceCenter << "( eye:" << sourceEye << ")"
                        << "to:" << targetCenter << "( eye:" << targetEye << ")";
            reset = false;
        }
    }
}

qreal PanController::speed() const
{
    return d->speed;
}

Qt::ArrowType PanController::direction() const
{
    return d->arrowDirection;
}

void PanController::setDirection(Qt::ArrowType arrow)
{
    d->arrowDirection = arrow;
    d->reset = true;
}

QGLView *PanController::view() const
{
    return d->view;
}

void PanController::setView(QGLView *view)
{
    d->view = view;
    d->reset = true;
}

