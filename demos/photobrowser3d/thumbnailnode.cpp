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
#include "thumbnailableimage.h"
#include "thumbnaileffect.h"
#include "imagemanager.h"

#include <QtGui/qmatrix4x4.h>

ThumbnailNode::ThumbnailNode(QObject *parent)
    : QGLSceneNode(parent)
    , m_image(0)
    , m_thresholdSquared(20.0f * 20.0f)
    , m_manager(0)
    , m_defaultMaterial(-1)
    , m_loading(false)
{
}

void ThumbnailNode::draw(QGLPainter *painter)
{
    if (m_defaultMaterial == -1)
        m_defaultMaterial = materialIndex();

    ThumbnailEffect *effect = qobject_cast<ThumbnailEffect*>(painter->effect());
    if (effect == 0)
    {
        qWarning("This node can only be drawn by thumbnail effect!");
        return;
    }

    // we know that the only transforms on these nodes are translation so
    // take some shortcuts with finding cullability
    QVector3D pos = position();
    QBox3D bb = boundingBox();
    bb.setExtents(bb.mincorner + pos, bb.maxcorner + pos);
    if (bb.isFinite() && !bb.isNull() && painter->isCullable(bb))
        return;

    QMatrix4x4 m = painter->modelViewMatrix().top();
    pos = m.mapVector(pos);

    // under the model-view transformation the eye of the camera is located
    // at the origin, so mapping the position of this node will make it into
    // a vector from the origin/camera - taking the magnitude of this gives
    // the distance from the eye to the node.

    qreal magSquared = pos.lengthSquared();

    if (magSquared > (1.4f * m_thresholdSquared))
    {
        // long way from camera - throw away image (if it isn't already)
        // and just draw the default frame - it will be just a dot in the
        // distance anyway
        effect->setThumbnail(false);
        setMaterialIndex(m_defaultMaterial);
        QGLSceneNode::draw(painter);
        delete m_image;
        m_image = 0;
        m_loading = false;
        return;
    }

    if (m_image == 0)
    {
        // medium distance away from camera - need the image loaded
        // start that process now
        emit imageRequired(m_url);
        m_loading = true;
        setMaterialIndex(m_defaultMaterial);
        QGLSceneNode::draw(painter);
        return;
    }

    // asynchronous call
    m_image->setThumbnailed(magSquared > m_thresholdSquared);

    // even tho' was just set, use whether thumbnail actually ready to choose
    // what mode to display in - thumbnail or normal
    effect->setThumbnail(m_image->isThumbnailed());
    if (m_image->isThumbnailed())
    {
        m_image->draw(painter);
    }
    else
    {
        QGLSceneNode::draw(painter);
    }
}

void ThumbnailNode::setImage(const ThumbnailableImage &image)
{
    m_image = new ThumbnailableImage(image);

    /*  -- cope with big pictures...?
    int scale = image.width() / 1024;
    if (scale != 1)
        thumb->setScale(QVector3D(scale, scale, scale));
    */

    // load the image as a new material into the current frame
    /*
    QGLMaterial *mat = image.atlas()
    QGLTexture2D *tex = new QGLTexture2D(mat);
    tex->setHorizontalWrap(QGL::Clamp);
    tex->setImage(image);
    mat->setTexture(tex);
    setMaterial(mat);
    */
}

void ThumbnailNode::updateFrom(const QGLSceneNode *node)
{
    Q_ASSERT(node);
    setPalette(node->palette());
    setMaterialIndex(node->materialIndex());
    setGeometry(node->geometry());
    setStart(node->start());
    setCount(node->count());
    setPosition(node->position());
}
