#include "filescanner.h"

#include <QTime>
#include <QFileInfo>
#include <QStringList>
#include <QSet>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QImageReader>

FileScanner::FileScanner(QObject *parent)
    : QThread(parent)
{
    m_stop = 0;
}

FileScanner::~FileScanner()
{
    qDebug() << "FileScanner::~FileScanner()";
}

void FileScanner::stop()
{
    m_stop.ref();
}

void FileScanner::run()
{
    scan();
}

void FileScanner::scan()
{
    qDebug() << "FileScanner::scan" << m_url.toString();

    // debug - remove me
    QTime timer;
    timer.start();
    int count = 0;

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
                // small optimization: if the file has a suffix, check if that
                // is known as an image format before sending to loader
                QString ext = u.suffix();
                if (ext.isEmpty() ||
                        QImageReader::supportedImageFormats().contains(ext.toLocal8Bit()))
                {
                    QUrl url2;
                    url2.setScheme("file");
                    url2.setPath(u.absoluteFilePath());
                    emit imageUrl(url2);
                    ++count;
                }
            }
        }
        QCoreApplication::processEvents();
        QThread::yieldCurrentThread();
    }
    if (m_stop)
        qDebug() << "stop detected";
    fprintf(stderr, "FileScanner::run - %d images found under %s in %d ms\n",
            count, qPrintable(m_url.path()), timer.elapsed());
}
