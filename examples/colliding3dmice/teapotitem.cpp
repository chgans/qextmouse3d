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
