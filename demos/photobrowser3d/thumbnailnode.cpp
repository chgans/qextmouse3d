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
#include "qglrendersequencer.h"

#include <QtGui/qmatrix4x4.h>

ThumbnailNode::ThumbnailNode(QObject *parent)
    : QGLSceneNode(parent)
    , m_thresholdSquared(20.0f * 20.0f)
    , m_defaultMaterial(-1)
    , m_loading(false)
    , m_full(0)
{
}

ThumbnailNode::~ThumbnailNode()
{
    delete m_full;
}

void ThumbnailNode::setupLoading()
{
    if (m_image.isNull() && !m_loading && !m_url.isEmpty())
    {
        qDebug() << "ThumbnailNode::setupLoading" << m_url;
        m_loading = true;
#ifdef QT_USE_TEST_IMAGES
        ThumbnailableImage image;
        image.setUrl(m_url);
        QImage im(m_url.toLocalFile());
        if (im.isNull())
            qDebug() << "############# -- could not load test image:"
                        << m_url.toLocalFile() << "-- #################";
        image.setData(im);
        setImage(image);
#else
        emit imageRequired(m_url);
#endif
    }
}

void ThumbnailNode::createFullNode()
{
    qDebug() << "ThumbnailNode::createFullNode" << m_url;
    qDebug() << *this;
    m_full = new QGLSceneNode;
    m_full->setPosition(position());
    m_full->setGeometry(geometry());
    m_full->setStart(start());
    m_full->setCount(count());
    m_full->setPalette(palette());
    m_full->setMaterialIndex(m_defaultMaterial);
}

void ThumbnailNode::destroyFullNode()
{
    if (!m_full)
        return;
    qDebug() << "ThumbnailNode::destroyFullNode" << m_url << "full node:" << m_full;
    QGLMaterial *mat = m_full->material();
    qDebug() << "     material:" << mat;
    if (m_full->materialIndex() != m_defaultMaterial)
        m_full->palette()->removeMaterial(mat);
    delete m_full;
    m_full = 0;
}

void ThumbnailNode::loadFullImage()
{
    qDebug() << "ThumbnailNode::loadFullImage" << m_url;
    if (!m_full)
        createFullNode();
    Q_CHECK_PTR(m_full);
    if (!m_image.data().isNull())
    {
        m_image.setThumbnailed(true);
        if (m_full->materialIndex() == m_defaultMaterial)
        {
            QGLMaterial *mat = new QGLMaterial;
            QGLTexture2D *tex = new QGLTexture2D;
            tex->setImage(m_image.data());
            qDebug() << "ThumbnailNode::draw" << m_url << "created tex:" << tex
                    << "setting image on new material:" << mat
                    << (m_image.isNull() ?
                        QString("NULL") : m_image.url().toString());
            mat->setTexture(tex);
            int ix = palette()->addMaterial(mat);
            m_full->setMaterialIndex(ix);
            mat->setParent(m_full);
        }
    }
}

void ThumbnailNode::draw(QGLPainter *painter)
{
    // save the material we were given at creation time.  This is the
    // default material to use when loading, has "Loading..." message.
    if (m_defaultMaterial == -1)
        m_defaultMaterial = materialIndex();

    // under the model-view transformation the eye of the camera is located
    // at the origin, so mapping the position of this node will make it into
    // a vector from the origin/camera - taking the magnitude of this gives
    // the distance from the eye to the node.  This only works because we
    // know that the parent nodes have not applied any transformations.
    QMatrix4x4 m = painter->modelViewMatrix().top();
    QVector3D pos = m.map(position());
    qreal magSquared = pos.lengthSquared();
    qDebug() << "ThumbnailNode::draw -- magSquared" << magSquared << "-- thresholdSquared" << m_thresholdSquared
                << m_url;

    Distance distance = Near;
    if (magSquared > (4.0f * m_thresholdSquared))
        distance = VeryFar;
    else if (magSquared > (2.0f * m_thresholdSquared))
        distance = Far;
    else if (magSquared > m_thresholdSquared)
        distance = Middle;
    else
        distance = Near;

    QGLSceneNode *nodeToDraw = this;
    switch (distance)
    {
    case Near:
        qDebug() << "ThumbnailNode::draw:  NEAR" << m_url;
        setupLoading();
        loadFullImage();
        nodeToDraw = m_full;
        break;
    case Middle:
        qDebug() << "ThumbnailNode::draw:  MIDDLE" << m_url;
        if (m_full)
            nodeToDraw = m_full;
        setupLoading();
        loadFullImage();
        break;
    case Far:
        qDebug() << "ThumbnailNode::draw:  FAR" << m_url;
        destroyFullNode();
        if (m_image.isMinimized())
        {
            m_image = ThumbnailableImage();
            setupLoading();
        }
        if (!m_image.data().isNull())
            m_image.setThumbnailed(true);
        break;
    case VeryFar:
        qDebug() << "ThumbnailNode::draw:  VERYFAR" << m_url;
        destroyFullNode();
        m_image.minimize();
        break;
    }

    QGLSceneNode *p = qobject_cast<QGLSceneNode*>(parent());
    Q_ASSERT_X(p && p->userEffect() && (!hasEffect()),
               "ThumbnailNode::draw", "Should only inherit parents ThumbnailEffect");
    ThumbnailEffect *effect = static_cast<ThumbnailEffect*>(p->userEffect());
    Q_ASSERT_X(effect && effect->name() == QLatin1String("ThumbnailEffect"),
               "ThumbnailNode::draw", "Can only be drawn with custom ThumbnailEffect");

    if (nodeToDraw == this)
    {
        qDebug() << "drawing thumbnail - thumbnailed:" << m_image.isThumbnailed() << this;
        effect->setThumbnail(m_image.isThumbnailed());
        QGLSceneNode::draw(painter);
    }
    else
    {
        qDebug() << "drawing full image - thumbnailed:" << m_image.isThumbnailed() << this;
        effect->setThumbnail(false);
        nodeToDraw->draw(painter);
    }

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
    if (sender())
        disconnect(sender());

    // the indices we are about to set will index this thumbnail image
    // into the image that its atlas is based on via the texture coords
    // that the atlas is using - those texture coords must be in the
    // same geometry that this node is referencing, so that they will
    // arrive at the vertex shader at the same time - ie they are all
    // matched in the data arrays in the geometry object
    Q_ASSERT(QAtlas::instance()->geometry() == geometry());

    m_image = image;
    Q_ASSERT(!m_image.data().isNull());

    // debug - delete me
    ::fprintf(stderr, "     ThumbnailNode::setImage -- m_image data: %p -- thread: %p\n",
              m_image.priv(),
              QThread::currentThread());

    // configure the placeholder for the actual image size
    // this makes a photo of 1024 x 768 display on approx 2.0 x 1.5 pane
    // add salt to taste
    QSizeF f = QSizeF(m_image.data().size()) / 1000.0f;
    QVector3D a(-f.width(), -f.height(), 0.0f);
    QVector3D b(f.width(), -f.height(), 0.0f);
    QVector3D c(f.width(), f.height(), 0.0f);
    QVector3D d(-f.width(), f.height(), 0.0f);
    int k = start();
    Q_ASSERT(count() == 6);
    QGeometryData g = geometry();
    QGL::IndexArray inxs = g.indices();
    g.vertex(inxs.at(k)) = a;
    g.vertex(inxs.at(k+1)) = b;
    g.vertex(inxs.at(k+2)) = c;
    g.vertex(inxs.at(k+5)) = d;

    m_image.setIndices(inxs.mid(start(), count()));
    setMaterialIndex(-1);
    m_loading = false;
}
