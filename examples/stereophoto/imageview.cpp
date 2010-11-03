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

#include "imageview.h"

ImageView::ImageView(QWidget *parent)
    : QGLView(parent)
    , m_leftTexture(0)
    , m_rightTexture(0)
{
    setOption(CameraNavigation, false);
}

void ImageView::load(const QString &fileName)
{
    delete m_leftTexture;
    delete m_rightTexture;
    m_leftTexture = 0;
    m_rightTexture = 0;

    if (!m_image.load(fileName))
        qWarning() << "Could not load" << fileName;
    else
        update();
}

void ImageView::paintGL(QGLPainter *painter)
{
    if (m_image.isNull())
        return;

    painter->projectionMatrix().setToIdentity();
    painter->modelViewMatrix().setToIdentity();

    painter->setStandardEffect(QGL::FlatReplaceTexture2D);

    if (painter->eye() == QGL::LeftEye || painter->eye() == QGL::NoEye) {
        if (!m_leftTexture) {
            m_leftTexture = new QGLTexture2D(this);
            m_leftTexture->setImage(m_image.leftImage());
        }
        m_leftTexture->bind();
    } else {
        if (!m_rightTexture) {
            m_rightTexture = new QGLTexture2D(this);
            m_rightTexture->setImage(m_image.rightImage());
        }
        m_rightTexture->bind();
    }

    QSize viewSize = painter->currentSurface()->viewportRect().size();
    QSize imageSize = m_image.size();
    imageSize.scale(viewSize, Qt::KeepAspectRatio);

    qreal x = qreal(imageSize.width()) / qreal(viewSize.width());
    qreal y = qreal(imageSize.height()) / qreal(viewSize.height());

    QVector2DArray vertices;
    QVector2DArray texCoords;
    vertices.append(-x, y);
    vertices.append(-x, -y);
    vertices.append(x, -y);
    vertices.append(x, y);
    texCoords.append(0.0f, 1.0f);
    texCoords.append(0.0f, 0.0f);
    texCoords.append(1.0f, 0.0f);
    texCoords.append(1.0f, 1.0f);

    painter->clearAttributes();
    painter->setVertexAttribute(QGL::Position, vertices);
    painter->setVertexAttribute(QGL::TextureCoord0, texCoords);

    painter->draw(QGL::TriangleFan, 4);
}
