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

#include <QPropertyAnimation>

#include "basketview.h"
#include "basket_data.h"
#include "qglbuilder.h"
#include "qglscenenode.h"

BasketView::BasketView(QWidget *parent)
    : QGLView(parent)
    , m_angle(0)
{
    setWindowTitle(tr("Basket"));

//! [1]
    QGLBuilder builder;
    builder << BasketPatches();
    basket = builder.finalizedSceneNode();

    QGLMaterial *mat = new QGLMaterial;
    QUrl url;
    url.setPath(QLatin1String(":/basket.jpg"));
    url.setScheme(QLatin1String("file"));
    mat->setTextureUrl(url);
    basket->setMaterial(mat);
    basket->setEffect(QGL::FlatReplaceTexture2D);
//! [1]

//! [2]
    QPropertyAnimation *animation;
    animation = new QPropertyAnimation(this, "angle", this);
    animation->setStartValue(0.0f);
    animation->setEndValue(360.0f);
    animation->setDuration(2000);
    animation->setLoopCount(-1);
    animation->start();
//! [2]

    camera()->setEye(QVector3D(0, 4, 10));
}

BasketView::~BasketView()
{
    delete basket;
}

//! [3]
void BasketView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().rotate(angle(), 0, 1, 0);
    painter->modelViewMatrix().scale(1.5f);
    basket->draw(painter);
}
//! [3]
