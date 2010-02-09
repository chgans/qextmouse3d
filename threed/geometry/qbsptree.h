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

class Q_QT3D_EXPORT QBSPTree
{
public:
    explicit QBSPTree(const QArray<QVector3D> *v) : vec(v) {}
    ~QBSPTree() {}

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

    const QArray<QVector3D> *vectorData() const { return vec; }
    const QArray<BSPIndex> *pointerData() const { return &ptrs; }

    static Partition cmp(const QVector3D &v, const QVector3D &w)
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

    class const_iterator
    {
    public:
        const_iterator(const QBSPTree *d, BSPIndex p, bool fi = false)
            : tree(d)
            , ptr(p)
            , isFindIterator(fi) { }
        const_iterator &operator++()
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
        int operator*() const
        {
            return ptr;
        }
        bool operator!=(const const_iterator &other) const
        {
            return (ptr != other.ptr || tree != other.tree);
        }
        bool operator==(const const_iterator &other) const
        {
            return (ptr == other.ptr && tree == other.tree);
        }
        QVector3D key() const
        {
            return tree->vectorData()->at(ptr);
        }
        BSPIndex value() const
        {
            return ptr;
        }
    private:
        const QBSPTree *tree;
        BSPIndex ptr;
        bool isFindIterator;
    };

    const_iterator constBegin() const
    {
        return const_iterator(this, ptrs.isEmpty() ? MaxIndex : 0);
    }

    const_iterator constEnd() const
    {
        return const_iterator(this, MaxIndex);
    }

    void reserve(int amount)
    {
        ptrs.reserve(amount * Stride);
    }

    const_iterator constFind(const QVector3D &v) const
    {
        BSPIndex ptr = 0;
        Partition part = EqualTo;
        while (ptr != MaxIndex)
        {
            part = cmp(v, vec->at(ptr));
            if (part == EqualTo)
                break;
            ptr = ptrs[ptr * Stride + part];
        }
        return const_iterator(this, ptr, true);
    }

    void recurseAndInsert(const QVector3D &v, int i, BSPIndex ptr, BSPIndex new_ix)
    {
        Q_ASSERT(ptr != new_ix);
        Partition part = cmp(v, vec->at(ptr));
        BSPIndex next = ptrs[ptr * Stride + part];
        /*
        qDebug() << "    recurseAndInsert(" << v << ", " << i << ", " << ptr
                << ", " << new_ix << ")"
                << "     part:" << part << "next:" << next;
        dump();
        */
        if (next != MaxIndex)
        {
            recurseAndInsert(v, i, next, new_ix);
        }
        else
        {
            ptrs.append(blank, Stride);
            ptrs[ptr * Stride + part] = new_ix;
        }
        //dump();
    }

    void insertMulti(const QVector3D &v, int i)
    {
        insert(v, i);
    }

    void insert(const QVector3D &v, int i)
    {
        // use MaxIndex as a sentinel so actually fit up to MaxIndex - 1
        if (static_cast<BSPIndex>(ptrs.size()) < (MaxIndex - 1) * Stride)
        {
            if (ptrs.size() > 0)
            {
                Q_ASSERT(vec->at(i) == v);
                BSPIndex ptr = 0;
                BSPIndex new_ix = ptrs.size() / Stride;
                recurseAndInsert(v, i, ptr, new_ix);
            }
            else
            {
                ptrs.append(blank, Stride);
            }
        }
#ifndef QT_NO_DEBUG
        else
        {
            qWarning("Overflowed QBSPTree");
        }
#endif
    }

#ifndef QT_NO_DEBUG
    void dump();
#endif

private:
    static const BSPIndex blank[Stride];
    QArray<quint32> ptrs;
    const QArray<QVector3D> *vec;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QQBSPTree_H
