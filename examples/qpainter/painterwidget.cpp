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

#include "painterwidget.h"
#include "qglcube.h"
#include "qglbuilder.h"

#include <QPainter>

PainterWidget::PainterWidget(QWidget *parent)
    : QWidget(parent)
{
    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    builder << QGLCube(1.5f);
    cube = builder.finalizedSceneNode();
    cube->setParent(this);
}

void PainterWidget::paintGL(QPainter *p)
{
    QGLPainter painter;
    if (!painter.begin(p)) {
        qWarning("GL graphics system needs to be activated with '-graphicssystem OpenGL'");
        return;
    }

    // Turn on depth testing.
    glEnable(GL_DEPTH_TEST);

    // Clear the window to the standard window background color.
    painter.setClearColor(palette().color(backgroundRole()));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw a triangle in an orthographic projection.
    QMatrix4x4 projm;
    projm.ortho(rect());
    painter.projectionMatrix() = projm;

    painter.modelViewMatrix().setToIdentity();

    QVector3DArray vertices;
    vertices.append(60.0,  10.0,  0.0);
    vertices.append(110.0, 110.0, 0.0);
    vertices.append(10.0,  110.0, 0.0);

    painter.setStandardEffect(QGL::FlatColor);
    painter.setVertexAttribute(QGL::Position, vertices);
    painter.setColor(QColor(0, 128, 0, 255));

    painter.draw(QGL::Triangles, 3);

    // Draw a cube in a perspective projection centered in the window.
    painter.setStandardEffect(QGL::LitMaterial);
    QMatrix4x4 projection;
    projection.perspective(30, painter.aspectRatio(), 5, 1000);
    projection.lookAt
            (QVector3D(0.0f, 0.0f, 10.0f),
             QVector3D(0.0f, 0.0f, 0.0f),
             QVector3D(0.0f, 1.0f, 0.0f));
    painter.projectionMatrix() = projection;
    painter.modelViewMatrix().rotate(45.0f, 1.0f, 1.0f, 1.0f);

    painter.setFaceColor(QGL::AllFaces, QColor(170, 202, 0));

    cube->draw(&painter);
}

void PainterWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    paintGL(&painter);
}
