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


#include "imageloader.h"
#include "launcher.h"
#include "imagemanager.h"

#include <QFileInfo>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QStringList>
#include <QPixmap>
#include <QPainter>

ImageLoader::ImageLoader(ImageManager *manager)
    : QThread(manager)
{
}

ImageLoader::~ImageLoader()
{
}

void ImageLoader::run()
{
    QImage im(m_url.path());
    if (im.isNull())
        return;
    // temporary hack - image sizing is not working
    emit imageLoaded(im);
    return;
    int max = qMax(im.width(), im.height());
    QImage frm;
    if (max <= 64)
        frm = QImage(QSize(64, 64), QImage::Format_ARGB32);
    else if (max <= 128)
        frm = QImage(QSize(128, 128), QImage::Format_ARGB32);
    else if (max <= 256)
        frm = QImage(QSize(256, 256), QImage::Format_ARGB32);
    else if (max <= 512)
        frm = QImage(QSize(512, 512), QImage::Format_ARGB32);
    else
        frm = QImage(QSize(1024, 1024), QImage::Format_ARGB32);
    frm.fill(qRgba(0, 0, 0, 0));
    QPainter ptr;
    ptr.begin(&frm);
    ptr.setBackgroundMode(Qt::TransparentMode);
    QRect r;
    if (max > 1024)
    {
        if (max == im.width())
        {
            float h = float(1024) * float(im.height()) / float(im.width());
            r.setSize(QSize(1024, h));
        }
        else
        {
            float w = float(1024) * float(im.width()) / float(im.height());
            r.setSize(QSize(w, 1024));
        }
    }
    else
    {
        r.setSize(im.size());
    }
    r.setTopLeft(QPoint(frm.width() - r.width() / 2,
                        frm.height() - r.height() / 2));
    ptr.drawImage(r, im);
    ptr.end();
    emit imageLoaded(frm);
}
