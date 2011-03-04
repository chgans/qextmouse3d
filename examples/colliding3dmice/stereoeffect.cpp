/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
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
