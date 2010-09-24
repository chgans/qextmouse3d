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
#include "imageloader.h"
#include "launcher.h"
#include "qatlas.h"

#include <QSemaphore>
#include <QDir>
#include <QTime>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

ImageManager::ImageManager(QObject *parent)
    : QThread(parent)
    , m_atlas(new QAtlas)
    , m_stop(true)
{
}

ImageManager::~ImageManager()
{
    delete m_atlas;
}

/*!
    Stop the running file scan if any, then sit waiting in the event loop
    for a quit call.
*/
void ImageManager::stop()
{
    m_stop = true;
}

/*!
    Stop the running file scan if any, then exit the event loop by calling
    QThread::quit().
*/
void ImageManager::quit()
{
    m_stop = true;
    QThread::quit();
}

/*!
    Stop any current scan, and (after a short delay) initiate a new scan
    for image file names.
*/
void ImageManager::rescan()
{
    m_stop = true;
    QTimer::singleShot(20, this, SLOT(scanForFiles()));
}

void ImageManager::scanForFiles()
{
    // debug
    QTime timer;
    timer.start();

    m_stop = false;
    m_count = 0;
    QStringList queue;
    queue.append(m_url.path());
    QSet<QString> loopProtect;
    while (queue.size() > 0 && !m_stop)
    {
        QString path = queue.takeFirst();
        QFileInfo u(path);
        if (u.isSymLink())
            path = u.symLinkTarget();
        if (u.isDir())
        {
            if (!loopProtect.contains(path))
            {
                loopProtect.insert(path);
                QDir dir(path);
                QStringList entries = dir.entryList();
                QStringList::const_iterator it = entries.constBegin();
                for ( ; it != entries.constEnd(); ++it)
                {
                    // ignore hidden files, system directories
                    if ((*it).startsWith("."))
                        continue;
                    queue.append(dir.absoluteFilePath(*it));
                }
            }
        }
        else
        {
            if (u.isFile() && u.isReadable())
            {
                // do no checking here for file extensions etc - just
                // forward any readable file found under the pictures
                // directory to the QImage loader, and let it sort out
                // if the thing can be loaded as an image.
                QUrl url2;
                url2.setScheme("file");
                url2.setPath(u.absoluteFilePath());
                emit imageUrl(url2);
                ++m_count;
            }
        }
        QCoreApplication::processEvents();
    }
    if (m_stop)
        qDebug() << "stop / quit detected";
    fprintf(stderr, "ImageManager::run - %d images found under %s in %d ms\n",
            m_count, qPrintable(m_url.path()), timer.elapsed());
}

void ImageManager::run()
{
    qDebug() << ">>>>>> ImageManager::run()" << QThread::currentThread();
    m_count = 0;

    Launcher launcher;
    launcher.setUrl(m_url);

    connect(&launcher, SIGNAL(finished()), this, SLOT(release()));
    connect(this, SIGNAL(createLoader(QUrl)), &launcher, SLOT(createLoader(QUrl)));

    launcher.start(QThread::IdlePriority);

    fprintf(stderr, "ImageManager::run - start %p\n", this);
    QTime timer;
    timer.start();

    // execute once in the event loop below
    QTimer::singleShot(0, this, SLOT(scanForFiles()));

    qDebug() << ">>> ImageManager - entering event loop";
    exec();
    qDebug() << "<<< ImageManager - exiting event loop";

    qDebug() << "<<<<<< ImageManager::run()" << QThread::currentThread();
}
