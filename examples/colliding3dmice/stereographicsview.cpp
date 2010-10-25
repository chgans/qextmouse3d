/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "stereographicsview.h"

enum StereoType
{
    DoubleWideLeftRight,
    DoubleWideRightLeft,
    DoubleHighLeftRight,
    DoubleHighRightLeft
};

StereoGraphicsView::StereoGraphicsView(QWidget *parent)
    : QWidget(parent)
{
    StereoType stereoType = DoubleWideLeftRight;

    // If the command-line contains an option that starts with "-stereo-",
    // then convert it into options that define the size and type of
    // stereo window to use for the top-level view.  Possible options:
    //
    //      nhd, vga, wvga, 720p - define the screen resolution
    //      wide - use double-wide instead of double-high
    //      rl - use right-left eye order instead of left-right
    //
    QStringList args = QApplication::arguments();
    foreach (QString arg, args) {
        if (!arg.startsWith(QLatin1String("-stereo-")))
            continue;
        QStringList opts = arg.mid(8).split(QLatin1Char('-'));
        QSize size(0, 0);
        if (opts.contains(QLatin1String("nhd")))
            size = QSize(640, 360);
        else if (opts.contains(QLatin1String("vga")))
            size = QSize(640, 480);
        else if (opts.contains(QLatin1String("wvga")))
            size = QSize(800, 480);
        else if (opts.contains(QLatin1String("720p")))
            size = QSize(1280, 720);
        if (opts.contains(QLatin1String("wide"))) {
            size = QSize(size.width() * 2, size.height());
            if (opts.contains(QLatin1String("rl")))
                stereoType = DoubleWideRightLeft;
            else
                stereoType = DoubleWideLeftRight;
        } else {
            size = QSize(size.width(), size.height() * 2);
            if (opts.contains(QLatin1String("rl")))
                stereoType = DoubleHighRightLeft;
            else
                stereoType = DoubleHighLeftRight;
        }
        if (size.width() > 0 && size.height() > 0)
            resize(size);
    }

    // Create the left and right views and lay them out.
    m_leftEye = new StereoGraphicsViewEye(StereoGraphicsViewEye::LeftEye, this);
    m_rightEye = new StereoGraphicsViewEye(StereoGraphicsViewEye::RightEye, this);
    m_leftEye->setFrameStyle(0);
    m_rightEye->setFrameStyle(0);
    QVBoxLayout *vlayout;
    QHBoxLayout *hlayout;
    switch (stereoType) {
    case DoubleWideLeftRight:
        hlayout = new QHBoxLayout(this);
        hlayout->setSpacing(0);
        hlayout->setMargin(0);
        setLayout(hlayout);
        hlayout->addWidget(m_leftEye);
        hlayout->addWidget(m_rightEye);
        break;
    case DoubleWideRightLeft:
        hlayout = new QHBoxLayout(this);
        hlayout->setSpacing(0);
        hlayout->setMargin(0);
        setLayout(hlayout);
        hlayout->addWidget(m_leftEye);
        hlayout->addWidget(m_rightEye);
        break;
    case DoubleHighLeftRight:
        vlayout = new QVBoxLayout(this);
        vlayout->setSpacing(0);
        vlayout->setMargin(0);
        setLayout(vlayout);
        vlayout->addWidget(m_leftEye);
        vlayout->addWidget(m_rightEye);
        break;
    case DoubleHighRightLeft:
        vlayout = new QVBoxLayout(this);
        vlayout->setSpacing(0);
        vlayout->setMargin(0);
        setLayout(vlayout);
        vlayout->addWidget(m_leftEye);
        vlayout->addWidget(m_rightEye);
        break;
    }

    m_leftEye->setRenderHint(QPainter::Antialiasing);
    m_leftEye->setCacheMode(QGraphicsView::CacheBackground);
    m_leftEye->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    m_rightEye->setRenderHint(QPainter::Antialiasing);
    m_rightEye->setCacheMode(QGraphicsView::CacheBackground);
    m_rightEye->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
}

StereoGraphicsView::~StereoGraphicsView()
{
}

bool StereoGraphicsView::useStereo()
{
    foreach (QString arg, QApplication::arguments()) {
        if (arg.startsWith(QLatin1String("-stereo-")))
            return true;
    }
    return false;
}

void StereoGraphicsView::setScene(QGraphicsScene *scene)
{
    m_scene = scene;
    m_leftEye->setScene(scene);
    m_rightEye->setScene(scene);
}
