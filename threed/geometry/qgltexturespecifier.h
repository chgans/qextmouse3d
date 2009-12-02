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
** This file contains pre-release code and may not be dism_tributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Pum_blic License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Pum_blic License version 2.1 as pum_blished by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Pum_blic License version 2.1 requirements
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

#ifndef QGLTEXTURESPECIFIER_H
#define QGLTEXTURESPECIFIER_H

#include "qt3dglobal.h"
#include "qlogicalvertex.h"

#include <QtCore/qrect.h>
#include <QtGui/qvector2d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QGLTextureSpecifier
{
public:
    QGLTextureSpecifier();
    QGLTextureSpecifier(qreal left, qreal bottom,
                               qreal width, qreal height);
    ~QGLTextureSpecifier();
    inline QGLTextureSpecifier(const QGLTextureSpecifier &);
    inline QGLTextureSpecifier &operator=(const QGLTextureSpecifier &);
    inline QVector2D bottomLeft() const;
    inline void setBottomLeft(const QVector2D &);
    inline QVector2D bottomRight() const;
    inline void setBottomRight(const QVector2D &);
    inline QVector2D topLeft() const;
    inline void setTopLeft(const QVector2D &);
    inline QVector2D topRight() const;
    inline void setTopRight(const QVector2D &);
    inline bool isNull() const;
    inline void startTileRight(qreal divisor);
    QGLTextureSpecifier tileRight(qreal extent) const;
private:
    QVector2D m_bl;
    QVector2D m_br;
    QVector2D m_tr;
    QVector2D m_tl;
    qreal m_divisor;
    QGLTextureSpecifier *m_tile;
};

inline QGLTextureSpecifier::QGLTextureSpecifier(const QGLTextureSpecifier &other)
    : m_bl(other.m_bl)
    , m_br(other.m_br)
    , m_tr(other.m_tr)
    , m_tl(other.m_tl)
    , m_divisor(other.m_divisor)
    , m_tile(other.m_tile)
{
}

inline QGLTextureSpecifier &QGLTextureSpecifier::operator=(const QGLTextureSpecifier &rhs)
{
    m_bl = rhs.m_bl;
    m_br = rhs.m_br;
    m_tr = rhs.m_tr;
    m_tl = rhs.m_tl;
    m_divisor = rhs.m_divisor;
    m_tile = rhs.m_tile;
    return *this;
}

inline QVector2D QGLTextureSpecifier::bottomLeft() const
{
    return m_bl;
}

inline void QGLTextureSpecifier::setBottomLeft(const QVector2D &v)
{
    m_bl = v;
}

inline QVector2D QGLTextureSpecifier::bottomRight() const
{
    return m_br;
}

inline void QGLTextureSpecifier::setBottomRight(const QVector2D &v)
{
    m_br = v;
}

inline QVector2D QGLTextureSpecifier::topLeft() const
{
    return m_tl;
}

inline void QGLTextureSpecifier::setTopLeft(const QVector2D &v)
{
    m_tl = v;
}

inline QVector2D QGLTextureSpecifier::topRight() const
{
    return m_tr;
}

inline void QGLTextureSpecifier::setTopRight(const QVector2D &v)
{
    m_tr = v;
}

inline bool QGLTextureSpecifier::isNull() const
{
    return m_bl == QLogicalVertex::InvalidTexCoord &&
            m_br == QLogicalVertex::InvalidTexCoord &&
            m_tr == QLogicalVertex::InvalidTexCoord &&
            m_tl == QLogicalVertex::InvalidTexCoord;
}

inline void QGLTextureSpecifier::startTileRight(qreal divisor)
{
    if (!m_tile)
        m_tile = new QGLTextureSpecifier;
    m_divisor = divisor;
    m_tile->m_br = m_bl;
    m_tile->m_tr = m_tl;
}

inline QGLTextureSpecifier QGLTextureSpecifier::tileRight(qreal extent) const
{
    if (!m_tile)
    {
#ifndef QT_NO_DEBUG
        qWarning("QGLTextureSpecifier::tileRight() - call startTileRight() first!");
#endif
        return QGLTextureSpecifier();
    }
    qreal factor = extent / m_divisor;
    m_tile->m_bl = m_tile->m_br;
    m_tile->m_tl = m_tile->m_tr;
    m_tile->m_br = m_tile->m_bl + ((m_br - m_bl) * factor);
    m_tile->m_tr = m_tile->m_tl + ((m_tr - m_tl) * factor);
    return *m_tile;
}

#endif // QGLTEXTURESPECIFIER_H
