/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#ifndef QGLSECTION_H
#define QGLSECTION_H

#include "qglnamespace.h"
#include "qgldisplaylist.h"
#include "qglpainter.h"

#include <QtOpenGL/qgl.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLPainter;

class Q_QT3D_EXPORT QGLSection
{
public:
    inline QGLSection(QGLDisplayList *d, QGL::Smoothing s = QGL::Smooth);
    ~QGLSection();

    // state accessors
    inline QGL::Smoothing smoothing() const;
    inline int start() const;
    inline int count() const;
    inline QGLDisplayList *displayList() const;

    // display methods
    inline void draw(QGLPainter *painter) const;
private:
    Q_DISABLE_COPY(QGLSection);
    friend class QGLDisplayList;

    GLuint m_start;
    GLuint m_count;

    QGL::Smoothing m_smoothing;
    QGLDisplayList *m_displayList;
};

inline QGLSection::QGLSection(QGLDisplayList *d,  QGL::Smoothing s)
    : m_start(0)
    , m_count(0)
    , m_smoothing(s)
    , m_displayList(d)
{
    Q_ASSERT(m_displayList);
    m_start = m_displayList->count();
    m_displayList->addSection(this);
}

QGL::Smoothing QGLSection::smoothing() const
{
    return m_smoothing;
}

int QGLSection::start() const
{
    return m_start;
}

int QGLSection::count() const
{
    return m_count;
}

QGLDisplayList *QGLSection::displayList() const
{
    return m_displayList;
}

void QGLSection::draw(QGLPainter *painter) const
{
    m_displayList->finalize();
    m_displayList->draw(painter, m_start, m_count);
}


#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGLSection &section);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLSECTION_H
