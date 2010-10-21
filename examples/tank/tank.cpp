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

#include "tank.h"

#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QImage>
#include <QPainter>
#include <QSize>

#ifndef QT_NO_DEBUG
#include <QThread>
#include <QCoreApplication>
#endif

#include "qglbuilder.h"
#include "qglcylinder.h"
#include "qglmaterial.h"
#include "qgltexture2d.h"

static inline int rval()
{
    // return an int 64 <--> 192 inclusive
    return (qreal(qrand()) / qreal(RAND_MAX)) * 128 + 64;
}

static QGLMaterial *qCreateFluid()
{
    QImage image(QSize(128,128), QImage::Format_ARGB32);
    QRgb col = qRgba(rval(), rval(), rval(), 196);
    image.fill(col);
    QPainter painter(&image);
    QLinearGradient linearGrad(QPointF(56, 56), QPointF(72, 72));
    linearGrad.setColorAt(0, col);
    linearGrad.setColorAt(1, QColor(col).lighter());
    linearGrad.setSpread(QGradient::ReflectSpread);
    painter.fillRect(image.rect(), linearGrad);
    painter.end();
    QGLMaterial *mat = new QGLMaterial;
    QColor white(Qt::white);
    white.setAlpha(128);
    mat->setAmbientColor(white);
    mat->setDiffuseColor(white);
    QGLTexture2D *tex = new QGLTexture2D;
    tex->setImage(image);
    mat->setTexture(tex);
    return mat;
}

Tank::Tank(QObject *parent) :
    QGLSceneNode(parent)
{
    QSequentialAnimationGroup *seq = new QSequentialAnimationGroup(this);
    QPropertyAnimation *anim = new QPropertyAnimation(this, "scale");
    anim->setDuration(10000);
    anim->setStartValue(QVector3D(1.0f, 0.1f, 1.0f));
    anim->setEndValue(QVector3D(1.0f, 1.2f, 1.0f));
    anim->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    seq->addAnimation(anim);
    seq->addPause(2000);
    anim = new QPropertyAnimation(this, "scale");
    anim->setDuration(10000);
    anim->setStartValue(QVector3D(1.0f, 1.2f, 1.0f));
    anim->setEndValue(QVector3D(1.0f, 0.1f, 1.0f));
    anim->setEasingCurve(QEasingCurve(QEasingCurve::InOutQuad));
    seq->addAnimation(anim);
    seq->setLoopCount(-1);
    seq->start();

    addNode(tankObject());
    setMaterial(qCreateFluid());
}

QGLSceneNode *Tank::tankObject()
{
#ifndef QT_NO_DEBUG
    // NOT THREADSAFE, NOT RE-ENTRANT
    Q_ASSERT(QThread::currentThread() == qApp->thread());
#endif
    static QGLSceneNode *theTank = 0;
    if (!theTank)
    {
        QGLBuilder build;
        build << QGLCylinder(1.0f, 1.0f, 1.0f, 32);
        theTank = build.finalizedSceneNode();
        theTank->setObjectName("Tank Component");
        theTank->setRotX(-90.0f);
        theTank->setY(-theTank->boundingBox().minimum().y());
    }
    return theTank;
}
