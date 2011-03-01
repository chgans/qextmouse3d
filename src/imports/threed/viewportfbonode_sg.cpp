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

#include "viewportfbonode_sg.h"
#include "viewport_sg.h"
#include "qglframebufferobjectsurface.h"

QT_BEGIN_NAMESPACE

ViewportFboNodeSG::ViewportFboNodeSG(ViewportSG *viewport, QSGContext *context)
    : m_viewport(viewport)
    , m_geometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4)
    , m_stereoInfo(context)
    , m_useAlpha(true)
    , m_linearFiltering(true)
    , m_leftWasDirty(true)
{
    setFlag(Node::UsePreprocess);
    setMaterial(&m_left.materialO);
    setOpaqueMaterial(&m_left.material);
    setGeometry(&m_geometry);
}

ViewportFboNodeSG::~ViewportFboNodeSG()
{
}

void ViewportFboNodeSG::setSize(const QSize &size)
{
    if (size == m_size)
        return;
    m_size = size;

    m_left.reset();
    m_right.reset();

    QRectF rect(0, 0, m_size.width(), m_size.height());
    QRectF sourceRect(0,1,1,-1);

    QSGGeometry::updateTexturedRectGeometry(&m_geometry, rect, sourceRect);
    markDirty(DirtyGeometry);
}

void ViewportFboNodeSG::setUseAlpha(bool useAlpha)
{
    if (useAlpha == m_useAlpha)
        return;

    m_useAlpha = useAlpha;

    m_left.reset();
    m_right.reset();

    markDirty(DirtyGeometry);
}

void ViewportFboNodeSG::preprocess()
{
    QGL::Eye eye = m_stereoInfo.eye();

    // Which eye buffer should we render into?
    EyeBuffer *buffer;
    if (eye == QGL::RightEye)
        buffer = &m_right;
    else
        buffer = &m_left;

    // If the viewport contents have not changed since the last
    // time we rendered into the fbo, then no point painting again.
    if (eye != QGL::RightEye) {
        m_leftWasDirty = m_viewport->isDirty();
        if (buffer->fbo && !m_viewport->isDirty())
            return;
    } else {
        if (buffer->fbo && !m_leftWasDirty)
            return;
    }

    if (!buffer->fbo) {
        QGLFramebufferObjectFormat format;
        if (!m_useAlpha) {
#ifndef QT_OPENGL_ES
            format.setInternalTextureFormat(GL_RGB8);
#else
            format.setInternalTextureFormat(GL_RGB);
#endif
        }
        format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
        buffer->fbo = new QGLFramebufferObject(m_size, format);

        QSGTexture *texture = new QSGTexture;
        texture->setTextureId(buffer->fbo->texture());
        texture->setTextureSize(buffer->fbo->size());
        texture->setAlphaChannel(m_useAlpha);
        texture->setStatus(QSGTexture::Ready);
        texture->setOwnsTexture(false);
        buffer->texture = QSGTextureRef(texture);

        buffer->material.setTexture(buffer->texture, !m_useAlpha);
        buffer->material.setLinearFiltering(m_linearFiltering);
        buffer->materialO.setTexture(buffer->texture, !m_useAlpha);
        buffer->materialO.setLinearFiltering(m_linearFiltering);
    }
    setMaterial(&(buffer->materialO));
    setOpaqueMaterial(&(buffer->material));

    QGLFramebufferObjectSurface surface(buffer->fbo);
    m_viewport->paint(&surface, eye);
}

QT_END_NAMESPACE
