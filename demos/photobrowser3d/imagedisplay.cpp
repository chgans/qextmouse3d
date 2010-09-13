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

#include "qglbuilder.h"
#include "qglcube.h"
#include "qframesscene.h"

#include <QApplication>
#include <QChildEvent>
#include <QUrl>
#include <QImage>

ImageDisplay::ImageDisplay(QObject *parent, QGLMaterialCollection *materials)
    : QGLSceneNode(parent)
    , m_wall(0)
    , m_frames(0)
    , m_currentWall(0)
    , m_currentFrame(0)
    , m_imageSetToDefault(false)
    , m_count(0)
{
    QGLBuilder builder(materials);
    setObjectName("ImageDisplay");

    // build the wall
    m_wall = builder.currentNode();
    m_wall->setObjectName("Wall");
    builder.pushNode();
    m_currentWall = builder.currentNode();
    m_currentWall->setObjectName("wall 0");
    builder << QGLCubeFace(QGLCubeFace::Front, 2.0f);
    builder.popNode();

    // build the frames
    m_frames = builder.currentNode();
    m_frames->setObjectName("Frames");
    builder.pushNode();
    m_currentFrame = builder.newNode();
    m_currentFrame->setObjectName("frame 0");
    builder << QGLCubeFace(QGLCubeFace::Front, 1.0f);
    builder.popNode();

    QGLSceneNode *top = builder.finalizedSceneNode();
    top->setParent(this);

    // paint the wall
    m_wall->setEffect(QGL::LitMaterial);
    QGLMaterial *mat = new QGLMaterial();
    mat->setAmbientColor(Qt::darkGray);
    mat->setDiffuseColor(Qt::darkGray);
    m_wall->setMaterial(mat);
    m_wall->setPosition(QVector3D(0.0f, 0.0f, -1.0f));

    // paint the frames
    m_frames->setEffect(QGL::FlatReplaceTexture2D);
    m_frames->setEffectEnabled(true);
    mat = new QGLMaterial();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(QImage(":/res/images/no-images-yet.png"));
    mat->setTexture(tex);
    m_currentFrame->setMaterial(mat);

    // make the frames pickable
    m_frameScene = new QFramesScene(this);
    m_frameScene->setRootNode(m_frames);
    m_frameScene->setPickable(true);

    m_imageSetToDefault = true;
}

void ImageDisplay::addImage(const QImage &image)
{
    // clone the current frame and shift the clone to the left
    // (or use the place holder frame if this is the first one)
    Q_ASSERT(m_currentFrame);
    QGLSceneNode *s = m_currentFrame;
    if (!m_imageSetToDefault)
    {
        s = m_currentFrame->clone(m_frames);
        ++m_count;
        s->setObjectName(QString("frame %1").arg(m_count));
        QVector3D p = s->position();
        p.setX(p.x() - 2.0f);
        s->setPosition(p);
        m_currentFrame = s;
        int scale = image.width() / 1024;
        if (scale != 1)
            s->setScale(QVector3D(scale, scale, scale));

        // tell the frame scene to make the child pickable
        QChildEvent e(QEvent::ChildAdded, s);
        QApplication::sendEvent(m_frameScene, &e);

        s = m_currentWall->clone(m_wall);
        s->setObjectName(QString("wall %1").arg(m_count));
        p = s->position();
        p.setX(p.x() - 2.0f);
        s->setPosition(p);
        m_currentWall = s;
    }
    m_imageSetToDefault = false;

    // load the image as a new material into the current frame
    QGLMaterial *mat = new QGLMaterial();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setHorizontalWrap(QGL::Clamp);
    tex->setImage(image);
    mat->setTexture(tex);
    m_currentFrame->setMaterial(mat);
    emit framesChanged();
}

QList<QGLPickNode *> ImageDisplay::pickNodes() const
{
    return m_frameScene->pickNodes();
}
