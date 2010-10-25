#include "filescanner.h"

#include <QTime>
#include <QFileInfo>
#include <QStringList>
#include <QSet>
#include <QDir>
#include <QCoreApplication>
#include <QImageReader>
#include <QDebug>

FileScanner::FileScanner(QObject *parent)
    : QThread(parent)
{
    m_stop = 0;
}

FileScanner::~FileScanner()
{
    // nothing to do here
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
    QStringList queue;
    queue.append(m_url.path());
    QSet<QString> loopProtect;
    int count = 0;
    while (queue.size() > 0 && !m_stop && count < 300)
    {
        QString path = queue.takeFirst();
        QFileInfo u(path);
        if (u.isSymLink())
        {
            path = u.symLinkTarget();
            u = QFileInfo(path);
        }
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
}
