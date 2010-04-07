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

#include "cubeitem.h"
#include "qglpainter.h"
#include "qglcube.h"
#include "qbox3d.h"
#include "qline3d.h"
#include "qtriangle3d.h"
#include <QtOpenGL/qglframebufferobject.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneevent.h>

const qreal CubeSize = 2.5f;

CubeItem::CubeItem(QGraphicsItem *parent)
    : QGLGraphicsViewportItem(parent)
    , mScene(0)
    , fbo(0)
{
    setFlag(ItemIsFocusable, true);

    cube.newSection(QGL::Faceted);
    cube << QGLCube(CubeSize);
}

CubeItem::~CubeItem()
{
}

void CubeItem::setScene(QGraphicsScene *scene)
{
    mScene = scene;
    connect(scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(updateScene()));
}

void CubeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // Render the inner scene into a framebuffer object.
    // We do this while the ordinary Qt paint engine has
    // control of the GL context rather than later when the
    // QGLPainter has control of the GL context.
    if (mScene) {
        if (!fbo) {
            QGLFramebufferObjectFormat format;
            format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
            //format.setSamples(8);
            fbo = new QGLFramebufferObject
                (mScene->sceneRect().size().toSize(), format);
        }
        QRectF rect(0, 0, fbo->size().width(), fbo->size().height());
        QPainter fboPainter(fbo);
        fboPainter.save();
        fboPainter.setPen(QPen(Qt::black, 3));
        fboPainter.setBrush(QBrush(QColor(230, 200, 167)));
        fboPainter.drawRect(rect);
        fboPainter.restore();
        mScene->render(&fboPainter, rect);
    }

    // Now render the GL parts of the item using QGLPainter.
    QGLGraphicsViewportItem::paint(painter, option, widget);
}

void CubeItem::paintGL(QGLPainter *painter)
{
    if (fbo) {
        painter->setStandardEffect(QGL::FlatReplaceTexture2D);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if !defined(QGL_SHADERS_ONLY)
        glEnable(GL_TEXTURE_2D);
#endif
        cube.draw(painter);
        glBindTexture(GL_TEXTURE_2D, 0);
#if !defined(QGL_SHADERS_ONLY)
        glDisable(GL_TEXTURE_2D);
#endif
    } else {
        painter->setStandardEffect(QGL::LitMaterial);
        cube.draw(painter);
    }
}

void CubeItem::updateScene()
{
    update();
}

void CubeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QVector3D pos = cubeIntersection(event->pos().toPoint());
    QGraphicsItem::mousePressEvent(event);
}

void CubeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);
}

void CubeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void CubeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseDoubleClickEvent(event);
}

static const int vertexDataLen = 6 * 4 * 3;

static const float vertexData[vertexDataLen] = {
    -0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,

    -0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,

    0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,

    0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,

    0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,
    0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, 0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, 0.5f * CubeSize,

    0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize,
    0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, -0.5f * CubeSize, -0.5f * CubeSize,
    -0.5f * CubeSize, 0.5f * CubeSize, -0.5f * CubeSize
};

QVector3D CubeItem::cubeIntersection(const QPoint &point) const
{
    // Get the ray from the eye through the point in eye co-ordinates.
    QRectF bounds = boundingRect();
    QVector3D v = camera()->mapPoint
        (point, bounds.width() / bounds.height(), bounds.size().toSize());
    QLine3D ray(QVector3D(0, 0, 0), v);

    // Get the modelview matrix from the camera.
    QMatrix4x4 mv = camera()->modelViewMatrix();

    // Determine which face of the cube intersects the ray with
    // the nearest z value.
    int closestFace = -1;
    qreal closestZ = 0.0f;
    for (int face = 0; face < 6; ++face) {
        QTriangle3D tri1(QVector3D(vertexData[face * 4 * 3],
                                   vertexData[face * 4 * 3 + 1],
                                   vertexData[face * 4 * 3 + 2]),
                         QVector3D(vertexData[face * 4 * 3 + 3],
                                   vertexData[face * 4 * 3 + 4],
                                   vertexData[face * 4 * 3 + 5]),
                         QVector3D(vertexData[face * 4 * 3 + 6],
                                   vertexData[face * 4 * 3 + 7],
                                   vertexData[face * 4 * 3 + 8]));
        tri1 = tri1.transformed(mv);
        QResult<QVector3D> result;
        result = tri1.intersection(ray);
        if (result.isValid()) {
            if (closestFace == -1 || closestZ < result.value().z()) {
                closestFace = face;
                closestZ = result.value().z(); // z will be -ve.
            }
            continue;
        }
        QTriangle3D tri2(QVector3D(vertexData[face * 4 * 3],
                                   vertexData[face * 4 * 3 + 1],
                                   vertexData[face * 4 * 3 + 2]),
                         QVector3D(vertexData[face * 4 * 3 + 6],
                                   vertexData[face * 4 * 3 + 7],
                                   vertexData[face * 4 * 3 + 8]),
                         QVector3D(vertexData[face * 4 * 3 + 9],
                                   vertexData[face * 4 * 3 + 10],
                                   vertexData[face * 4 * 3 + 11]));
        tri2 = tri2.transformed(mv);
        result = tri2.intersection(ray);
        if (result.isValid()) {
            if (closestFace == -1 || closestZ < result.value().z()) {
                closestFace = face;
                closestZ = result.value().z(); // z will be -ve.
            }
        }
    }

    return v;
}
