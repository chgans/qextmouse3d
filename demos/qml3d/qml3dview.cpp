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

#include "qml3dview.h"
#include "item3d.h"
#include "viewport.h"
#include <QtCore/qtimer.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlcontext.h>

QT_BEGIN_NAMESPACE

Qml3dView::Qml3dView(QWidget *parent)
    : QGLView(parent)
{
    m_viewport = 0;
    pickId = 0;
    initGLCalled = false;
    setOption(QGLView::ObjectPicking, true);
}

Qml3dView::~Qml3dView()
{
}

void Qml3dView::updateGL()
{
    QGLView::updateGL();
}

void Qml3dView::initializeGL(QGLPainter *painter)
{
    if (m_viewport)
        m_viewport->initialize(this, painter);
    initGLCalled = true;
}

void Qml3dView::earlyPaintGL(QGLPainter *painter)
{
    if (m_viewport)
        m_viewport->earlyDraw(painter);
    else
        QGLView::earlyPaintGL(painter);
}

void Qml3dView::paintGL(QGLPainter *painter)
{
    if (m_viewport)
        m_viewport->draw(painter);
}

void Qml3dView::setUrl(const QUrl& url)
{
    // Load the .qml file into a sub-context.
    component = new QmlComponent(&engine, url, this);

    if (!component->isLoading()) {
        loaded();
    } else {
        connect(component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(loaded()));
    }
}

void Qml3dView::loaded()
{
    disconnect(component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(loaded()));

    if (component->isError()) {
        QList<QmlError> errors = component->errors();
        foreach (const QmlError &error, errors) {
            qWarning() << error;
        }
    }

    QmlContext *ctx = new QmlContext(&engine);
    QObject *mainObject = component->create(ctx);

    Viewport *viewport = qobject_cast<Viewport *>(mainObject);
    Item3d *item = qobject_cast<Item3d *>(mainObject);

    if (viewport) {
        int width = viewport->width();
        if (width > 0)
            setMinimumWidth(width);
        int height = viewport->height();
        if (height > 0)
            setMinimumHeight(height);
        setOption(QGLView::ObjectPicking, viewport->picking());
        setOption(QGLView::ShowPicking, viewport->showPicking());
        setOption(QGLView::CameraNavigation, viewport->navigation());
        setViewport(viewport);

        QGLPainter painter(this);
        viewport->initialize(this, &painter);

        QTimer::singleShot(0, this, SLOT(updateGL()));
    } else if (item) {
        qWarning() << "qml3d: Item3d object without an enclosing Scene";
    } else if (qobject_cast<QmlGraphicsItem *>(mainObject)) {
        qWarning() << "qml3d: Ordinary QmlGraphicsItem node found; may be "
                      "missing '-graphicssystem OpenGL'";
    } else {
        qWarning() << "qml3d: No Scene or Item3d node found";
    }

    QGLView::initializeGL();
}

QT_END_NAMESPACE
