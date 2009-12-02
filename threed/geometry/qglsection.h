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
#include "qglpainter.h"
#include "qgeometrydata_p.h"
#include "qlogicalvertex.h"

#include <QtOpenGL/qgl.h>
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLPainter;
class QGLDisplayList;
class QGLSectionPrivate;

class Q_QT3D_EXPORT QGLSection
{
public:
    QGLSection(QGLDisplayList *d, QGL::Smoothing s = QGL::Smooth);
    ~QGLSection();

    // data accessors
    QGL::VectorArray vertices() const;
    QGL::VectorArray normals() const;
    QGL::IndexArray indices() const;
    QGL::TexCoordArray texCoords() const;
    QGL::ColorArray colors() const;
    int indexOf(const QLogicalVertex &lv) const;
    const QLogicalVertex &vertexAt(int i) const;

    // state accessors
    inline bool hasData(QLogicalVertex::Types types);
    inline QGL::Smoothing smoothing() const;
    inline int start() const;
    inline int count() const;
    inline QGLDisplayList *displayList() const;

    // display methods
    inline void draw(QGLPainter *painter) const;

    // data update methods
    inline void append(const QLogicalVertex &vertex);
protected:
    virtual void appendSmooth(const QLogicalVertex &vertex);
    virtual void appendFaceted(const QLogicalVertex &vertex);
    virtual int updateTexCoord(int position, const QVector2D &t);
private:
    Q_DISABLE_COPY(QGLSection);
    friend class QGLDisplayList;

    GLuint m_start;
    GLuint m_count;

    QGL::Smoothing m_smoothing;
    QGLDisplayList *m_displayList;
    QLogicalVertex::Types m_dataTypes;
    QGLSectionPrivate *d;
};

inline QGL::Smoothing QGLSection::smoothing() const
{
    return m_smoothing;
}

inline int QGLSection::start() const
{
    return m_start;
}

inline int QGLSection::count() const
{
    return m_count;
}

inline QGLDisplayList *QGLSection::displayList() const
{
    return m_displayList;
}

inline bool QGLSection::hasData(QLogicalVertex::Types types)
{
    return (types & m_dataTypes);
}

inline void QGLSection::append(const QLogicalVertex &vertex)
{
    if (m_smoothing == QGL::Smooth)
        appendSmooth(vertex);
    else
        appendFaceted(vertex);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const QGLSection &section);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLSECTION_H
