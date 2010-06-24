/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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
