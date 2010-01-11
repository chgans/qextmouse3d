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

#ifndef QGLTEXTUREMODEL_H
#define QGLTEXTUREMODEL_H

#include "qt3dglobal.h"
#include "qlogicalvertex_p.h"

#include <QtCore/qrect.h>
#include <QtGui/qvector2d.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QGLTextureModel
{
public:
    QGLTextureModel();
    QGLTextureModel(qreal left, qreal bottom,
                               qreal width, qreal height);
    ~QGLTextureModel();
    inline QGLTextureModel(const QGLTextureModel &);
    inline QGLTextureModel &operator=(const QGLTextureModel &);
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
    QGLTextureModel *tileRight(qreal extent) const;
private:
    QVector2D m_bl;
    QVector2D m_br;
    QVector2D m_tr;
    QVector2D m_tl;
    qreal m_divisor;
    QGLTextureModel *m_tile;
};

inline QGLTextureModel::QGLTextureModel(const QGLTextureModel &other)
    : m_bl(other.m_bl)
    , m_br(other.m_br)
    , m_tr(other.m_tr)
    , m_tl(other.m_tl)
    , m_divisor(other.m_divisor)
    , m_tile(other.m_tile)
{
}

inline QGLTextureModel &QGLTextureModel::operator=(const QGLTextureModel &rhs)
{
    m_bl = rhs.m_bl;
    m_br = rhs.m_br;
    m_tr = rhs.m_tr;
    m_tl = rhs.m_tl;
    m_divisor = rhs.m_divisor;
    m_tile = rhs.m_tile;
    return *this;
}

inline QVector2D QGLTextureModel::bottomLeft() const
{
    return m_bl;
}

inline void QGLTextureModel::setBottomLeft(const QVector2D &v)
{
    m_bl = v;
}

inline QVector2D QGLTextureModel::bottomRight() const
{
    return m_br;
}

inline void QGLTextureModel::setBottomRight(const QVector2D &v)
{
    m_br = v;
}

inline QVector2D QGLTextureModel::topLeft() const
{
    return m_tl;
}

inline void QGLTextureModel::setTopLeft(const QVector2D &v)
{
    m_tl = v;
}

inline QVector2D QGLTextureModel::topRight() const
{
    return m_tr;
}

inline void QGLTextureModel::setTopRight(const QVector2D &v)
{
    m_tr = v;
}

inline bool QGLTextureModel::isNull() const
{
    return m_bl == QLogicalVertex::InvalidTexCoord &&
            m_br == QLogicalVertex::InvalidTexCoord &&
            m_tr == QLogicalVertex::InvalidTexCoord &&
            m_tl == QLogicalVertex::InvalidTexCoord;
}

inline void QGLTextureModel::startTileRight(qreal divisor)
{
    if (!m_tile)
        m_tile = new QGLTextureModel;
    m_divisor = divisor;
    m_tile->m_br = m_bl;
    m_tile->m_tr = m_tl;
}

inline QGLTextureModel *QGLTextureModel::tileRight(qreal extent) const
{
    if (!m_tile)
    {
#ifndef QT_NO_DEBUG
        qWarning("QGLTextureMODEL::tileRight() - call startTileRight() first!");
#endif
        return 0;
    }
    qreal factor = extent / m_divisor;
    m_tile->m_bl = m_tile->m_br;
    m_tile->m_tl = m_tile->m_tr;
    m_tile->m_br = m_tile->m_bl + ((m_br - m_bl) * factor);
    m_tile->m_tr = m_tile->m_tl + ((m_tr - m_tl) * factor);
    return m_tile;
}

#endif // QGLTEXTUREMODEL_H
