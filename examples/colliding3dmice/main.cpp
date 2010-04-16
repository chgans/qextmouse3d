/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "mouse.h"
#include "redcyaneffect.h"
#include "teapotitem.h"
#include "qglgraphicsnavigationitem.h"

#include <QtGui>
#include <QtOpenGL/qgl.h>

#include <math.h>

static const int MouseCount = 7;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    bool useTeapot = QApplication::arguments().contains(QLatin1String("-teapot"));

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    QGraphicsScene scene;
    if (useTeapot)
        scene.setSceneRect(-128, -128, 256, 256); // power of 2 for fbo
    else
        scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    for (int i = 0; i < MouseCount; ++i) {
        Mouse *mouse = new Mouse(!useTeapot);
        mouse->setPos(::sin((i * 6.28) / MouseCount) * 200,
                      ::cos((i * 6.28) / MouseCount) * 200);
        if (!useTeapot) {
            // Pick a random z value for the mouse to be drawn at
            // and create an effect object for the mouse.
            qreal z = qrand() % 15;
            RedCyanEffect *effect = new RedCyanEffect();
            effect->setZ(z);
            mouse->setGraphicsEffect(effect);
        }
        scene.addItem(mouse);
    }

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    timer.start(1000 / 33);

    if (useTeapot) {
        // Create an outer scene containing the teapot item and
        // set the mouse scene on it as its inner scene.
        QGraphicsScene outerScene;
        outerScene.setBackgroundBrush(Qt::black);

        TeapotItem *teapot = new TeapotItem();
        teapot->setRect(0, 0, 640, 480);
        teapot->setScene(&scene);
        teapot->setBackgroundBrush(QPixmap(":/images/cheese.jpg"));
        outerScene.addItem(teapot);

        QGLGraphicsNavigationItem *navigator = new QGLGraphicsNavigationItem();
        navigator->setViewportItem(teapot);
        outerScene.addItem(navigator);

        teapot->setFocus();

        QGraphicsView view(&outerScene);
        view.setViewport(new QGLWidget());
        view.show();

        return app.exec();
    } else {
        QGraphicsView view(&scene);
        view.setViewport(new QGLWidget());
        view.setRenderHint(QPainter::Antialiasing);
        view.setBackgroundBrush(QPixmap(":/images/cheese.jpg"));
        view.setCacheMode(QGraphicsView::CacheBackground);
        view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        view.setDragMode(QGraphicsView::ScrollHandDrag);
        view.setWindowTitle(QT_TRANSLATE_NOOP(QGraphicsView, "Colliding Mice"));
        view.resize(400, 300);
        view.show();

        return app.exec();
    }
}
