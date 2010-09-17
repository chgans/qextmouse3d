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

#include <QApplication>
#include <QChildEvent>
#include <QUrl>
#include <QImage>

static QImage makeFrameImage()
{
    QImage frm(QSize(128, 128), QImage::Format_ARGB32);
    frm.fill(qRgba(8, 8, 8, 255));  // dark grey frame
    QPainter ptr;
    ptr.begin(&frm);
    QRect r(8, 8, 112, 112);
    ptr.setBackgroundMode(Qt::TransparentMode);
    ptr.fillRect(r, QColor(0, 30, 50, 64));
    ptr.setPen(QColor("orange"));
    ptr.drawText(frm.rect(), "Loading...", Qt::AlignCenter);
    ptr.end();
    return frm;
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
{
    QGLBuilder builder(materials);
    setObjectName("ImageDisplay");

    // build the wall
    m_wall = builder.currentNode();
    m_wall->setObjectName("Wall");
    builder.pushNode();
    m_currentWall = builder.currentNode();
    m_currentWall->setObjectName("wall 0");
    builder.addPane(m_size);
    builder.popNode();
    m_wall->setPosition(QVector3D(0.0f, 0.0f, m_size / -4.0));

    // build the frames
    m_frames = builder.currentNode();
    m_frames->setObjectName("Frames");
    builder.pushNode();
    m_currentFrame = builder.newNode();
    m_currentFrame->setObjectName("frame 0");
    builder.addPane(m_frameSize);
    builder.popNode();

    QGLSceneNode *top = builder.finalizedSceneNode();
    top->setParent(this);

    // paint the wall
    m_wall->setEffect(QGL::FlatReplaceTexture2D);
    QGLMaterial *mat = new QGLMaterial();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(QImage(":/res/images/girder.png"));
    mat->setTexture(tex);
    m_wall->setMaterial(mat);

    // paint the frames
    m_effect = new ThumbnailEffect;
    m_frames->setUserEffect(m_effect);
    m_frames->setEffectEnabled(true);
    m_frameImage = makeFrameImage();
    mat = new QGLMaterial();
    tex = new QGLTexture2D(mat);
    tex->setImage();
    mat->setTexture(tex);
    m_currentFrame->setMaterial(mat);

    // make the frames pickable
    m_frameScene = new QFramesScene(this);
    m_frameScene->setRootNode(m_frames);
    m_frameScene->setPickable(true);

    qDebug() << *m_currentFrame;

    m_imageSetToDefault = true;
}

ImageDisplay::~ImageDisplay()
{
    delete m_effect;
}

void ImageDisplay::addThumbnailNode(const QUrl &image)
{
    ImageManager *manager = qobject_cast<ImageManager*>(sender());
    Q_ASSERT(manager);

    ThumbnailNode *thumb = new ThumbnailNode(m_frames);
    connect(thumb, SIGNAL(imageRequired(QUrl)), manager,
            SLOT(createLoader(QUrl)));
    connect(manager, SIGNAL(imageReady(ThumbnailableImage)),
            thumb, SLOT(setImage(ThumbnailableImage)));
    thumb->setManager(manager);
    thumb->setUrl(image);
    thumb->updateFrom(m_currentFrame);
    if (m_imageSetToDefault)
        delete m_currentFrame;
    m_currentFrame = thumb;
    thumb->setObjectName(QString("frame %1").arg(m_count));
    if (!m_imageSetToDefault)
    {
        QVector3D p = thumb->position();
        p.setX(p.x() - m_size);
        thumb->setPosition(p);

        // tell the frame scene to make the child pickable
        QChildEvent e(QEvent::ChildAdded, s);
        QApplication::sendEvent(m_frameScene, &e);

        QGLSceneNode *s = m_currentWall->clone(m_wall);
        s->setObjectName(QString("wall %1").arg(m_count));
        p = s->position();
        p.setX(p.x() - m_size);
        s->setPosition(p);
        m_currentWall = s;
    }
    m_imageSetToDefault = false;
    emit framesChanged();
    ++m_count;
}

QList<QGLPickNode *> ImageDisplay::pickNodes() const
{
    return m_frameScene->pickNodes();
}
