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

#ifndef QGLCOLORMODEL_H
#define QGLCOLORMODEL_H


#include "qt3dglobal.h"
#include "qcolor4b.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class Q_QT3D_EXPORT QGLColorModel
{
public:
    QGLColorModel();
    QGLColorModel(QColor4b left, QColor4b bottom,
                  QColor4b width, QColor4b height);
    ~QGLColorModel();
    inline QGLColorModel(const QGLColorModel &);
    inline QGLColorModel &operator=(const QGLColorModel &);
    inline QColor4b bottomLeft() const;
    inline void setBottomLeft(const QColor4b &);
    inline QColor4b bottomRight() const;
    inline void setBottomRight(const QColor4b &);
    inline QColor4b topLeft() const;
    inline void setTopLeft(const QColor4b &);
    inline QColor4b topRight() const;
    inline void setTopRight(const QColor4b &);
    inline void startGradientRight(qreal divisor);
    QGLColorModel *gradientRight(qreal extent) const;
    bool isNull() const { return m_null; }
private:
    QColor4b m_bl;
    QColor4b m_br;
    QColor4b m_tr;
    QColor4b m_tl;
    qreal m_divisor;
    QGLColorModel *m_gradient;
    bool m_null;
};

inline QGLColorModel::QGLColorModel(const QGLColorModel &other)
    : m_bl(other.m_bl)
    , m_br(other.m_br)
    , m_tr(other.m_tr)
    , m_tl(other.m_tl)
    , m_divisor(other.m_divisor)
    , m_gradient(other.m_gradient)
    , m_null(other.m_null)
{
}

inline QGLColorModel &QGLColorModel::operator=(const QGLColorModel &rhs)
{
    m_bl = rhs.m_bl;
    m_br = rhs.m_br;
    m_tr = rhs.m_tr;
    m_tl = rhs.m_tl;
    m_divisor = rhs.m_divisor;
    m_gradient = rhs.m_gradient;
    m_null = rhs.m_null;
    return *this;
}

inline QColor4b QGLColorModel::bottomLeft() const
{
    return m_bl;
}

inline void QGLColorModel::setBottomLeft(const QColor4b &v)
{
    m_null = false;
    m_bl = v;
}

inline QColor4b QGLColorModel::bottomRight() const
{
    return m_br;
}

inline void QGLColorModel::setBottomRight(const QColor4b &v)
{
    m_null = false;
    m_br = v;
}

inline QColor4b QGLColorModel::topLeft() const
{
    return m_tl;
}

inline void QGLColorModel::setTopLeft(const QColor4b &v)
{
    m_null = false;
    m_tl = v;
}

inline QColor4b QGLColorModel::topRight() const
{
    return m_tr;
}

inline void QGLColorModel::setTopRight(const QColor4b &v)
{
    m_null = false;
    m_tr = v;
}

inline void QGLColorModel::startGradientRight(qreal divisor)
{
    Q_ASSERT(!m_null);
    if (!m_gradient)
        m_gradient = new QGLColorModel;
    m_divisor = divisor;
    m_gradient->m_br = m_bl;
    m_gradient->m_tr = m_tl;
}

inline QColor4b qColorInterpolate(const QColor4b &a, const QColor4b &b, qreal factor)
{
    QColor ac = a.toColor();
    int ha, sa, va;
    ac.getHsv(&ha, &sa, &va);
    int hb, sb, vb;
    QColor bc = b.toColor();
    bc.getHsv(&hb, &sb, &vb);
    int hr = ha + ((hb - ha) * factor);
    int sr = sa + ((sb - sa) * factor);
    int vr = va + ((vb - va) * factor);
    return QColor4b(QColor::fromHsv(hr, sr, vr));
}

inline QGLColorModel *QGLColorModel::gradientRight(qreal extent) const
{
    if (!m_gradient)
    {
#ifndef QT_NO_DEBUG
        qWarning("QGLColorModel::tileRight() - call startTileRight() first!");
#endif
        return 0;
    }
    qreal factor = extent / m_divisor;
    m_gradient->m_bl = m_gradient->m_br;
    m_gradient->m_tl = m_gradient->m_tr;
    m_gradient->m_br = qColorInterpolate(m_gradient->m_bl, m_br, factor);
    m_gradient->m_tr = qColorInterpolate(m_gradient->m_tl, m_tr, factor);
    m_gradient->m_null = false;
    return m_gradient;
}

#endif // QGLCOLORMODEL_H
