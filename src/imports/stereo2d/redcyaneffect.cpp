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

#include "redcyaneffect.h"
#include <QtGui/qpaintengine.h>
#include <QtOpenGL>

RedCyanEffect::RedCyanEffect(QObject *parent)
    : QGraphicsEffect(parent)
    , m_depth(0.0f)
{
}

RedCyanEffect::~RedCyanEffect()
{
}

QRectF RedCyanEffect::boundingRectFor(const QRectF &sourceRect) const
{
    qreal depth = qAbs(m_depth);
    return sourceRect.adjusted(-depth, -depth, depth, depth);
}

void RedCyanEffect::setDepth(qreal depth)
{
    if (depth != m_depth) {
        m_depth = depth;
        emit depthChanged(depth);
        update();
    }
}

void RedCyanEffect::draw(QPainter *painter)
{
    QPaintEngine *engine = painter->paintEngine();
    if (m_depth != 0.0f && (engine->type() == QPaintEngine::OpenGL ||
                            engine->type() == QPaintEngine::OpenGL2)) {
#ifdef RED_CYAN_PIXMAP_CACHE
        QPoint offset;
        QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset, NoPad);

        // Paint the source twice, with the GL color mask set up
        // to allow through different colors each time.
        // We apply the left/right eye adjustment by shifting the
        // image left or right in the x direction by the z value.
        QTransform translationRed, translationCyan, transform;
        transform = painter->transform();

        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        translationRed.translate(m_depth / 2.0f, 0);
        painter->setTransform(translationRed * transform);
        painter->drawPixmap(offset, pixmap);

        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        translationCyan.translate(-m_depth / 2.0f, 0);
        painter->setTransform(translationCyan * transform);
        painter->drawPixmap(offset, pixmap);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        painter->setTransform(transform);
#else
        // Paint the source twice, with the GL color mask set up
        // to allow through different colors each time.
        // We apply the left/right eye adjustment by shifting the
        // viewport left or right in the x direction by the z value.
        //
        // We have to shift the viewport because otherwise the
        // two drawSource() calls will draw on top of each other
        // and there will be no stereo effect.  There seems to be
        // no way to alter this with the painter's transform instead.
        // One problem with this is that the viewport is integer
        // based so we lose sub-pixel accuracy on non-even depths.

        QRect viewport = painter->viewport();
        QRect origViewport = viewport;

        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        viewport.translate(qRound(m_depth / 2.0f), 0);
        painter->setViewport(viewport);
        drawSource(painter);

        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        viewport = origViewport;
        viewport.translate(qRound(-m_depth / 2.0f), 0);
        painter->setViewport(viewport);

        drawSource(painter);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        painter->setViewport(origViewport);
#endif
    } else {
        drawSource(painter);
    }
}
