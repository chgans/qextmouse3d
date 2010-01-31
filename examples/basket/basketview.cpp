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

#include <QTimer>
#include "basketview.h"
#include "basket_data.h"

BasketView::BasketView(QWidget *parent)
    : QGLView(parent)
{
    setWindowTitle(tr("Basket"));

    angle = 0;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(rotate()));
    timer->start(10);
}

BasketView::~BasketView()
{
}

void BasketView::initializeGL(QGLPainter *painter)
{
    basket << BasketPatches();
    basket.finalize();

    QImage textureImage(QLatin1String(":/basket.jpg"));
    texture.setImage(textureImage);

    painter->setStandardEffect(QGL::FlatReplaceTexture2D);
    painter->setTexture(&texture);
}

void BasketView::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().scale(1.5f);
    painter->modelViewMatrix().rotate(angle, 0, 1, 0);
    basket.draw(painter);
}

void BasketView::rotate()
{
    angle = (angle + 5) % 360;
    updateGL();
}
