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

#ifndef FLOATINGITEMNODE_SG_H
#define FLOATINGITEMNODE_SG_H

#include <QtDeclarative/qdeclarative.h>

#if defined(QML_VERSION) && QML_VERSION >= 0x020000

#include <QtDeclarative/node.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSGContext;
class QSGStereoContext;

class FloatingItemSGNode : public TransformNode
{
public:
    FloatingItemSGNode(QSGContext *);
    ~FloatingItemSGNode();

    void setDepth(qreal depth);
    inline qreal depth() const { return m_depth; }

    void preprocess();
    void applyTo(QSGMatrix4x4Stack *stack);

private:
    QSGStereoContext *m_context;
    qreal m_depth;
    QMatrix4x4 m_adjustMatrix;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QML_VERSION >= 0x020000

#endif // FLOATINGITEMNODE_SG_H
