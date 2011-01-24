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
#include "qsgstereocontext.h"
#include "qglframebufferobjectsurface.h"

QT_BEGIN_NAMESPACE

ViewportFboNodeSG::ViewportFboNodeSG(ViewportSG *viewport, QSGContext *context)
    : m_viewport(viewport)
    , m_context(qobject_cast<QSGStereoContext *>(context))
    , m_opacity(1.0f)
    , m_fbo(0)
    , m_useAlpha(true)
    , m_linearFiltering(true)
{
    setFlag(Node::UsePreprocess);
    setMaterial(&m_material);
    updateGeometryDescription(Utilities::getTexturedRectGeometryDescription(), GL_UNSIGNED_SHORT);
}

ViewportFboNodeSG::~ViewportFboNodeSG()
{
    delete m_fbo;
}

void ViewportFboNodeSG::setSize(const QSize &size)
{
    if (size == m_size)
        return;

    m_size = size;
    setBoundingRect(QRectF(0, 0, size.width(), size.height()));

    delete m_fbo;
    m_fbo = 0;

    Utilities::setupRectGeometry(geometry(), QRectF(0, 0, m_size.width(), m_size.height()), QSize(1, 1), QRectF(0,1,1,-1));
    markDirty(DirtyGeometry);
}

void ViewportFboNodeSG::setOpacity(qreal opacity)
{
    if (opacity == m_opacity)
        return;

    m_opacity = opacity;
    m_materialO.setOpacity(opacity);
    setMaterial(opacity == 1 ? &m_material : &m_materialO);
}

void ViewportFboNodeSG::setUseAlpha(bool useAlpha)
{
    if (useAlpha == m_useAlpha)
        return;

    m_useAlpha = useAlpha;

    delete m_fbo;
    m_fbo = 0;
}

void ViewportFboNodeSG::preprocess()
{
    QSGStereoContext::Eye eye;
    eye = m_context ? m_context->eye() : QSGStereoContext::NoEye;

    // If the viewport contents have not changed since the last
    // time we rendered into the fbo, then no point painting again.
    // In stereo mode, the viewport contents are assumed to always
    // change: left eye, right eye, left eye, right eye, ...
    // May want to experiment later with using an fbo for each eye.
    if (m_fbo && !m_viewport->isDirty() && eye == QSGStereoContext::NoEye)
        return;

    if (!m_fbo) {
        QGLFramebufferObjectFormat format;
        if (!m_useAlpha) {
#ifndef QT_OPENGL_ES
            format.setInternalTextureFormat(GL_RGB8);
#else
            format.setInternalTextureFormat(GL_RGB);
#endif
        }
        format.setAttachment(QGLFramebufferObject::CombinedDepthStencil);
        m_fbo = new QGLFramebufferObject(m_size, format);

        QSGTexture *texture = new QSGTexture;
        texture->setTextureId(m_fbo->texture());
        texture->setTextureSize(m_fbo->size());
        texture->setAlphaChannel(m_useAlpha);
        texture->setStatus(QSGTexture::Ready);
        texture->setOwnsTexture(false);
        m_texture = QSGTextureRef(texture);

        m_material.setTexture(m_texture, !m_useAlpha);
        m_material.setLinearFiltering(m_linearFiltering);
        m_materialO.setTexture(m_texture, !m_useAlpha);
        m_materialO.setLinearFiltering(m_linearFiltering);
        m_materialO.setOpacity(m_opacity);
        setMaterial(m_opacity == 1 ? &m_material : &m_materialO);
    }

    QGLFramebufferObjectSurface surface(m_fbo);
    m_viewport->paint(&surface, QGL::Eye(eye));
}

QT_END_NAMESPACE
