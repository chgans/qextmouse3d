/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef QMOUSE3DEVENT_H
#define QMOUSE3DEVENT_H

#include <QtCore/qcoreevent.h>
#include "qt3dglobal.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QMouse3DEventPrivate;

class Q_QT3D_EXPORT QMouse3DEvent : public QEvent
{
public:
    QMouse3DEvent(short translateX, short translateY, short translateZ,
                  short rotateX, short rotateY, short rotateZ);
    ~QMouse3DEvent();

    static const QEvent::Type type;

    short translateX() const { return m_translateX; }
    short translateY() const { return m_translateY; }
    short translateZ() const { return m_translateZ; }

    short rotateX() const { return m_rotateX; }
    short rotateY() const { return m_rotateY; }
    short rotateZ() const { return m_rotateZ; }

private:
    short m_translateX, m_translateY, m_translateZ;
    short m_rotateX, m_rotateY, m_rotateZ;
    QMouse3DEventPrivate *d_ptr;    // For future expansion.

    Q_DISABLE_COPY(QMouse3DEvent)   // d_ptr is not default-copiable.
};

inline QMouse3DEvent::QMouse3DEvent
        (short translateX, short translateY, short translateZ,
         short rotateX, short rotateY, short rotateZ)
    : QEvent(QMouse3DEvent::type)
    , m_translateX(translateX)
    , m_translateY(translateY)
    , m_translateZ(translateZ)
    , m_rotateX(rotateX)
    , m_rotateY(rotateY)
    , m_rotateZ(rotateZ)
    , d_ptr(0)
{
}

QT_END_NAMESPACE

QT_END_HEADER

#endif
