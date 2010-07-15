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

#include "teapotitem.h"
#include "qglpainter.h"
#include "qglteapot.h"
#include "qglbuilder.h"

#include <QtOpenGL/qglframebufferobject.h>
#include <QtGui/qpainter.h>
#include <QtGui/qgraphicsscene.h>

TeapotItem::TeapotItem(QGraphicsItem *parent)
    : QGLGraphicsViewportItem(parent)
    , mScene(0)
    , fbo(0)
{
    setFlag(ItemIsFocusable, true);

    QGLBuilder builder;
    builder << QGLTeapot();
    teapot = builder.finalizedSceneNode();
    teapot->setParent(this);
}

TeapotItem::~TeapotItem()
{
}

void TeapotItem::setScene(QGraphicsScene *scene)
{
    mScene = scene;
    connect(scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(updateScene()));
}

void TeapotItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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
        fboPainter.setBrush(background);
        fboPainter.drawRect(rect);
        fboPainter.restore();
        mScene->render(&fboPainter, rect);
    }

    // Now render the GL parts of the item using QGLPainter.
    QGLGraphicsViewportItem::paint(painter, option, widget);
}

void TeapotItem::paintGL(QGLPainter *painter)
{
    painter->modelViewMatrix().scale(1.5f);
    if (fbo) {
        painter->setStandardEffect(QGL::LitModulateTexture2D);
        glBindTexture(GL_TEXTURE_2D, fbo->texture());
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (painter->isFixedFunction())
            glEnable(GL_TEXTURE_2D);
        teapot->draw(painter);
        glBindTexture(GL_TEXTURE_2D, 0);
        if (painter->isFixedFunction())
            glDisable(GL_TEXTURE_2D);
    } else {
        painter->setStandardEffect(QGL::LitMaterial);
        teapot->draw(painter);
    }
}

void TeapotItem::updateScene()
{
    update();
}
