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
#include "imagemanager.h"
#include "bytereader.h"

#include <QFileInfo>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QStringList>
#include <QPixmap>
#include <QPainter>
#include <QBuffer>
#include <QImageReader>
#include <QMutex>
#include <QMutexLocker>
#include <QTimer>

ImageLoader::ImageLoader()
{
    m_stop = 0;
}

ImageLoader::~ImageLoader()
{
    // nothing to do here
}

QUrl ImageLoader::url() const
{
    return m_url;
}

void ImageLoader::setUrl(const QUrl &url)
{
    qDebug() << "###### setUrl:" << url;
    m_url = url;
    if (!m_stop && isRunning())
    {
        emit readRequired(url);
        qDebug() << "      emit readRequired:" << url;
    }
}

void ImageLoader::stop()
{
    m_stop.ref();
    emit stopLoading();

    qDebug() << "ImageLoader::stop()" << QThread::currentThread();
}

void ImageLoader::debugStuff()
{
    qDebug() << "ImageLoader::debugStuff" << QThread::currentThread() << m_url;
}

void ImageLoader::queueInitialUrl()
{
    emit readRequired(m_url);
}

void ImageLoader::unusedTimeout()
{
    emit unused();
    qDebug() << "unusedTimeout" << m_url << QThread::currentThread();
}

void ImageLoader::run()
{
    qDebug() << ">>>>> ImageLoader::run()" << m_url.toString() << QThread::currentThread();

    ByteReader reader;
    connect(this, SIGNAL(readRequired(QUrl)), &reader, SLOT(loadFile(QUrl)));
    connect(&reader, SIGNAL(imageLoaded(ThumbnailableImage)),
            this, SIGNAL(imageLoaded(ThumbnailableImage)));
    connect(this, SIGNAL(stopLoading()), &reader, SLOT(stop()));

    connect(&reader, SIGNAL(stopped()), this, SLOT(quit()));

    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(unusedTimeout()));
    timer.start(2 * 60 * 1000);

    if (!m_url.isEmpty())
        QTimer::singleShot(0, this, SLOT(queueInitialUrl()));

    qDebug() << "ImageLoader - entering event loop:" << m_url;
    exec();

    qDebug() << "<<<<< ImageLoader::run()" << m_url.toString() << QThread::currentThread();
}
