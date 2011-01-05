/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mouse.h"
#include "stereoeffect.h"
#include "stereographicsview.h"
#include "teapotitem.h"

#include <QtGui>
#include <QtOpenGL/qgl.h>

#include <math.h>

static const int MouseCount = 7;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    bool useTeapot = QApplication::arguments().contains(QLatin1String("-teapot"));
    bool useStereo = StereoGraphicsView::useStereo();

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    QGraphicsScene scene;
    if (useTeapot)
        scene.setSceneRect(-128, -128, 256, 256); // power of 2 for fbo
    else
        scene.setSceneRect(-300, -300, 600, 600);
    scene.setItemIndexMethod(QGraphicsScene::NoIndex);

    for (int i = 0; i < MouseCount; ++i) {
        Mouse *mouse = new Mouse(!useTeapot && !useStereo);
        mouse->setPos(::sin((i * 6.28) / MouseCount) * 200,
                      ::cos((i * 6.28) / MouseCount) * 200);
        if (!useTeapot) {
            // Pick a random z value for the mouse to be drawn at
            // and create an effect object for the mouse.
            qreal z = qrand() % 15;
            StereoEffect *effect = new StereoEffect();
            effect->setZ(z);
            mouse->setGraphicsEffect(effect);
        }
        scene.addItem(mouse);
    }

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &scene, SLOT(advance()));
    timer.start(1000 / 33);

    if (!useTeapot && useStereo) {
        // Create two QGraphicsView's showing the left and right eyes.
        StereoGraphicsView view;
        view.setScene(&scene);
        QPixmap cheese(QLatin1String(":/images/cheese.jpg"));
        view.leftEye()->setBackgroundBrush(cheese);
        view.rightEye()->setBackgroundBrush(cheese);
        QSize size = view.leftEye()->size();
        scene.setSceneRect(-size.width() / 2, -size.height() / 2,
                           size.width(), size.height());
        view.show();
        return app.exec();
    } else if (useTeapot) {
        // Create an outer scene containing the teapot item and
        // set the mouse scene on it as its inner scene.
        QGraphicsScene outerScene;
        outerScene.setBackgroundBrush(Qt::black);

        TeapotItem *teapot = new TeapotItem();
        teapot->setRect(0, 0, 640, 480);
        teapot->setScene(&scene);
        teapot->setBackgroundBrush(QPixmap(QLatin1String(":/images/cheese.jpg")));
        outerScene.addItem(teapot);

        teapot->setFocus();

        QGraphicsView view(&outerScene);
        view.setViewport(new QGLWidget());
        view.show();

        return app.exec();
    } else {
        QGraphicsView view(&scene);
        view.setViewport(new QGLWidget());
        view.setRenderHint(QPainter::Antialiasing);
        view.setBackgroundBrush(QPixmap(QLatin1String(":/images/cheese.jpg")));
        view.setCacheMode(QGraphicsView::CacheBackground);
        view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        view.setDragMode(QGraphicsView::ScrollHandDrag);
        view.resize(400, 300);
        view.show();

        return app.exec();
    }
}
