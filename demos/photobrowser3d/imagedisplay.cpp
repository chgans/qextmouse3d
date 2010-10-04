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


#include "imagedisplay.h"
#include "thumbnailableimage.h"
#include "thumbnailnode.h"
#include "thumbnaileffect.h"
#include "qglbuilder.h"
#include "qglcube.h"
#include "qframesscene.h"
#include "imagemanager.h"
#include "qatlas.h"
#include "qglshaderprogrameffect.h"

#include <QApplication>
#include <QChildEvent>
#include <QUrl>
#include <QImage>

static inline QImage qMakeFrameImage()
{
    QImage frm(QSize(128, 128), QImage::Format_ARGB32);
    frm.fill(qRgba(8, 8, 8, 255));  // dark grey frame
    QPainter ptr;
    ptr.begin(&frm);
    QRect r(8, 8, 112, 112);
    ptr.setBackgroundMode(Qt::TransparentMode);
    ptr.fillRect(r, QColor(0, 30, 50, 64));
    ptr.setPen(QColor("orange"));
    ptr.drawText(frm.rect(), Qt::AlignCenter, "Loading...");
    ptr.end();
    return frm;
}

static inline void qAddPane(QSizeF size, QGeometryData *data)
{
    Q_ASSERT(data);
    QSizeF f = size / 2.0f;
    QVector2D a(-f.width(), -f.height());
    QVector2D b(f.width(), -f.height());
    QVector2D c(f.width(), f.height());
    QVector2D d(-f.width(), f.height());
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    QVector2D td(0.0f, 1.0f);
    int k = data->count();
    data->appendVertex(a, b, c, d);
    data->appendTexCoord(ta, tb, tc, td);
    data->appendIndices(k, k+1, k+2);
    data->appendIndices(k, k+2, k+3);
}

ImageDisplay::ImageDisplay(QObject *parent, QGLMaterialCollection *materials, qreal wallSize)
    : QGLSceneNode(parent)
    , m_wall(0)
    , m_frames(0)
    , m_currentWall(0)
    , m_currentFrame(0)
    , m_imageSetToDefault(false)
    , m_count(0)
    , m_size(wallSize)
    , m_frameSize((m_size * 3.0f) / 4.0f)
    , m_maxImages(500)
    , m_frameLoadingMaterial(-1)
{
    qDebug() << ">>>>>>> ImageDisplay::ImageDisplay" << QThread::currentThread();

    // the real values will get poked in here by the atlas
    m_atlasPlaceHolder.append(QVector2D(), QVector2D(), QVector2D(), QVector2D());

    setObjectName("ImageDisplay");
    setPalette(materials);

    // build the wall
    qAddPane(QSize(m_size, m_size), &m_wallGeometry);
    m_wall = new QGLSceneNode(this);
    m_wall->setObjectName("Wall");
    m_wall->setPalette(materials);
    m_currentWall = new QGLSceneNode(m_wall);
    m_currentWall->setObjectName("wall 0");
    m_currentWall->setGeometry(m_wallGeometry);
    m_currentWall->setCount(m_wallGeometry.indexCount());
    m_wall->setPosition(QVector3D(0.0f, 0.0f, m_size / -4.0));

    // paint the wall
    m_wall->setEffect(QGL::FlatReplaceTexture2D);
    QGLMaterial *mat = new QGLMaterial();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    qDebug() << "created girder texture" << tex << QThread::currentThread();
    tex->setImage(QImage(":/res/images/girder.png"));
    mat->setTexture(tex);
    m_wall->setMaterial(mat);

    // build the frames
    qAddPane(QSize(m_frameSize, m_frameSize), &m_frameGeometry);
    m_frameGeometry.appendTexCoordArray(m_atlasPlaceHolder, QGL::TextureCoord1);
    m_frames = new QGLSceneNode(this);
    m_frames->setObjectName("Frames");
    m_currentFrame = new ThumbnailNode(m_frames);
    m_currentFrame->setObjectName("frame 0");
    m_currentFrame->setGeometry(m_frameGeometry);
    m_currentFrame->setCount(m_frameGeometry.indexCount());

    // use the frames geometry to put the atlas data into
    QAtlas *atlas = QAtlas::instance();
    atlas->setGeometry(m_frameGeometry);

    // generally the frames use the thumbnail material & effect
    m_effect = new ThumbnailEffect;
    qDebug() << "created thumbnail effect:" << m_effect;
    m_frames->setUserEffect(m_effect);
    m_frames->setEffectEnabled(true);
    //m_frames->setEffect(QGL::FlatDecalTexture2D);
    m_frames->setMaterial(atlas->material());

    // unless they're loading, in which case use the "loading" image
    m_frameImage = qMakeFrameImage();
    mat = new QGLMaterial();
    tex = new QGLTexture2D(mat);
    qDebug() << "created frame texture" << tex << QThread::currentThread();
    tex->setHorizontalWrap(QGL::Clamp);
    tex->setImage(m_frameImage);
    mat->setTexture(tex);
    m_frameLoadingMaterial = materials->addMaterial(mat);
    m_currentFrame->setMaterialIndex(m_frameLoadingMaterial);

    // make the frames pickable
    m_frameScene = new QFramesScene(this);
    m_frameScene->setRootNode(m_frames);
    m_frameScene->setPickable(true);

    m_imageSetToDefault = true;

    qDebug() << "<<<<<<< ImageDisplay::ImageDisplay" << QThread::currentThread();
}

