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


#include "imagemanager.h"
#include "qatlas.h"
#include "filescanner.h"
#include "threadpool.h"

#include <QTime>
#include <QTimer>

ImageManager::ImageManager()
    : m_atlas(new QAtlas)
{
}

ImageManager::~ImageManager()
{
    qDebug() << "~ImageManager";
    delete m_atlas;
}

// INVARIANT: only ever called before the run() function is started
// therefore no need for synchronized url
void ImageManager::setImageBaseUrl(const QUrl &url)
{
    Q_ASSERT(!isRunning());
    m_url = url;
}

/*!
    Stop the running threads if any, then sit waiting in the event loop
    for a quit call.
*/
void ImageManager::stop()
{
    emit stopAll();
    qDebug() << "ImageManager::stop" << QThread::currentThread();
}

void ImageManager::scanForFiles()
{
    // TODO: In a real app there would be a way to detect new files arriving
    // and trigger a rescan to pick these new files up.  Here we just scan
    // once and then destroy the scanner, to save on resources.

    FileScanner *scanner = new FileScanner;
    scanner->setBaseUrl(m_url);
    connect(scanner, SIGNAL(imageUrl(QUrl)), this, SIGNAL(imageUrl(QUrl)));
    connect(scanner, SIGNAL(finished()), scanner, SLOT(deleteLater()));
    connect(this, SIGNAL(stopAll()), scanner, SLOT(stop()));
    scanner->start();
}

void ImageManager::quit()
{
    qDebug() << "ImageManager::quit";
    QThread::quit();
}

void ImageManager::debugStuff()
{
    qDebug() << "ImageManager::debugStuff()  -- Got a signal from:" << sender();
}

void ImageManager::run()
{
    qDebug() << ">>>>>> ImageManager::run()" << QThread::currentThread();

    if (m_url.scheme() != "file")
    {
        qWarning("URL scheme %s not yet supported", qPrintable(m_url.scheme()));
        return;
    }

    // execute once in the event loop below
    QTimer::singleShot(0, this, SLOT(scanForFiles()));

    ThreadPool pool;

    connect(this, SIGNAL(deployLoader(QUrl)), &pool, SLOT(deployLoader(QUrl)));
    connect(this, SIGNAL(stopAll()), &pool, SLOT(stop()));

    connect(&pool, SIGNAL(stopped()), this, SLOT(quit()));

    fprintf(stderr, "ImageManager::run - start %p\n", this);
    QTimer timer;
    connect(&timer, SIGNAL(timeout()), this, SLOT(debugStuff()));
    timer.start(1000);

    qDebug() << ">>> ImageManager - entering event loop";
    exec();
    qDebug() << "<<< ImageManager - exiting event loop";

    qDebug() << "<<<<<< ImageManager::run()" << QThread::currentThread();
}
