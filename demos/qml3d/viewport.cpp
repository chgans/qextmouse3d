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

#include "viewport.h"
#include "effect.h"
#include "qml3dview.h"
#include "qgldepthbufferoptions.h"
#include "qglblendoptions.h"
#include "qgllightmodel.h"
#include "qgllightparameters.h"
#include "qglcamera.h"
#include <QtGui/qpainter.h>

QT_BEGIN_NAMESPACE

QML_DEFINE_TYPE(Qt,4,6,Viewport,Viewport)
QML_DEFINE_TYPE(Qt,4,6,LightModel,QGLLightModel)
QML_DEFINE_TYPE(Qt,4,6,Light,QGLLightParameters)
QML_DEFINE_TYPE(Qt,4,6,Camera,QGLCamera)

class ViewportPrivate
{
public:
    ViewportPrivate();

    bool picking;
    bool showPicking;
    bool navigation;
    bool itemsInitialized;
    QGLCamera *camera;
    QGLDepthBufferOptions depthBufferOptions;
    QGLBlendOptions blendOptions;
    QGLLightModel *lightModel;
    Effect *backdrop;
    QGLVertexArray backdropVertices;
    Qml3dView *view;
};

ViewportPrivate::ViewportPrivate()
    : picking(true)
    , showPicking(false)
    , navigation(true)
    , itemsInitialized(false)
    , camera(0)
    , lightModel(0)
    , backdrop(0)
    , backdropVertices(QGL::Position, 2, QGL::TextureCoord0, 2)
    , view(0)
{
    depthBufferOptions.setEnabled(true);
    depthBufferOptions.setFunction(QGLDepthBufferOptions::Less);

    blendOptions.setEnabled(true);
    blendOptions.setSourceColorFactor(QGLBlendOptions::SrcAlpha);
    blendOptions.setSourceAlphaFactor(QGLBlendOptions::SrcAlpha);
    blendOptions.setDestinationColorFactor(QGLBlendOptions::OneMinusSrcAlpha);
    blendOptions.setDestinationAlphaFactor(QGLBlendOptions::OneMinusSrcAlpha);

    // Construct the vertices for a quad with (0, 0) as the
    // texture co-ordinate for the bottom-left of the screen
    // and (1, 1) as the texture co-ordinate for the top-right.
    backdropVertices.append(-1.0f, -1.0f);
    backdropVertices.append(0.0f, 0.0f);
    backdropVertices.append(-1.0f, 1.0f);
    backdropVertices.append(0.0f, 1.0f);
    backdropVertices.append(1.0f, 1.0f);
    backdropVertices.append(1.0f, 1.0f);
    backdropVertices.append(1.0f, -1.0f);
    backdropVertices.append(1.0f, 0.0f);
}

Viewport::Viewport(QmlGraphicsItem *parent)
    : QmlGraphicsItem(parent)
{
    d = new ViewportPrivate();
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

Viewport::~Viewport()
{
    delete d;
}

bool Viewport::picking() const
{
    return d->picking;
}

void Viewport::setPicking(bool value)
{
    d->picking = value;
    update3d();
}

bool Viewport::showPicking() const
{
    return d->showPicking;
}

void Viewport::setShowPicking(bool value)
{
    d->showPicking = value;
    update3d();
}

bool Viewport::navigation() const
{
    return d->navigation;
}

void Viewport::setNavigation(bool value)
{
    d->navigation = value;
    update3d();
}

QGLCamera *Viewport::camera() const
{
    return d->camera;
}

void Viewport::setCamera(QGLCamera *value)
{
    if (d->camera != value) {
        if (d->camera) {
            disconnect(d->camera, SIGNAL(projectionChanged()),
                       this, SLOT(cameraChanged()));
            disconnect(d->camera, SIGNAL(viewChanged()),
                       this, SLOT(cameraChanged()));
        }
        d->camera = value;
        if (d->camera) {
            connect(d->camera, SIGNAL(projectionChanged()),
                    this, SLOT(cameraChanged()));
            connect(d->camera, SIGNAL(viewChanged()),
                    this, SLOT(cameraChanged()));
        }
        cameraChanged();
    }
}

QGLLightModel *Viewport::lightModel() const
{
    return d->lightModel;
}

void Viewport::setLightModel(QGLLightModel *value)
{
    if (d->lightModel != value) {
        if (d->lightModel) {
            disconnect(d->lightModel, SIGNAL(lightModelChanged()),
                       this, SLOT(update3d()));
        }
        d->lightModel = value;
        if (d->lightModel) {
            connect(d->lightModel, SIGNAL(lightModelChanged()),
                    this, SLOT(update3d()));
        }
        update3d();
    }
}

Effect *Viewport::backdrop() const
{
    return d->backdrop;
}

void Viewport::setBackdrop(Effect *value)
{
    if (d->backdrop != value) {
        if (d->backdrop) {
            disconnect(d->backdrop, SIGNAL(effectChanged()),
                       this, SLOT(update3d()));
        }
        d->backdrop = value;
        if (d->backdrop) {
            connect(d->backdrop, SIGNAL(effectChanged()),
                    this, SLOT(update3d()));
        }
        update3d();
    }
}

void Viewport::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    QGLPainter painter;
    if (!painter.begin(p)) {
        qDebug("GL graphics system is not active; cannot use 3D items");
        return;
    }

    // Initialize the objects in the scene if this is the first paint.
    if (!d->itemsInitialized)
        initialize(0, &painter);

    // Modify the GL viewport to only cover the extent of this QmlGraphicsItem.
    QTransform transform = p->transform();
    painter.setViewport(QRect(qRound(transform.dx()), qRound(transform.dy()),
                              width(), height()));

    // Perform early drawing operations.
    earlyDraw(&painter);

    // Set up the scene the way Qml3dView would if we were using it.
    d->depthBufferOptions.apply(&painter);
    d->blendOptions.apply(&painter);
    painter.setCullFaces(QGL::CullDisabled);
    if (d->camera) {
        d->camera->apply(&painter, QSize(width(), height()));
    } else {
        QGLCamera defCamera;
        defCamera.apply(&painter, QSize(width(), height()));
    }

    // Draw the Item3d children.
    draw(&painter);
}

