/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

//! [includes]
#include <QApplication>
#include <QtOpenGL/qgl.h>
#include "qglpainter.h"
//! [includes]

//! [class-defn]
class TutorialWidget : public QGLWidget
{
    Q_OBJECT
public:
    TutorialWidget(QWidget *parent=0) : QGLWidget(parent) {}

protected:
    void resizeGL(int width, int height);
    void paintGL();
};
//! [class-defn]

//! [set-viewport]
void TutorialWidget::resizeGL(int width, int height)
{
    QGLPainter painter(this);
    painter.setViewport(width, height);
}
//! [set-viewport]

//! [paintGL]
//! [clear-window]
void TutorialWidget::paintGL()
{
    QGLPainter painter(this);

    painter.setClearColor(palette().color(backgroundRole()));
    painter.clear();
//! [clear-window]

//! [set-matrices]
    painter.projectionMatrix().setToIdentity();
    painter.projectionMatrix().ortho(rect());

    painter.modelViewMatrix().setToIdentity();
//! [set-matrices]

//! [draw-all]
//! [set-effect]
    painter.setStandardEffect(QGL::FlatColor);
    painter.setColor(QColor(170, 202, 0, 255));
//! [set-effect]

//! [create-vertex-array]
    QGLVertexArray vertices(QGL::Position, 3);
    vertices.append(60.0f,  10.0f,  0.0f);
    vertices.append(110.0f, 110.0f, 0.0f);
    vertices.append(10.0f,  110.0f, 0.0f);
//! [create-vertex-array]

//! [draw]
    painter.setVertexArray(vertices);
    painter.draw(QGL::Triangles, 3);
//! [draw]
//! [draw-all]
}
//! [paintGL]

//! [main]
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TutorialWidget widget;
    widget.show();
    return app.exec();
}
//! [main]

#include "converting.moc"
