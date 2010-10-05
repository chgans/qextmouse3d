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
#include "qglpicknode.h"

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
        m_loading = true;
#ifdef QT_NO_THREADED_FILE_LOAD
        ThumbnailableImage image;
        image.setUrl(m_url);
        QImage im(m_url.toLocalFile());
        if (im.isNull())
            qDebug() << "ThumbnailNode::setupLoading: could not load image:"
                     << m_url.toLocalFile();
        if (im.size().width() > 1024 || im.size().height() > 768)
            im = im.scaled(QSize(1024, 768), Qt::KeepAspectRatio,
                           Qt::SmoothTransformation);
        image.setData(im);
        setImage(image);
#else
        emit imageRequired(m_url);
#endif
        if (m_url.toString().contains("1"))
            qDebug() << "ThumbnailNode::setupLoading" << m_url;
    }
}

void ThumbnailNode::createFullNode()
{
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
    QGLMaterial *mat = m_full->material();
    if (m_full->materialIndex() != m_defaultMaterial)
        m_full->palette()->removeMaterial(mat);
    delete m_full;
    m_full = 0;
}

void ThumbnailNode::loadFullImage()
{
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
            mat->setTexture(tex);
            int ix = palette()->addMaterial(mat);
            m_full->setMaterialIndex(ix);
            mat->setParent(m_full);
        }
    }
}

void ThumbnailNode::geometryDraw(QGLPainter *painter)
{
    QGLSceneNode::geometryDraw(painter);
}

void ThumbnailNode::draw(QGLPainter *painter)
{
    if (m_defaultMaterial == -1)
        m_defaultMaterial = materialIndex();

    QMatrix4x4 m = painter->modelViewMatrix().top();
    QVector3D pos = m.map(position());
    qreal magSquared = pos.lengthSquared();

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
        setupLoading();
        loadFullImage();
        nodeToDraw = m_full;
        break;
    case Middle:
        if (m_full)
            nodeToDraw = m_full;
        setupLoading();
        loadFullImage();
        break;
    case Far:
        destroyFullNode();
        if (m_image.isMinimized() && !m_loading)
        {
            m_image = ThumbnailableImage();
            setupLoading();
        }
        if (!m_image.data().isNull())
            m_image.setThumbnailed(true);
        break;
    case VeryFar:
        destroyFullNode();
        m_image.minimize();
        break;
    }

    QGLSceneNode *p = qobject_cast<QGLSceneNode*>(parent());
    //Q_ASSERT_X(p && p->userEffect() && (!hasEffect()),
    //           "ThumbnailNode::draw", "Should only inherit parents ThumbnailEffect");
    ThumbnailEffect *effect = static_cast<ThumbnailEffect*>(p->userEffect());
    //Q_ASSERT_X(effect && effect->name() == QLatin1String("ThumbnailEffect"),
    //           "ThumbnailNode::draw", "Can only be drawn with custom ThumbnailEffect");

    if (nodeToDraw == this)
    {
        effect->setThumbnail(m_image.isThumbnailed());
        QGLSceneNode::draw(painter);
    }
    else
    {
        effect->setThumbnail(false);
        if (pickNode() && painter->isPicking())
            painter->setObjectPickId(pickNode()->id());
        nodeToDraw->draw(painter);
    }

}

void ThumbnailNode::setImage(const ThumbnailableImage &image)
{
    Q_ASSERT(!image.isNull());

    if (m_url.toString().contains("basket-screenshot.jpg"))
    {
        qDebug() << "ThumbnailNode::setImage"
                 << m_url << "-- got:" << image.url()
                 << "loading? --" << m_loading
                 << "thread:" << QThread::currentThread();
        ::fprintf(stderr, "     ThumbnailNode::setImage -- image data: %p -- thread: %p\n", image.priv(),
                  QThread::currentThread());
    }

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
    // this makes a photo of 1024 x 768 display on approx 3.0 x 2.8 pane
    // add salt to taste
    QSizeF f = QSizeF(m_image.data().size()) / 600.0f;
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
