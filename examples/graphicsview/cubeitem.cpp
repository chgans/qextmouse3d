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
#include "qline3d.h"
#include "qplane3d.h"
#include <QtOpenGL/qglframebufferobject.h>
#include <QtGui/qgraphicsscene.h>
#include <QtGui/qgraphicssceneevent.h>
#include <QtGui/qgraphicsview.h>
#include <QtGui/qpolygon.h>
#include <QtGui/qapplication.h>

const qreal CubeSize = 2.0f;

CubeItem::CubeItem(QGraphicsItem *parent)
    : QGLGraphicsViewportItem(parent)
    , mScene(0)
    , fbo(0)
    , navigating(false)
    , pressedFace(-1)
    , pressedButton(Qt::NoButton)
{
    startNavCamera = new QGLCamera();

    setFlag(ItemIsFocusable, true);

    cube.newSection(QGL::Faceted);
    cube << QGLCube(CubeSize);
}

CubeItem::~CubeItem()
{
    delete startNavCamera;
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
        QLinearGradient gradient(rect.topLeft(), rect.bottomRight());
        gradient.setColorAt(0, QColor(0, 128, 192, 255));
        gradient.setColorAt(1, QColor(0, 0, 128, 255));
        fboPainter.setPen(QPen(Qt::black, 3));
        fboPainter.setBrush(gradient);
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
        painter->setDepthTestingEnabled(false);
        painter->setBlendingEnabled(true);
        painter->setFaceColor(QGL::AllFaces, QColor(0, 0, 0, 200));
        painter->setStandardEffect(QGL::LitDecalTexture2D);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#if !defined(QGL_SHADERS_ONLY)
        glEnable(GL_TEXTURE_2D);
#endif
        painter->setCullFaces(QGL::CullFrontFaces);
        cube.draw(painter);
        painter->setCullFaces(QGL::CullBackFaces);
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
    int face;
    QPoint pos = cubeIntersection
        (event->widget(), event->pos().toPoint(), &face);
    if (!navigating && pressedFace == -1 && face != -1) {
        pressedFace = face;
        pressedButton = event->button();
        pressedPos = pos;
        deliverSceneEvent(pos, event);
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
        QPoint pos = cubeIntersection
            (event->widget(), event->pos().toPoint(), &face);
        if (face != pressedFace)
            pos = QPoint(-1, -1);
        deliverSceneEvent(pos, event);
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
        QPoint pos = cubeIntersection
            (event->widget(), event->pos().toPoint(), &face);
        if (face != pressedFace)
            pos = QPoint(-1, -1);
        if (pressedButton == event->button()) {
            pressedFace = -1;
            pressedButton = Qt::NoButton;
        }
        deliverSceneEvent(pos, event);
        return;
    }
    QGraphicsItem::mouseReleaseEvent(event);
}

void CubeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    int face;
    QPoint pos = cubeIntersection
        (event->widget(), event->pos().toPoint(), &face);
    if (pressedFace == -1 && face != -1) {
        pressedFace = face;
        pressedButton = event->button();
        deliverSceneEvent(pos, event);
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

QPoint CubeItem::cubeIntersection
    (QWidget *widget, const QPoint &point, int *actualFace) const
{
    // Bail out if no scene.
    if (!mScene) {
        *actualFace = -1;
        return QPoint();
    }

    // Get the combined matrix for the projection.
    int dpiX = widget->logicalDpiX();
    int dpiY = widget->logicalDpiY();
    QRectF bounds = boundingRect();
    qreal aspectRatio = (bounds.width() * dpiY) / (bounds.height() * dpiX);
    QMatrix4x4 mv = camera()->modelViewMatrix();
    QMatrix4x4 proj = camera()->projectionMatrix(aspectRatio);
    QMatrix4x4 combined = proj * mv;

    // Find the relative position of the point within (-1, -1) to (1, 1).
    QPointF relativePoint =
        QPointF((point.x() - bounds.center().x()) * 2 / bounds.width(),
                -(point.y() - bounds.center().y()) * 2 / bounds.height());

    // Determine which face of the cube contains the point.
    QVector3D pt1, pt2, pt3, pt4;
    bool singleFace = (pressedFace != -1);
    for (int face = 0; face < 6; ++face) {
        if (singleFace && face != pressedFace)
            continue;

        // Create a polygon from the projected version of the face
        // so that we can test for point membership.
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
        QVector<QPointF> points2d;
        points2d.append((combined * pt1).toPointF());
        points2d.append((combined * pt2).toPointF());
        points2d.append((combined * pt3).toPointF());
        points2d.append((combined * pt4).toPointF());
        QPolygonF polygon(points2d);
        if (!singleFace) {
            if (!polygon.containsPoint(relativePoint, Qt::OddEvenFill))
                continue;
        }

        // We want the face that is pointing towards the user.
        QVector3D v = mv.mapVector
            (QVector3D::crossProduct(pt2 - pt1, pt3 - pt1));
        if (!singleFace && v.z() <= 0.0f)
            continue;

        // Determine the intersection between the cube face and
        // the ray coming from the eye position.
        QVector3D eyept = proj.inverted().map
            (QVector3D(relativePoint.x(), relativePoint.y(), -1.0f));
        QLine3D ray(QVector3D(0, 0, 0), eyept);
        QPlane3D plane(mv * pt1, v);
        QResult<QVector3D> intersection = plane.intersection(ray);
        if (!intersection.isValid())
            continue;
        QVector3D worldpt = mv.inverted().map(intersection.value());

        // Map the world point to the range 0..1.
        worldpt = (worldpt / CubeSize) + QVector3D(0.5f, 0.5f, 0.5f);

        // Figure out the texture co-ordinates on the face that
        // correspond to the point.
        qreal xtex, ytex;
        switch (face) {
        case 0:
            xtex = 1.0f - worldpt.y();
            ytex = 1.0f - worldpt.z();
            break;
        case 1:
            xtex = 1.0f - worldpt.x();
            ytex = 1.0f - worldpt.z();
            break;
        case 2:
            xtex = worldpt.y();
            ytex = 1.0f - worldpt.z();
            break;
        case 3:
            xtex = worldpt.x();
            ytex = 1.0f - worldpt.z();
            break;
        case 4:
            xtex = worldpt.x();
            ytex = 1.0f - worldpt.y();
            break;
        case 5: default:
            xtex = worldpt.x();
            ytex = worldpt.y();
            break;
        }

        // Turn the texture co-ordinates into scene co-ordinates.
        bounds = mScene->sceneRect();
        xtex *= bounds.width();
        ytex *= bounds.height();
        int x = qRound(xtex);
        int y = qRound(ytex);
        if (x < 0)
            x = 0;
        else if (x >= bounds.width())
            x = qRound(bounds.width() - 1);
        if (y < 0)
            y = 0;
        else if (y >= bounds.height())
            y = qRound(bounds.height() - 1);
        *actualFace = face;
        return QPoint(x, y);
    }

    *actualFace = -1;
    return QPoint();
}

void CubeItem::deliverSceneEvent
    (const QPoint &point, QGraphicsSceneMouseEvent *event)
{
    QRectF srect = mScene->sceneRect();
    QPointF spoint = QPointF(point.x() + srect.x(), point.y() + srect.y());
    QGraphicsSceneMouseEvent e(event->type());
    e.setScenePos(spoint);
    e.setScreenPos(point);
    e.setButtonDownScreenPos(event->button(), pressedPos);
    e.setButtonDownScenePos
        (event->button(), QPointF(pressedPos.x() + srect.x(),
                                  pressedPos.y() + srect.y()));
    e.setButtons(event->buttons());
    e.setButton(event->button());
    e.setModifiers(event->modifiers());
    e.setAccepted(false);
    QApplication::sendEvent(mScene, &e);
}
