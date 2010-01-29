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

#include "qglcontextwatcher.h"
#include <QtOpenGL/qgl.h>
#include <QtOpenGL/private/qgl_p.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLContextWatcher
    \brief The QGLContextWatcher class tracks context destruction and shared resource transfers.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers
*/

class QGLContextWatcherPrivate
{
public:
    QGLContextWatcherPrivate()
        : context(0),
          trackResourceTransfer(false),
          signalHooked(false)
    {
    }

    const QGLContext *context;
    bool trackResourceTransfer;
    bool signalHooked;
};

/*!
    Creates a new context watcher and attaches it to \a parent.

    \sa setContext()
*/
QGLContextWatcher::QGLContextWatcher(QObject *parent)
    : QObject(parent), d_ptr(new QGLContextWatcherPrivate())
{
}

/*!
    Creates a new context watcher for \a context and attaches it to \a parent.
*/
QGLContextWatcher::QGLContextWatcher(const QGLContext *context, QObject *parent)
    : QObject(parent), d_ptr(new QGLContextWatcherPrivate())
{
    setContext(context);
}

/*!
    Destroys this context watcher.
*/
QGLContextWatcher::~QGLContextWatcher()
{
}

/*!
    Returns the context that is being watched by this object.

    \sa setContext(), contextDestroyed()
*/
const QGLContext *QGLContextWatcher::context() const
{
    Q_D(const QGLContextWatcher);
    return d->context;
}

/*!
    Sets the \a context that is being watched by this object.

    \sa context(), contextDestroyed()
*/
void QGLContextWatcher::setContext(const QGLContext *context)
{
    Q_D(QGLContextWatcher);
    d->context = context;
    if (context && !d->signalHooked) {
        d->signalHooked = true;
        connect(QGLSignalProxy::instance(),
                SIGNAL(aboutToDestroyContext(const QGLContext *)),
                this,
                SLOT(aboutToDestroyContext(const QGLContext *)));
    }
}

/*!
    Returns true if this object is tracking the transfer of resources
    between contexts in the same sharing group; false otherwise.

    When resource transfer tracking is enabled, resourcesTransferred()
    will be emitted in place of contextDestroyed() if there are other
    contexts in the same sharing group as context() when it is destroyed.

    \sa setTrackResourceTransfer(), resourcesTransferred()
*/
bool QGLContextWatcher::trackResourceTransfer() const
{
    Q_D(const QGLContextWatcher);
    return d->trackResourceTransfer;
}

/*!
    Enables or disables tracking the transfer of resources
    between contexts in the same sharing group according to \a enable.

    When \a enable is true, resourcesTransferred() will be emitted in
    place of contextDestroyed() if there are other contexts in the same
    sharing group as context() when it is destroyed.

    \sa trackResourceTransfer(), resourcesTransferred()
*/
void QGLContextWatcher::setTrackResourceTransfer(bool enable)
{
    Q_D(QGLContextWatcher);
    d->trackResourceTransfer = enable;
}

/*!
    \fn void QGLContextWatcher::contextDestroyed()

    Signal that is emitted when context() is destroyed.

    If trackResourceTransfer() is true, then resourcesTransferred()
    will be emitted instead of contextDestroyed() if there are other
    contexts in the same sharing group when context() is destroyed.

    \sa resourcesTransferred()
*/

/*!
    \fn void QGLContextWatcher::resourcesTransferred(const QGLContext *shareContext)

    Signal that is emitted when context() is destroyed,
    trackResourceTransfer() is true, and the resources have been
    transferred to \a shareContext.  After the signal is emitted,
    context() will be changed to \a shareContext.

    \sa contextDestroyed(), trackResourceTransfer()
*/

// Private function in QtOpenGL.
const QGLContext *qt_gl_transfer_context(const QGLContext *);

/*!
    \internal
*/
void QGLContextWatcher::aboutToDestroyContext(const QGLContext *context)
{
    Q_D(QGLContextWatcher);
    if (d->context != context)
        return;
    if (d->trackResourceTransfer) {
        const QGLContext *shareContext = qt_gl_transfer_context(context);
        if (shareContext) {
            emit resourcesTransferred(shareContext);
            d->context = shareContext;
            return;
        }
    }
    emit contextDestroyed();
    d->context = 0;
}

QT_END_NAMESPACE
