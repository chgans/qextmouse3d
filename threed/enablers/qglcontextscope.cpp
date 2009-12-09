/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qglcontextscope.h"
#include <QtOpenGL/qgl.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLContextScope
    \brief The QGLContextScope class temporarily switches to another QGLContext within a program scope.
    \ingroup qt3d
    \ingroup qt3d::enablers

    QGLContextScope is typically used in GL resource management to
    temporarily switch to another context, operate on a resource
    such as a texture or shader program, and then return to the
    original GL context when the stack is destroyed.

    For example, the following code temporarily switches to \c ctx
    and deletes the specified \c texture:

    \code
    {
        QGLContextScope scope(ctx);
        glDeleteTextures(1, &texture);
    }
    \endcode

    The original context will be automatically restored when \c scope
    goes out of scope.

    A useful feature of QGLContextScope is that it only calls
    QGLContext::makeCurrent() if the new context is not sharing
    with the current context.  This can reduce the number of
    context switches during GL resource management.
*/

/*!
    Constructs a scope handler for \a context.  If \a context is
    not sharing with the current context, then \a context will
    be made current.  When this scope handler is destroyed,
    the original context will be made current again.
*/
QGLContextScope::QGLContextScope(const QGLContext *context)
{
    QGLContext *current =
        const_cast<QGLContext *>(QGLContext::currentContext());
    if (context && context != current &&
            !QGLContext::areSharing(context, current)) {
        const_cast<QGLContext *>(context)->makeCurrent();
    }
    m_restoreContext = current;
}

/*!
    Destroys this scope handler and returns to the original context
    that was current when this object was constructed.
*/
QGLContextScope::~QGLContextScope()
{
    QGLContext *current =
        const_cast<QGLContext *>(QGLContext::currentContext());
    if (m_restoreContext) {
        if (m_restoreContext != current)
            m_restoreContext->makeCurrent();
    } else if (current != 0) {
        current->doneCurrent();
    }
}

QT_END_NAMESPACE
