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

#include "cubeitem.h"
#include "qglpainter.h"
#include "qglcube.h"
#include "qray3d.h"
#include "qplane3d.h"
#include "qtriangle3d.h"
#include "qgraphicsembedscene.h"
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qpolygon.h>
#include <QtGui/qapplication.h>

const qreal CubeSize = 2.0f;

CubeItem::CubeItem(QGraphicsItem *parent)
    : QGLGraphicsViewportItem(parent)
    , mScene(0)
    , textureId(0)
    , navigating(false)
    , pressedFace(-1)
    , pressedButton(Qt::NoButton)
{
    startNavCamera = new QGLCamera();

    setFlag(ItemIsFocusable, true);

    QGLBuilder builder;
    builder.newSection(QGL::Faceted);
    builder << QGLCube(CubeSize);
    cube = builder.finalizedSceneNode();
}

CubeItem::~CubeItem()
{
    delete startNavCamera;
    delete cube;
}

void CubeItem::setScene(QGraphicsEmbedScene *scene)
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
    if (mScene)
        textureId = mScene->renderToTexture();

    // Now render the GL parts of the item using QGLPainter.
    QGLGraphicsViewportItem::paint(painter, option, widget);
}

void CubeItem::paintGL(QGLPainter *painter)
{
    if (textureId) {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        painter->setFaceColor(QGL::AllFaces, QColor(0, 0, 0, 200));
        painter->setStandardEffect(QGL::LitDecalTexture2D);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (painter->isFixedFunction())
            glEnable(GL_TEXTURE_2D);
        glCullFace(GL_FRONT);
        glEnable(GL_CULL_FACE);
        cube->draw(painter);
        glCullFace(GL_BACK);
        cube->draw(painter);
        glDisable(GL_CULL_FACE);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (painter->isFixedFunction())
            glDisable(GL_TEXTURE_2D);
    } else {
        painter->setStandardEffect(QGL::LitMaterial);
        cube->draw(painter);
    }
}

void CubeItem::updateScene()
{
    update();
}

void CubeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    int face;
    QPointF tc = cubeIntersection
        (event->widget(), event->pos().toPoint(), &face);
    if (!navigating && pressedFace == -1 && face != -1) {
        pressedFace = face;
        pressedButton = event->button();
        mScene->deliverEvent(event, tc);
        return;
    } else if (!navigating && face == -1) {
        navigating = true;
        pressedButton = event->button();
        pressedPos = event->pos().toPoint();
        startNavCamera->setEye(camera()->eye());
        startNavCamera->setCenter(camera()->center());
        startNavCamera->setUpVector(camera()->upVector());
#ifndef QT_NO_CURSOR
        setCursor(Qt::ClosedHandCursor);
#endif
        return;
    }
    QGraphicsItem::mousePressEvent(event);
}

void CubeItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (navigating) {
        QPoint delta = event->pos().toPoint() - pressedPos;
        int deltax = delta.x();
        int deltay = delta.y();
        QGLCamera *camera = this->camera();
        int rotation = camera->screenRotation();
        if (rotation == 90 || rotation == 270) {
            qSwap(deltax, deltay);
        }
        if (rotation == 90 || rotation == 180) {
            deltax = -deltax;
        }
        if (rotation == 180 || rotation == 270) {
            deltay = -deltay;
        }
        qreal anglex = deltax * 90.0f / rect().width();
        qreal angley = deltay * 90.0f / rect().height();
        QQuaternion q = startNavCamera->pan(-anglex);
        q *= startNavCamera->tilt(-angley);
        camera->setEye(startNavCamera->eye());
        camera->setCenter(startNavCamera->center());
        camera->setUpVector(startNavCamera->upVector());
        camera->rotateCenter(q);
    } else if (pressedFace != -1) {
        int face;
        QPointF tc = cubeIntersection
            (event->widget(), event->pos().toPoint(), &face);
        if (face != pressedFace)
            tc = QPointF(-1, -1);
        mScene->deliverEvent(event, tc);
        return;
    }
    QGraphicsItem::mouseMoveEvent(event);
}

void CubeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (navigating && pressedButton == event->button()) {
        navigating = false;
        pressedButton = Qt::NoButton;
#ifndef QT_NO_CURSOR
        unsetCursor();
#endif
        return;
    } else if (pressedFace != -1) {
        int face;
        QPointF tc = cubeIntersection
            (event->widget(), event->pos().toPoint(), &face);
        if (face != pressedFace)
            tc = QPoint(-1, -1);
        if (pressedButton == event->button()) {
            pressedFace = -1;
            pressedButton = Qt::NoButton;
        }
        mScene->deliverEvent(event, tc);
        return;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void CubeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    int face;
    QPointF tc = cubeIntersection
        (event->widget(), event->pos().toPoint(), &face);
    if (pressedFace == -1 && face != -1) {
        pressedFace = face;
        pressedButton = event->button();
        mScene->deliverEvent(event, tc);
        return;
    }
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

QPointF CubeItem::cubeIntersection
    (QWidget *widget, const QPoint &point, int *actualFace) const
{
    // Bail out if no scene.
    if (!mScene) {
        *actualFace = -1;
        return QPointF();
    }

    // Get the combined matrix for the projection.
    int dpiX = widget->logicalDpiX();
    int dpiY = widget->logicalDpiY();
    QRectF bounds = boundingRect();
    qreal aspectRatio = (bounds.width() * dpiY) / (bounds.height() * dpiX);
    QMatrix4x4 mv = camera()->modelViewMatrix();
    QMatrix4x4 proj = camera()->projectionMatrix(aspectRatio);

    // Find the relative position of the point within (-1, -1) to (1, 1).
    QPointF relativePoint =
        QPointF((point.x() - bounds.center().x()) * 2 / bounds.width(),
                -(point.y() - bounds.center().y()) * 2 / bounds.height());

    // Get the ray extending from the eye through the point the user selected.
    QVector3D eyept = proj.inverted().map
        (QVector3D(relativePoint.x(), relativePoint.y(), -1.0f));
    QRay3D ray(QVector3D(0, 0, 0), eyept);

    // Determine which face of the cube contains the point.
    QVector3D pt1, pt2, pt3, pt4;
    QVector2D tc1, tc2, tc3;
    bool singleFace = (pressedFace != -1);
    for (int face = 0; face < 6; ++face) {
        if (singleFace && face != pressedFace)
            continue;

        // Test the two triangles on the face for an intersection.
        pt1 = QVector3D(vertexData[face * 4 * 3],
                        vertexData[face * 4 * 3 + 1],
                        vertexData[face * 4 * 3 + 2]);
        pt2 = QVector3D(vertexData[face * 4 * 3 + 3],
                        vertexData[face * 4 * 3 + 4],
                        vertexData[face * 4 * 3 + 5]);
        pt3 = QVector3D(vertexData[face * 4 * 3 + 6],
                        vertexData[face * 4 * 3 + 7],
                        vertexData[face * 4 * 3 + 8]);
        pt4 = QVector3D(vertexData[face * 4 * 3 + 9],
                        vertexData[face * 4 * 3 + 10],
                        vertexData[face * 4 * 3 + 11]);
        pt1 = mv.map(pt1);
        pt2 = mv.map(pt2);
        pt3 = mv.map(pt3);
        pt4 = mv.map(pt4);
        QTriangle3D triangle(pt1, pt2, pt3);
        qreal t = triangle.intersection(ray);
        if (qIsNaN(t)) {
            triangle.setQ(pt3);
            triangle.setR(pt4);
            t = triangle.intersection(ray);
            if (qIsNaN(t)) {
                if (!singleFace)
                    continue;
                // The mouse probably moved outside the face while the
                // mouse button was held down.  Use the triangle's plane
                // to compute a virtual texture co-ordinate.
                t = triangle.plane().intersection(ray);
            }
            tc1 = QVector2D(1.0f, 0.0f);
            tc2 = QVector2D(0.0f, 1.0f);
            tc3 = QVector2D(0.0f, 0.0f);
        } else {
            tc1 = QVector2D(1.0f, 0.0f);
            tc2 = QVector2D(1.0f, 1.0f);
            tc3 = QVector2D(0.0f, 1.0f);
        }

        // We want the face that is pointing towards the user.
        QVector3D v = QVector3D::crossProduct(pt2 - pt1, pt3 - pt1);
        if (!singleFace && v.z() <= 0.0f)
            continue;

        // Get the texture co-ordinate corresponding to the intersection.
        QVector2D uv = triangle.uv(ray.point(t));
        QVector2D tc =
            uv.x() * tc1 + uv.y() * tc2 + (1 - uv.x() - uv.y()) * tc3;
        *actualFace = face;
        return QPointF(tc.x(), tc.y());
    }

    *actualFace = -1;
    return QPointF();
}
