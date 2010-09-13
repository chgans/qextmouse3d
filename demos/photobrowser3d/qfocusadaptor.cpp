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

#include "qfocusadaptor.h"
#include "qglscenenode.h"
#include "qglview.h"
#include "qglcamera.h"

class QFocusAdaptorPrivate
{
public:
    QFocusAdaptorPrivate()
        : progress(0.0)
        , reset(true)
        , target(0)
        , view(0)
    {}
    qreal progress;
    QVector3D targetEye;
    QVector3D targetCenter;
    QVector3D sourceEye;
    QVector3D sourceCenter;
    bool reset;
    QGLSceneNode *target;
    QGLView *view;
};

QFocusAdaptor::QFocusAdaptor(QObject *parent)
    : QObject(parent)
    , d(new QFocusAdaptorPrivate)
{
    QGLView *v = qobject_cast<QGLView*>(parent);
    if (v)
        d->view = v;
}

QFocusAdaptor::~QFocusAdaptor()
{
    delete d;
}

qreal QFocusAdaptor::progress() const
{
    qDebug() << "requesting progress:" << d->progress;
    return d->progress;
}

void QFocusAdaptor::setProgress(qreal progress)
{
    qDebug() << "setting progress:" << progress;
    if (d->progress != progress)
    {
        qDebug() << "     from:" << d->progress;
        d->progress = progress;
        if (d->reset)
            calculateValues();
        emit progressChanged();
    }
}

QGLView *QFocusAdaptor::view() const
{
    return d->view;
}

void QFocusAdaptor::setView(QGLView *view)
{
    d->view = view;
    d->reset = true;
}

QGLSceneNode *QFocusAdaptor::target() const
{
    return d->target;
}

void QFocusAdaptor::setTarget(QGLSceneNode *target)
{
    d->target = target;
    d->reset = true;
}

void QFocusAdaptor::calculateValues()
{
    qDebug() << "QFocusAdaptor::calculateValues() -- target:" << d->target << "view:" << d->view;
    if (d->target && d->view)
    {
        QGLCamera *cam = d->view->camera();
        Q_ASSERT(cam);
        if (d->reset)
        {
            QGeometryData data = d->target->geometry();
            if (data.count() == 0 || d->target->count() == 0)
            {
                qWarning("Could not setup focus animation");
                return;
            }
            // assume that the first triangle referenced by this node is the one
            // you want to focus on - works for simple rects and like cases
            QGL::IndexArray ixs = data.indices();
            QVector3D a = data.vertexAt(ixs.at(d->target->start()));
            QVector3D b = data.vertexAt(ixs.at(d->target->start() + 1));
            QVector3D c = data.vertexAt(ixs.at(d->target->start() + 2));

            // assumes that first triangle is facing the camera
            QVector3D toCam = QVector3D::normal(a, b, c);

            // wont work very well if the target is not axis-aligned
            // find the distance q for the eye to be away from this object
            // in order that it is a tight fit in the viewport
            QBox3D box = d->target->boundingBox();
            QVector3D sz = box.size();
            qreal near = cam->nearPlane();
            QSizeF v = cam->viewSize();
            qreal qh = (near * v.height()) / sz.y();
            qreal qw = (near * v.width()) / sz.x();
            qreal q = qMax(qh, qw);

            d->sourceCenter = cam->center();
            d->sourceEye = cam->eye();

            d->targetCenter = box.center();
            d->targetEye = d->targetCenter + (toCam * q);
            d->reset = false;
        }
        cam->setCenter(d->sourceCenter + ((d->targetCenter - d->sourceCenter) * d->progress));
        cam->setEye(d->sourceEye + ((d->targetEye - d->sourceEye) * d->progress));
    }
}
