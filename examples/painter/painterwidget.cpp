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

#include "painterwidget.h"
#include "qglcube.h"

void PainterWidget::initializeGL()
{
    QGLPainter painter(this);

    // Turn on depth testing.
    glEnable(GL_DEPTH_TEST);

    // Create the cube object for later.
    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    builder << QGLCube(1.5f);
    cube = builder.finalizedSceneNode();
    cube->setParent(this);
}

void PainterWidget::resizeGL(int width, int height)
{
    QGLPainter painter(this);
    painter.setViewport(width, height);
}

void PainterWidget::paintGL()
{
    QGLPainter painter(this);

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