void Viewport::earlyDraw(QGLPainter *painter)
{
    // If we have a QmlGraphicsItem parent, then assume that it has cleared
    // the screen (e.g. Rect::color), and just clear the depth buffer.
    if (parentItem())
        painter->clear(GL_DEPTH_BUFFER_BIT);
    else
        painter->clear();

    // If we have a scene backdrop, then draw it now.
    if (d->backdrop) {
        painter->projectionMatrix().setToIdentity();
        painter->modelViewMatrix().setToIdentity();
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDepthMask(GL_FALSE);

        // Select the effect and draw the backdrop quad.
        d->backdrop->enableEffect(painter);
        painter->setVertexArray(d->backdropVertices);
        painter->draw(QGL::TriangleFan, 4);
        d->backdrop->disableEffect(painter);

        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
    }

#ifdef GL_RESCALE_NORMAL
    glEnable(GL_RESCALE_NORMAL);
#endif
}

void Viewport::draw(QGLPainter *painter)
{
    painter->setObjectPickId(-1);
    QObjectList list = QObject::children();
    bool haveLights = false;
    foreach (QObject *child, list) {
        QGLLightParameters *light = qobject_cast<QGLLightParameters *>(child);
        if (light) {
            painter->setLightParameters(0, light); // XXX - non-zero lights
            painter->setLightEnabled(0, true);
            haveLights = true;
        }
    }
    painter->setLightModel(d->lightModel);
    foreach (QObject *child, list) {
        Item3d *item = qobject_cast<Item3d *>(child);
        if (item)
            item->draw(painter);
    }
    if (haveLights) {
        foreach (QObject *child, list) {
            QGLLightParameters *light = qobject_cast<QGLLightParameters *>(child);
            if (light) {
                painter->setLightParameters(0, 0);  // XXX - non-zero lights
                painter->setLightEnabled(0, false);
            }
        }
    }
}

// If "view" is null, then we are running on a standard QFxView
// canvas widget.  If "view" is not null, then we are running
// on a Qml3dView canvas widget.
void Viewport::initialize(Qml3dView *view, QGLPainter *painter)
{
    // Remember which view we are associate with, if any.
    d->view = view;

    // Apply the camera to the view.
    if (view && d->camera)
        view->setCamera(d->camera);

    // Initialize the Item3d objects attached to this scene.
    QObjectList list = QObject::children();
    foreach (QObject *child, list) {
        Item3d *item = qobject_cast<Item3d *>(child);
        if (item)
            item->initialize(this, painter);
    }
    d->itemsInitialized = true;
}

Qml3dView *Viewport::view() const
{
    return d->view;
}

void Viewport::update3d()
{
    if (d->view)
        d->view->queueUpdate();
    else
        update();
}

void Viewport::cameraChanged()
{
    if (d->view)
        d->view->setCamera(d->camera);  // Calls queueUpdate() internally.
    else
        update();
}

QT_END_NAMESPACE
