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

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QtOpenGL/qgl.h>
#include "teapotitem.h"
#include "qglcamera.h"
#include "qglgraphicsnavigationitem.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QGraphicsScene scene;
    scene.setBackgroundBrush(Qt::black);

    TeapotItem *item = new TeapotItem();
    item->setRect(0, 0, 600, 480);
    scene.addItem(item);

    TeapotItem *item2 = new TeapotItem();
    item2->setRect(400, 0, 200, 150);
    item2->camera()->setEye(QVector3D(8.0f, 0.0f, 6.0f));
    item2->setBackgroundColor(QColor(255, 0, 0, 64));
    item2->setZValue(1);
    scene.addItem(item2);

    QGLGraphicsNavigationItem *navigator = new QGLGraphicsNavigationItem();
    navigator->setViewportItem(item);
    scene.addItem(navigator);

    QGLGraphicsNavigationItem *navigator2 = new QGLGraphicsNavigationItem();
    navigator2->setViewportItem(item2);
    scene.addItem(navigator2);

    item->setFocus();

    QGraphicsView view(&scene);
    view.setViewport(new QGLWidget());
    view.show();

    return app.exec();
}
