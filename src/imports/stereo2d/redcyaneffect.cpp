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
#include <QtGui/private/qgraphicseffect_p.h>
#include <QtGui/private/qgraphicsitem_p.h>
#include <QtOpenGL>

RedCyanEffect::RedCyanEffect(QObject *parent)
    : QGraphicsEffect(parent)
    , m_depth(0.0f)
    , m_sourced(0)
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
        // Paint the source twice, with the GL color mask set up
        // to allow through different colors each time.
        // We apply the left/right eye adjustment by shifting the
        // item left or right in the x direction by the z value.

        // Modify the effectTransform to adjust the position
        // and draw the left eye version of the source.
        QGraphicsItemPaintInfo *drawContext = m_sourced->info;
        const QTransform *origTransform = drawContext->effectTransform;
        QTransform transform(Qt::Uninitialized);
        if (!origTransform) {
            transform = QTransform::fromTranslate(m_depth / 2.0f, 0.0f);
        } else {
            transform = *origTransform;
            transform.translate(m_depth / 2.0f, 0.0f);
        }
        drawContext->effectTransform = &transform;
        glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
        drawSource(painter);

        // Modify the effectTransform again for the right eye version.
        if (!origTransform) {
            transform = QTransform::fromTranslate(-m_depth / 2.0f, 0.0f);
        } else {
            transform = *origTransform;
            transform.translate(-m_depth / 2.0f, 0.0f);
        }
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        drawSource(painter);

        // Restore the original effectTransform and color mask.
        drawContext->effectTransform = origTransform;
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    } else {
        drawSource(painter);
    }
}

void RedCyanEffect::sourceChanged(QGraphicsEffect::ChangeFlags flags)
{
    if (flags & SourceAttached) {
        QGraphicsEffectPrivate *ep =
            static_cast<QGraphicsEffectPrivate *>
                (QObjectPrivate::get(this));
        m_sourced = static_cast<QGraphicsItemEffectSourcePrivate *>
            (QObjectPrivate::get(ep->source));
    } else if (flags & SourceDetached) {
        m_sourced = 0;
    }
}
