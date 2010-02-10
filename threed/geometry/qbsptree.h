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

#ifndef QQBSPTree_H
#define QQBSPTree_H

#include "qarray.h"

#include <QtGui/qvector3d.h>
#include <QtCore/qdebug.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class insert_record;

class Q_QT3D_EXPORT QBSPTree
{
public:
    explicit QBSPTree(const QArray<QVector3D> *v);
    ~QBSPTree();

    typedef quint32 BSPIndex;
    static const BSPIndex MaxIndex;

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

    const QArray<QVector3D> *vectorData() const { return m_vec; }
    const QArray<BSPIndex> *pointerData() const { return &m_ptrs; }
    BSPIndex root() const { return m_root; }

    static Partition cmp(const QVector3D &v, const QVector3D &w);

    class const_iterator
    {
    public:
        const_iterator(const QBSPTree *d, BSPIndex p, bool fi = false);
        const_iterator &operator++();
        BSPIndex operator*() const;
        bool operator!=(const const_iterator &other) const;
        bool operator==(const const_iterator &other) const;
        QVector3D key() const;
        BSPIndex value() const;
    private:
        const QBSPTree *tree;
        BSPIndex ptr;
        bool isFindIterator;
    };

    const_iterator constBegin() const;
    const_iterator constEnd() const;
    void reserve(int amount);
    const_iterator constFind(const QVector3D &v) const;
    void insertMulti(const QVector3D &v, int i);
    void insert(const QVector3D &v, int i);

    void dump() const;

private:
    void rotateLLorRR(const insert_record *);
    void rotateLRorRL(const insert_record *);
    void maybe_rebalance(const insert_record *);
    void recurseAndInsert(insert_record *);
    void recurseAndDump(BSPIndex index, int indent) const;

    static const BSPIndex blank[Stride];
    static const Partition complement[Stride];

    QArray<quint32> m_ptrs;
    const QArray<QVector3D> *m_vec;
    int m_maxHeight;
    BSPIndex m_root;
};


inline QBSPTree::Partition QBSPTree::cmp(const QVector3D &v, const QVector3D &w)
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

inline QBSPTree::const_iterator QBSPTree::constBegin() const
{
    return const_iterator(this, m_ptrs.isEmpty() ? MaxIndex : m_root);
}

inline QBSPTree::const_iterator QBSPTree::constEnd() const
{
    return const_iterator(this, MaxIndex);
}

inline void QBSPTree::reserve(int amount)
{
    m_ptrs.reserve(amount * Stride);
}

inline QBSPTree::const_iterator QBSPTree::constFind(const QVector3D &v) const
{
    BSPIndex ptr = 0;
    Partition part = EqualTo;
    while (ptr != MaxIndex)
    {
        part = cmp(v, m_vec->at(ptr));
        if (part == EqualTo)
            break;
        ptr = m_ptrs[ptr * Stride + part];
    }
    return const_iterator(this, ptr, true);
}

inline QBSPTree::insertMulti(const QVector3D &v, int i)
{
    insert(v, i);
}

inline QBSPTree::const_iterator::const_iterator(const QBSPTree *d, BSPIndex p, bool fi)
    : tree(d)
    , ptr(p)
    , isFindIterator(fi)
{
}

inline QBSPTree::const_iterator::const_iterator &QBSPTree::const_iterator::operator++()
{
    if (ptr != MaxIndex)
    {
        if (isFindIterator)
        {
            ptr = tree->pointerData()->at(ptr * Stride + EqualTo);
        }
        else
        {
            if ((ptr+1) * Stride != static_cast<BSPIndex>(
                    tree->pointerData()->size()))
                ++ptr;
            else
                ptr = MaxIndex;
        }
    }
    return *this;
}

inline QBSPTree::BSPIndex QBSPTree::const_iterator::operator*() const
{
    return ptr;
}

inline bool QBSPTree::const_iterator::operator!=(const const_iterator &other) const
{
    return (ptr != other.ptr || tree != other.tree);
}

inline bool QBSPTree::const_iterator::operator==(const const_iterator &other) const
{
    return (ptr == other.ptr && tree == other.tree);
}

inline QVector3D QBSPTree::const_iterator::key() const
{
    return tree->vectorData()->at(ptr);
}

inline QBSPTree::BSPIndex QBSPTree::const_iterator::value() const
{
    return ptr;
}



QT_END_NAMESPACE

QT_END_HEADER

#endif // QQBSPTree_H
