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

#include "qatlas.h"
#include "qareaallocator_p.h"
#include "qgltexture2d.h"
#include "qglpainter.h"
#include "qgeometrydata.h"
#include "qglmaterial.h"
#include "qglpainter.h"
#include "qglframebufferobjectsurface.h"
#include "qglbuilder.h"

#include <QImage>
#include <QThread>
#include <QGLFramebufferObject>
#include <QGLFramebufferObjectFormat>
#include <QCoreApplication>

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

QAtlas::QAtlas()
    : m_size(1024, 1024)
    , m_data(0)
    , m_renderTarget(0)
    , m_allocator(new QSimpleAreaAllocator(m_size))
    , m_tex(0)
    , m_material(new QGLMaterial)
    , m_initialized(false)
    , m_count(0)
{
}

QAtlas::~QAtlas()
{
    delete m_data;
    delete m_renderTarget;
    delete m_allocator;
}

void QAtlas::initialize(QGLPainter *painter)
{
    Q_UNUSED(painter);
    if (!m_initialized)
    {
        m_data = new QGLFramebufferObject(m_size);
        m_renderTarget = new QGLFramebufferObjectSurface(m_data);
        m_tex = QGLTexture2D::fromTextureId(m_data->texture(), m_size);
        m_material->setTexture(m_tex, 1);
        m_material->setObjectName("Atlas material");
        m_initialized = true;
    }
}

void QAtlas::paint(QGLPainter *painter)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());

    if (m_allocationQueue.isEmpty() || painter->isPicking())
        return;

    painter->pushSurface(m_renderTarget);
    painter->setStandardEffect(QGL::FlatReplaceTexture2D);
    painter->projectionMatrix().push();
    painter->modelViewMatrix().push();
    QRect rect = painter->currentSurface()->viewportRect();
    QMatrix4x4 proj;
    proj.ortho(rect);
    painter->projectionMatrix() = proj;
    painter->modelViewMatrix().setToIdentity();

    painter->update();

    for (int i = 0; i < m_allocationQueue.count(); ++i)
    {
        QAtlasEntry entry = m_allocationQueue.at(i);
        QRect a = entry.rect;
        QImage image = entry.image;

        if (a.left() == 0 && a.top() == 0) // first one - paint fill color
        {
            painter->setClearColor(Qt::red);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        QGLTexture2D t;
        t.setImage(image);
        t.bind();
        QVector3D va(a.left(), a.bottom()+1, 0);
        QVector3D vb(a.right()+1, a.bottom()+1, 0);
        QVector3D vc(a.right()+1, a.top(), 0);
        QVector3D vd(a.left(), a.top(), 0);
        QVector2D ta(0.0f, 0.0f);
        QVector2D tb(1.0f, 0.0f);
        QVector2D tc(1.0f, 1.0f);
        QVector2D td(0.0f, 1.0f);
        QGeometryData quad;
        quad.setBufferStrategy(QGeometryData::KeepClientData);
        quad.appendVertex(va, vb, vc, vd);
        quad.appendTexCoord(ta, tb, tc, td);
        quad.appendIndices(0, 1, 2);
        quad.appendIndices(0, 2, 3);
        quad.draw(painter, 0, 6);
    }
    m_allocationQueue.clear();

    painter->popSurface();
}

QRect QAtlas::allocate(const QSize &size, const QImage &image, const QGL::IndexArray &indices)
{
    Q_ASSERT(QThread::currentThread() == qApp->thread());

    QRect a = m_allocator->allocate(size);
    if (a.isEmpty())
    {
        qWarning("QAtlas::allocate: overflowed");
        return a;
    }

    m_allocationQueue.append(QAtlasEntry(image, a));
    ++m_count;

    QRectF af(a);
    QSizeF szf(m_size);
    float l = af.left() / szf.width();
    float r = af.right() / szf.width();
    float t = 1.0f - (af.top() / szf.height());
    float b = 1.0f - (af.bottom() / szf.height());
    m_geometry.texCoord(indices.at(0), QGL::TextureCoord1) = QVector2D(l, b);
    m_geometry.texCoord(indices.at(1), QGL::TextureCoord1) = QVector2D(r, b);
    m_geometry.texCoord(indices.at(2), QGL::TextureCoord1) = QVector2D(r, t);
    m_geometry.texCoord(indices.at(5), QGL::TextureCoord1) = QVector2D(l, t);
    return a;
}

void QAtlas::release(QRect frame)
{
    m_allocator->release(frame);
}

Q_GLOBAL_STATIC(QAtlas, atlasInstance);

QAtlas *QAtlas::instance()
{
    return atlasInstance();
}
