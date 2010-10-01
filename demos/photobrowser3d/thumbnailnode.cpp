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

#include "thumbnailnode.h"
#include "qglpainter.h"
#include "thumbnaileffect.h"
#include "imagemanager.h"
#include "qatlas.h"

#include <QtGui/qmatrix4x4.h>

ThumbnailNode::ThumbnailNode(QObject *parent)
    : QGLSceneNode(parent)
    , m_thresholdSquared(20.0f * 20.0f)
    , m_defaultMaterial(-1)
    , m_loading(false)
    , m_geometry(0)
    , m_fullImageMaterial(0)
{
}

void ThumbnailNode::setupLoading(QGLPainter *painter)
{
    if (!m_loading && !m_url.isEmpty())
    {
        qDebug() << "ThumbnailNode::setupLoading" << m_url;
        m_loading = true;
        emit imageRequired(m_url);
        setMaterialIndex(m_defaultMaterial);
    }
    thumbGeometry()->draw(painter, start(), count());
}

void ThumbnailNode::draw(QGLPainter *painter)
{
    ThumbnailEffect *effect = static_cast<ThumbnailEffect*>(painter->effect());
    Q_ASSERT_X(effect, "ThumbnailNode::draw", "Can only be drawn with custom effect");

    if (m_image.isNull())
        qDebug() << ">>>> ThumbnailNode::draw - null image" << m_url;
    else
    {
        ::fprintf(stderr, "     ThumbnailNode::draw -- m_image data: %p -- thread: %p\n",
                  m_image.priv(),
                  QThread::currentThread());
        qDebug() << ">>>> ThumbnailNode::draw" << m_image.url() << m_url;
    }

    if (m_defaultMaterial == -1)
        m_defaultMaterial = materialIndex();

    QMatrix4x4 m = painter->modelViewMatrix().top();
    QVector3D pos = m.map(position());

    // we know that the only transforms on these nodes are translation so
    // take some shortcuts with finding cullability
    painter->modelViewMatrix().push();
    painter->modelViewMatrix().translate(position());

    if (m_boundingBox.isNull())
    {
        QGL::IndexArray inxs = thumbGeometry()->indices();
        for (int i = start(); i < start() + count(); ++i)
            m_boundingBox.unite(thumbGeometry()->vertexAt(inxs.at(i)));
    }
    if (painter->isCullable(m_boundingBox))
    {
        painter->modelViewMatrix().pop();
        qDebug() << "<<<< position:" << position() << "culled";
        return;
    }

    //qDebug() << "position()" << position() << "-- mapped:" << pos << m_url;

    // under the model-view transformation the eye of the camera is located
    // at the origin, so mapping the position of this node will make it into
    // a vector from the origin/camera - taking the magnitude of this gives
    // the distance from the eye to the node.
    qreal magSquared = pos.lengthSquared();
    qDebug() << "      magSquared" << magSquared << "-- thresholdSquared" << m_thresholdSquared
                << m_url;

    if (magSquared > (4.0f * m_thresholdSquared))
    {
        // long way from camera - throw away image (if it isn't already)
        // and just draw the default frame - it will be just a dot in the
        // distance anyway
        effect->setThumbnail(false);
        setMaterialIndex(m_defaultMaterial);
        m_image = ThumbnailableImage();
        m_loading = false;
        thumbGeometry()->draw(painter, start(), count());
        painter->modelViewMatrix().pop();
        qDebug() << "<<<< ThumbnailNode::draw -- 4x threshold" << m_url;
        return;
    }

    if (m_image.isNull() && !m_url.isEmpty())
    {
        // still waiting for load
        setupLoading(painter);
        painter->modelViewMatrix().pop();
        qDebug() << "<<<< ThumbnailNode::draw -- null image" << m_url << "loading? " << m_loading;
        return;
    }

    // asynchronous call
    if (magSquared > m_thresholdSquared)
    {
        m_image.setThumbnailed(true);
        if (magSquared > (2.0f * m_thresholdSquared))
        {
            qDebug() << "    ThumbnailNode::draw -- over DOUBLE threshold" << m_url;
            m_image.minimize();
            delete m_fullImageMaterial->texture();
            delete m_fullImageMaterial;
            m_fullImageMaterial = 0;
        }
        else if (magSquared < (1.4f * m_thresholdSquared) && m_image.isMinimized())
        {
            qDebug() << "    ThumbnailNode::draw -- over *1.4* threshold & minimized" << m_url;
            // thrown away full-size image, force reload
            m_image = ThumbnailableImage();
            setupLoading(painter);
            painter->modelViewMatrix().pop();
            return;
        }
    }

    effect->setThumbnail(m_image.isThumbnailed());
    if (m_image.isThumbnailed())
    {
        // dont need to set material - it is set on the parent node in the
        // ImageDisplay class when the first ThumbnailNode is created
        thumbGeometry()->draw(painter, start(), count());
        qDebug() << "      TTTTTTTTTTTTTTTT drawing thumbgeometry" << m_url;
    }
    else
    {
        bool just_set = false;  // debug
        if (!m_image.isNull())
        {
            if (m_fullImageMaterial == 0)
            {
                m_fullImageMaterial = new QGLMaterial(this);
                QGLTexture2D *tex = new QGLTexture2D;
                tex->setImage(m_image.data());
                qDebug() << "ThumbnailNode::draw" << m_url << "created tex:" << tex
                         << "setting image"
                         << (m_image.isNull() ?
                                 QString("NULL") : m_image.url().toString());

                m_fullImageMaterial->setTexture(tex);
                just_set = true; // debug
            }
            setMaterial(m_fullImageMaterial);
        }
        static bool debugged = false;
        if (!debugged && just_set)
        {
            qDumpScene(this);
            debugged = true;
        }
        qDebug() << "FFFFFFFFFFFFFF drawing full material" << m_url;
        thumbGeometry()->draw(painter, start(), count());
    }

    painter->modelViewMatrix().pop();

}

