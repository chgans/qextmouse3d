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

#include "bytereader.h"
#include "thumbnailableimage.h"

#include <QByteArray>
#include <QThread>
#include <QImage>
#include <QImageReader>
#include <QUrl>
#include <QFile>
#include <QCoreApplication>

ByteReader::ByteReader()
{
    m_stop = 0;
    m_loading = 0;
}

void ByteReader::loadFile(const QUrl &url)
{
    if (!url.isEmpty() && !m_stop)
    {
        m_loading.ref();

        // FIXME: actually handle remote files
        QString fn = url.toLocalFile();
        int pos = fn.lastIndexOf('.');
        QString ext;
        if (pos != -1)
            ext = fn.mid(pos).toUpper();
        if (ext.isEmpty() ||
                !QImageReader::supportedImageFormats().contains(ext.toLocal8Bit()))
            ext = QString();
        QImage im;
        QFile f(fn);
        QString errorMessage;
        if (f.open(QIODevice::ReadOnly))
        {
            QByteArray bytes;
            while (!f.atEnd() & !m_stop)
            {
                bytes.append(f.read(1024));
                QCoreApplication::processEvents();
            }
            im = ext.isEmpty() ? QImage::fromData(bytes)
                               : QImage::fromData(bytes, qPrintable(ext));
        }
        else
        {
            errorMessage = tr("Could not read: %1").arg(url.toString());
        }

        QCoreApplication::processEvents();
        if (!m_stop)
        {
            if (im.isNull())
            {
                im = QImage(QSize(128, 128), QImage::Format_ARGB32);
                im.fill(qRgba(0, 30, 50, 64));
                QPainter ptr;
                ptr.begin(&im);
                ptr.setBackgroundMode(Qt::TransparentMode);
                if (errorMessage.isEmpty())
                    errorMessage = tr("Could not load: %1").arg(url.toString());
                ptr.setPen(QColor("orange"));
                ptr.drawText(im.rect(), Qt::AlignCenter, errorMessage);
                ptr.end();
            }
            else
            {
                // TODO: Fix size limit
                // This arbitrary size limit is really a debugging/development thing
                // In a real program once you had loaded the full image, the photo-
                // viewer would allow zooming and panning all around in the image
                // so loading a 2896 pixel × 1944 pixel photo would make sense even
                // on a small screen.  For now work with fairly cruddy image quality.
                if (im.size().width() > 1024 || im.size().height() > 768)
                    im = im.scaled(QSize(1024, 768), Qt::KeepAspectRatio,
                                   QThread::idealThreadCount() > 2 ?
                                       Qt::SmoothTransformation : Qt::FastTransformation);
            }

            ThumbnailableImage thumb;
            thumb.setData(im);
            thumb.setUrl(url);

            Q_ASSERT(!im.isNull());
            Q_ASSERT(!thumb.isNull());

            emit imageLoaded(thumb);
        }

        m_loading.deref();
    }

    if (m_stop)
        emit stopped();

}

void ByteReader::stop()
{
    m_stop.ref();
    if (!m_loading)
        emit stopped();
}
