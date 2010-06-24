/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt/3D module of the Qt Toolkit.
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

#include "stereoeffect.h"
#include "stereographicsview.h"
#include <QtGui>
#include <QtOpenGL>

StereoEffect::StereoEffect(QObject *parent)
    : QGraphicsEffect(parent)
    , m_z(0.0f)
{
}

StereoEffect::~StereoEffect()
{
}

QRectF StereoEffect::boundingRectFor(const QRectF &sourceRect) const
{
    qreal z = qAbs(m_z);
    return sourceRect.adjusted(-z, -z, z, z);
}

void StereoEffect::setZ(qreal z)
{
    if (z != m_z) {
        m_z = z;
        emit zChanged(z);
    }
}

void StereoEffect::draw(QPainter *painter)
{
    StereoGraphicsViewEye::Eye eye = StereoGraphicsViewEye::NoEye;
    QPaintDevice *dev = painter->device();
    if (dev->devType() == QInternal::Widget) {
        StereoGraphicsViewEye *view = qobject_cast<StereoGraphicsViewEye *>
            (static_cast<QWidget *>(dev)->parentWidget());
        if (view)
            eye = view->eye();
    }
    if (eye != StereoGraphicsViewEye::NoEye) {
        // We have two StereoGraphicsView instances for left and right eyes.
        if (m_z != 0.0f) {
            QPoint offset;
            QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset, NoPad);

            QTransform translationEye, transform;
            transform = painter->transform();

            if (eye == StereoGraphicsViewEye::LeftEye)
                translationEye.translate(m_z / 2.0f, 0);
            else
                translationEye.translate(-m_z / 2.0f, 0);
            painter->setTransform(transform * translationEye);
            painter->drawPixmap(offset, pixmap);

            painter->setTransform(transform);
        } else {
            drawSource(painter);
        }
    } else {
        // Use red/cyan anaglyphs to draw the source with stereo depth.
        if (m_z != 0.0f) {
            QPoint offset;
            QPixmap pixmap = sourcePixmap(Qt::LogicalCoordinates, &offset, NoPad);

            // Paint the source twice, with the GL color mask set up
            // to allow through different colors each time.
            // We apply the left/right eye adjustment by shifting the
            // image left or right in the x direction by the z value.
            QTransform translationRed, translationCyan, transform;
            transform = painter->transform();

            glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
            translationRed.translate(m_z / 2.0f, 0);
            painter->setTransform(transform * translationRed);
            painter->drawPixmap(offset, pixmap);

            glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
            translationCyan.translate(-m_z / 2.0f, 0);
            painter->setTransform(transform * translationCyan);
            painter->drawPixmap(offset, pixmap);

            glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            painter->setTransform(transform);
        } else {
            drawSource(painter);
        }
    }
}
