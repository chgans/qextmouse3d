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

#include <QtDeclarative/qdeclarative.h>

#if defined(QML_VERSION) && QML_VERSION >= 0x020000

#include "floatingitemnode_sg.h"
//#include <QtDeclarative/qsgstereocontext.h>
#include <QtDeclarative/qsgmatrix4x4stack.h>

QT_BEGIN_NAMESPACE

FloatingItemSGNode::FloatingItemSGNode(QSGContext *context)
    //: m_context(qobject_cast<QSGStereoContext *>(context)), m_depth(0.0f)
    : m_context(0), m_depth(0.0f)
{
    Q_UNUSED(context);
    setFlag(Node::UsePreprocess, true);
}

FloatingItemSGNode::~FloatingItemSGNode()
{
}

void FloatingItemSGNode::setDepth(qreal depth)
{
    m_depth = depth;
}

void FloatingItemSGNode::preprocess()
{
#if 0
    QSGStereoContext::Eye eye;
    m_adjustMatrix.setToIdentity();
    eye = m_context ? m_context->eye() : QSGStereoContext::NoEye;
    if (eye != QSGStereoContext::NoEye && m_depth != 0.0f) {
        // Correct the depth value for the screen's DPI.  We treat 100 DPI
        // as "normal" and scale the depth value accordingly.  This way,
        // the same number of millimeters are used on all displays viewed
        // at the same viewing distance.  A depth of 1 is 0.254 millimeters.
        // Note: we should probably correct for viewing distance also,
        // but that is harder to determine.
        qreal depth = m_depth * m_context->glContext()->device()->logicalDpiX() / 100.0f;

        // Determine the transformation to pre-multiply with the modelview.
        if (eye == QSGStereoContext::LeftEye)
            m_adjustMatrix.translate(depth / 2.0f, 0.0f, 0.0f);
        else
            m_adjustMatrix.translate(-depth / 2.0f, 0.0f, 0.0f);
    }
#endif
    markDirty(DirtyMatrix);
}

void FloatingItemSGNode::applyTo(QSGMatrix4x4Stack *stack)
{
    *stack = m_adjustMatrix * stack->top() * matrix();
}

QT_END_NAMESPACE

#endif // QML_VERSION >= 0x020000
