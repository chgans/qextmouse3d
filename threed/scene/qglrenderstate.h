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


#ifndef QGLRENDERSTATE_H
#define QGLRENDERSTATE_H

#include "qglnamespace.h"
#include "qglscenenode.h"

#include <QtCore/qshareddata.h>

class QGLAbstractEffect;
class QGLMaterial;

class QGLRenderStatePrivate;

class Q_QT3D_EXPORT QGLRenderState
{
public:
    QGLRenderState();
    QGLRenderState(const QGLRenderState &other);
    virtual ~QGLRenderState();
    QGLRenderState &operator=(const QGLRenderState &rhs);
    virtual void updateFrom(const QGLSceneNode *node);
    QGLAbstractEffect *userEffect() const;
    QGL::StandardEffect standardEffect() const;
    bool hasEffect() const;
    QGLMaterial *material() const;
    QGLMaterial *backMaterial() const;
    const QGLSceneNode *node() const;
    uint hash() const;
    bool operator==(const QGLRenderState &rhs) const
    {
        if (userEffect() != rhs.userEffect())
            return false;
        if (standardEffect() != rhs.standardEffect())
            return false;
        if (hasEffect() != rhs.hasEffect())
            return false;
        if (material() != rhs.material())
            return false;
        if (backMaterial() != rhs.backMaterial())
            return false;
        return true;
    }
    bool isValid() const;
private:
    void detach();
    QGLRenderStatePrivate *d;
};

inline uint qHash(const QGLRenderState &s)
{
    return s.hash();
}

#ifndef QT_NO_DEBUG_STREAM
#include <QtCore/qdebug.h>
Q_QT3D_EXPORT QDebug operator<<(QDebug dbg, const QGLRenderState &order);
#endif

#endif // QGLRENDERSTATE_H