void ThumbnailNode::setImage(const ThumbnailableImage &image)
{
    Q_ASSERT(!image.isNull());

    qDebug() << "ThumbnailNode::setImage"
                << m_url << "-- got:" << image.url()
                   << "loading? --" << m_loading
                      << "thread:" << QThread::currentThread();
    ::fprintf(stderr, "     ThumbnailNode::setImage -- image data: %p -- thread: %p\n", image.priv(),
              QThread::currentThread());

    // ok maybe we got what we asked for but in the meantime we decided
    // we didnt want it anymore
    if (!m_loading)
        return;

    // the manager will be (potentially) loading a number of images, but
    // we only want our one, so just check this is our order
    if (m_url != image.url())
        return;

    // ok we got the right one, stop listening to the manager
    disconnect(sender());

    ImageManager *manager = qobject_cast<ImageManager*>(sender());
    Q_ASSERT(manager);

    // the indices we are about to set will index this thumbnail image
    // into the image that its atlas is based on via the texture coords
    // that the atlas is using - those texture coords must be in the
    // same geometry that this node is referencing, so that they will
    // arrive at the vertex shader at the same time - ie they are all
    // matched in the data arrays in the geometry object

    m_image = image;
    m_image.setAtlas(manager->atlas());

    ::fprintf(stderr, "     ThumbnailNode::setImage -- m_image data: %p -- thread: %p\n",
              m_image.priv(),
              QThread::currentThread());

    QGeometryData *g = thumbGeometry();
    QGL::IndexArray inxs = g->indices();
    Q_ASSERT(m_image.atlas()->geometry() == g);

    QImage im = m_image.data();
    Q_ASSERT(!im.isNull());

    // this makes a photo of 1024 x 768 display on a 2.0 x 1.5 pane
    // add salt to taste
    QSizeF f = QSizeF(im.size()) / 1000.0f;
    QVector3D a(-f.width(), -f.height(), 0.0f);
    QVector3D b(f.width(), -f.height(), 0.0f);
    QVector3D c(f.width(), f.height(), 0.0f);
    QVector3D d(-f.width(), f.height(), 0.0f);
    int k = start();
    g->vertex(inxs.at(k)) = a;
    g->vertex(inxs.at(k+1)) = b;
    g->vertex(inxs.at(k+2)) = c;
    g->vertex(inxs.at(k+3)) = d;

    m_image.setIndices(inxs.mid(start(), count()));
    m_loading = false;
}
