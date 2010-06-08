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

#include "qgldisplaylist.h"
#include "qglcube.h"

#include <QUrl>
#include <QImage>

ImageDisplay::ImageDisplay(QObject *parent, QGLMaterialCollection *materials)
    : QGLDisplayList(parent, materials)
    , m_wall(0)
    , m_frames(0)
    , m_currentWall(0)
    , m_currentFrame(0)
    , m_imageSetToDefault(false)
{
    newSection(QGL::Faceted);

    // build the wall
    m_wall = currentNode();
    pushNode();
    m_currentWall = currentNode();
    *this << QGLCubeFace(QGLCubeFace::Front, 2.0f);
    popNode();

    // build the frames
    m_frames = currentNode();
    pushNode();
    m_currentFrame = newNode();
    *this << QGLCubeFace(QGLCubeFace::Front, 1.0f);
    popNode();

    finalize();

    // paint the wall
    m_wall->setEffect(QGL::LitMaterial);
    QGLMaterial *mat = new QGLMaterial();
    mat->setAmbientColor(Qt::darkGray);
    mat->setDiffuseColor(Qt::darkGray);
    m_wall->setMaterial(mat);
    m_wall->setPosition(QVector3D(0.0f, 0.0f, -0.3f));

    // paint the frames
    m_frames->setEffect(QGL::FlatReplaceTexture2D);
    m_frames->setEffectEnabled(true);
    mat = new QGLMaterial();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(QImage(":/res/images/no-images-yet.png"));
    mat->setTexture(tex);
    m_currentFrame->setMaterial(mat);

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
        QVector3D p = s->position();
        p.setX(p.x() - 2.0f);
        s->setPosition(p);
        m_currentFrame = s;
        s = m_currentWall->clone(m_wall);
        p = s->position();
        p.setX(p.x() - 2.0f);
        s->setPosition(p);
        m_currentWall = s;
    }
    m_imageSetToDefault = false;

    // load the image as a new material into the current frame
    QGLMaterial *mat = new QGLMaterial();
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setImage(image);
    mat->setTexture(tex);
    m_currentFrame->setMaterial(mat);
}