ImageDisplay::~ImageDisplay()
{
    qDebug() << ">>> ImageDisplay::~ImageDisplay()";
    delete m_effect;
    qDebug() << "<<< ImageDisplay::~ImageDisplay()";
}

void ImageDisplay::addThumbnailNode(const QUrl &image)
{
    qDebug() << ">>>>>>> ImageDisplay::addThumbnailNode" << image.toString() << " #" << m_count << "in thread:" << QThread::currentThread();
    ImageManager *manager = qobject_cast<ImageManager*>(sender());
    if (!m_imageSetToDefault)
    {
        QVector3D p = m_currentFrame->position();
        p.setX(p.x() - m_size);
        int start = m_frameGeometry.indexCount();
        qAddPane(QSize(m_frameSize, m_frameSize), &m_frameGeometry);
        int count = m_frameGeometry.indexCount() - start;
        m_frameGeometry.appendTexCoordArray(m_atlasPlaceHolder, QGL::TextureCoord1);
        m_currentFrame = new ThumbnailNode(m_frames);
        m_currentFrame->setObjectName(QString("frame %1").arg(m_count));
        m_currentFrame->setPosition(p);
        m_currentFrame->setStart(start);
        m_currentFrame->setCount(count);
        m_currentFrame->setGeometry(m_frameGeometry);
        m_currentFrame->setMaterialIndex(m_frameLoadingMaterial);

        // tell the frame scene to make the child pickable
        QChildEvent e(QEvent::ChildAdded, m_currentFrame);
        QApplication::sendEvent(m_frameScene, &e);

        QGLSceneNode *s = m_currentWall->clone(m_wall);
        s->setObjectName(QString("wall %1").arg(m_count));
        p = s->position();
        p.setX(p.x() - m_size);
        s->setPosition(p);
        m_currentWall = s;
    }
    m_currentFrame->setUrl(image);
    if (manager)
    {
        connect(m_currentFrame, SIGNAL(imageRequired(QUrl)), manager, SIGNAL(deployLoader(QUrl)));
        connect(manager, SIGNAL(imageReady(ThumbnailableImage)), m_currentFrame, SLOT(setImage(ThumbnailableImage)));
    }
    m_imageSetToDefault = false;
    emit framesChanged();
    //if (m_count == 20)
    //    qDumpScene(this);
    ++m_count;
    qDebug() << "<<<<<<< ImageDisplay::addThumbnailNode" << image.toString() << QThread::currentThread();
}

QList<QGLPickNode *> ImageDisplay::pickNodes() const
{
    return m_frameScene->pickNodes();
}
