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

#ifndef QQBSPTREE_P_H
#define QQBSPTREE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qvectorarray.h"

QT_BEGIN_NAMESPACE

namespace QBSP
{
    typedef quint16 Index;
    static const Index MaxIndex = UINT16_MAX;

    enum Partition {
        EqualTo,
        LessThanX,
        GreaterThanX,
        LessThanY,
        GreaterThanY,
        LessThanZ,
        GreaterThanZ,
        Stride
    };

    Partition cmp(const QVector3D &v, const QVector3D &w);

    struct NodeRef
    {
        Index ix;
        Partition part;
    };

    struct InsertRecord
    {
        int height;
        QVector3D vec;
        QBSP::Index vec_ix;
        QBSP::Index child;
        NodeRef chain[3];
        NodeRef *parent, *grand, *great;

        InsertRecord(const QVector3D &v, QBSP::Index vi,
                      QBSP::Index pointer, QBSP::Index new_index)
            : height(0)
            , vec(v)
            , vec_ix(vi)
            , child(new_index)
            , parent(&chain[0])
            , grand(&chain[1])
            , great(&chain[2])
        {
            parent->ix = pointer;
        }

        void shift_down(QBSP::Index ix, QBSP::Partition part)
        {
            ++height;
            NodeRef *temp = great;
            great = grand;
            grand = parent;
            parent = temp;
            parent->ix = ix;
            parent->part = part;
        }
    };

    struct Node
    {
        Index next[Stride];
    };

    class Data
    {
    public:
        Data(const QArray<QVector3D> *v);
        const QArray<QVector3D> *vectorData() const { return m_vec; }
        const QArray<Node> *pointerData() const { return &m_ptrs; }
        Index root() const { return m_root; }
        int height() const { return m_maxHeight; }
        bool isEmpty() const { return m_ptrs.isEmpty(); }
        void reserve(int amount) { m_ptrs.reserve(amount * Stride); }
        bool isBalanced() const { return m_balanced; }
        void setBalanced(bool bal) { m_balanced = bal; }

        void insert(const QVector3D &v, int i);
        void rotateLLorRR(const InsertRecord *);
        void rotateLRorRL(const InsertRecord *);
        void maybe_rebalance(const InsertRecord *);
        void recurseAndInsert(InsertRecord *);
        void recurseAndDump(Index index, int indent) const;

    private:
        static const Node Blank;
        static const Partition complement[Stride];

        QArray<Node> m_ptrs;
        const QArray<QVector3D> *m_vec;
        int m_maxHeight;
        Index m_root;
        bool m_balanced;
    };

};

inline QBSP::Partition QBSP::cmp(const QVector3D &v, const QVector3D &w)
{
    if (qFuzzyCompare(v.x(), w.x()))
    {
        if (qFuzzyCompare(v.y(), w.y()))
        {
            if (qFuzzyCompare(v.z(), w.z()))
            {
                return EqualTo;
            }
            else
            {
                if (v.z() < w.z())
                    return LessThanZ;
                else
                    return GreaterThanZ;
            }
        }
        else
        {
            if (v.y() < w.y())
                return LessThanY;
            else
                return GreaterThanY;
        }
    }
    else
    {
        if (v.x() < w.x())
            return LessThanX;
        else
            return GreaterThanX;
    }
}

QT_END_NAMESPACE

#endif    // QQBSPTREE_P_H
