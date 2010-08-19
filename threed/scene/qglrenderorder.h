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


#ifndef QGLRENDERORDER_H
#define QGLRENDERORDER_H

#include "qglscenenode.h"
#include "qglrenderstate.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)


struct RenderOrderKey
{
    RenderOrderKey() : node(0), state(QGLRenderState()) {}
    RenderOrderKey(const QGLSceneNode *n, const QGLRenderState &s)
        : node(n), state(s) {}
    bool valid() const { return node != 0; }

    const QGLSceneNode *node;
    QGLRenderState state;
};


class Q_QT3D_EXPORT QGLRenderOrder
{
public:
    explicit QGLRenderOrder(const RenderOrderKey &key) : m_key(key) { }
    virtual ~QGLRenderOrder() {}

    virtual uint effectHash() const;
    virtual bool isEqual(const QGLRenderOrder &rhs) const;
    virtual bool isLessThan(const QGLRenderOrder &rhs) const;

    inline bool operator!=(const QGLRenderOrder &rhs) const;
    inline bool operator==(const QGLRenderOrder &rhs) const;
    inline bool operator<(const QGLRenderOrder &rhs) const;

    inline const QGLSceneNode *node() const;
    inline void setState(const QGLRenderState &state);
    inline QGLRenderState state() const;
    inline RenderOrderKey key() const;
    inline QGLAbstractEffect *effectiveUserEffect() const;
    inline QGL::StandardEffect effectiveStandardEffect() const;
    inline QGLMaterial *effectiveMaterial() const;
    inline QGLMaterial *effectiveBackMaterial() const;
    inline bool effectiveHasEffect() const;
private:
    RenderOrderKey m_key;
};


inline bool QGLRenderOrder::operator!=(const QGLRenderOrder &rhs) const
{
    return !isEqual(rhs);
}

inline bool QGLRenderOrder::operator==(const QGLRenderOrder &rhs) const
{
    return isEqual(rhs);
}

inline bool QGLRenderOrder::operator<(const QGLRenderOrder &rhs) const
{
    return isLessThan(rhs);
}

inline const QGLSceneNode *QGLRenderOrder::node() const
{
    return m_key.node;
}

inline QGLRenderState QGLRenderOrder::state() const
{
    return m_key.state;
}

inline void QGLRenderOrder::setState(const QGLRenderState &state)
{
    m_key.state = state;
}

inline RenderOrderKey QGLRenderOrder::key() const
{
    return m_key;
}

inline QGLAbstractEffect *QGLRenderOrder::effectiveUserEffect() const
{
    QGLAbstractEffect *result = 0;
    if (m_key.node->userEffect())
        result = m_key.node->userEffect();
    else if (m_key.state.userEffect())
        result = m_key.state.userEffect();
    return result;
}

inline QGL::StandardEffect QGLRenderOrder::effectiveStandardEffect() const
{
    QGL::StandardEffect result = QGL::FlatColor;
    if (m_key.node->hasEffect())
        result = m_key.node->effect();
    else if (m_key.state.hasEffect())
        result = m_key.state.standardEffect();
    return result;
}

inline QGLMaterial *QGLRenderOrder::effectiveMaterial() const
{
    QGLMaterial *result = 0;
    if (m_key.node->material())
        result = m_key.node->material();
    else if (m_key.state.material())
        result = m_key.state.material();
    return result;
}

inline QGLMaterial *QGLRenderOrder::effectiveBackMaterial() const
{
    QGLMaterial *result = 0;
    if (m_key.node->backMaterial())
        result = m_key.node->backMaterial();
    else if (m_key.state.backMaterial())
        result = m_key.state.backMaterial();
    return result;
}

inline bool QGLRenderOrder::effectiveHasEffect() const
{
    bool result = false;
    if (m_key.node->hasEffect())
        result = true;
    else
        result = m_key.state.hasEffect();
    return result;
}

inline uint qHash(const QGLRenderOrder &order)
{
    quint64 result = order.effectHash();
    return result ^ reinterpret_cast<quint64>(order.effectiveMaterial());
}

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLRenderOrder &order);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLRENDERORDER_H
