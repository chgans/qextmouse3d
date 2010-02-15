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

#ifndef QQBSPTREE_H
#define QQBSPTREE_H

#include "qarray.h"
#include "qbsptree_p.h"

#include <QtGui/qvector3d.h>
#include <QtCore/qdebug.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class tst_QBSPTree;

class Q_QT3D_EXPORT QBSPTree
{
public:
    enum Strategy {
        Balanced,
        AdHoc
    };

    explicit QBSPTree(const QArray<QVector3D> *v, Strategy strategy = AdHoc);
    ~QBSPTree();

    class const_iterator
    {
    public:
        const_iterator &operator++();
        int operator*() const;
        bool operator!=(const const_iterator &other) const;
        bool operator==(const const_iterator &other) const;
        QVector3D key() const;
        int value() const;
    private:
        const_iterator(const QBSP::Data *d, QBSP::Index p, bool fi = false);
        friend class QBSPTree;
        friend class tst_QBSPTree;

        const QBSP::Data *tree;
        QBSP::Index ptr;
        bool isFindIterator;
    };

    const_iterator constBegin() const;
    const_iterator constEnd() const;
    const_iterator constFind(const QVector3D &v) const;

    void insertMulti(const QVector3D &v, int i);
    void insert(const QVector3D &v, int i);

    void reserve(int amount);
    void dump() const;

protected:
    QBSP::Data *data() { return &d; }
    const QBSP::Data *data() const { return &d; }

private:

    QBSP::Data d;
};



inline QBSPTree::const_iterator QBSPTree::constBegin() const
{
    return const_iterator(&d, d.isEmpty() ? QBSP::MaxIndex : d.root());
}

inline QBSPTree::const_iterator QBSPTree::constEnd() const
{
    return const_iterator(&d, QBSP::MaxIndex);
}

inline void QBSPTree::reserve(int amount)
{
    d.reserve(amount);
}

inline QBSPTree::const_iterator QBSPTree::constFind(const QVector3D &v) const
{
    QBSP::Index ptr = d.root();
    QBSP::Partition part = QBSP::EqualTo;
    while (ptr != QBSP::MaxIndex)
    {
        part = QBSP::cmp(v, d.vectorData()->at(ptr));
        if (part == QBSP::EqualTo)
            break;
        ptr = d.pointerData()->at(ptr).next[part];
    }
    return const_iterator(&d, ptr, true);
}

inline void QBSPTree::insert(const QVector3D &v, int i)
{
    d.insert(v, i);
}

inline void QBSPTree::insertMulti(const QVector3D &v, int i)
{
    d.insert(v, i);
}

inline QBSPTree::const_iterator::const_iterator(const QBSP::Data *d, QBSP::Index p, bool fi)
    : tree(d)
    , ptr(p)
    , isFindIterator(fi)
{
}

inline QBSPTree::const_iterator::const_iterator &QBSPTree::const_iterator::operator++()
{
    if (ptr != QBSP::MaxIndex)
    {
        if (isFindIterator)
        {
            ptr = tree->pointerData()->at(ptr).next[QBSP::EqualTo];
        }
        else
        {
            if ((int)ptr < tree->pointerData()->size() - 1)
                ++ptr;
            else
                ptr = QBSP::MaxIndex;
        }
    }
    return *this;
}

inline int QBSPTree::const_iterator::operator*() const
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

inline int QBSPTree::const_iterator::value() const
{
    return ptr;
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QQBSPTree_H
