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

#include <stdlib.h>

#include "qbsptree.h"

/*!
    \class QBSPTree
    \brief The QBSPTree class maps QVector3D keys to scalar index values.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    The QBSPTree class is intended for fast lookup of indexes, from their
    QVector3D keys using a binary-space-partition method.

    Since typical applications will already be storing QVector3D values the
    QBSPTree does not create its own duplicates of this data, and instead its
    constructor takes a pointer to a QVector3DArray which will contain all
    the QVector3D data.  The QSPTree maintains indexes to the data.

    The insert() function takes a QVector3D coordinate and its index and
    stores this mapping in the QBSPTree.  The method of storage uses a tree
    structure based on the x, y and z values of the coordinate to divided
    the search space into partitions.  A self-balancing algorithm is used
    to ensure that key-order insertions (vector data inserted left-to-right,
    top-to-bottom, front-to-back) does not cause degenerate tree structures.

    The constFind() function takes a QVector3D value and by following the
    tree according to the x, y and z values in the coordinate quickly locates
    the resulting entry.

    The tree can store up to QBSP::MaxIndex - 1 values - currently this
    limit is 65534 entries.

    At present many typical data-structure functions are not supported in
    QBSPTree, but may be added in the future.  It is a simple, highly-
    optimised structure tailored for insert and search of QVector3D values.

    \sa QBSPTree::const_iterator
*/

/*
    A blank QBSPTree entry, used for quickly creating new entries.
*/
const QBSP::Node QBSP::Data::Blank = {{
    QBSP::MaxIndex,
    QBSP::MaxIndex,
    QBSP::MaxIndex,
    QBSP::MaxIndex,
    QBSP::MaxIndex,
    QBSP::MaxIndex,
    QBSP::MaxIndex
}};

/*
    Mapping of partitions to their complements; for example the entry
    at 1 - that is QBSPTree::LessThanX - is QBSPTree::GreaterThanX.
    This is used in tree balancing.
*/
const QBSP::Partition QBSP::Data::complement[QBSP::Stride] = {
    EqualTo,
    GreaterThanX,
    LessThanX,
    GreaterThanY,
    LessThanY,
    GreaterThanZ,
    LessThanZ
};

/*!
    \class QBSPTree::const_iterator
    \brief The const_iterator class is an iterator over a const QBSPTree.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    QBSPTree functions constFind() and constBegin() return iterator instances
    which can be used to retrieve all relevant values.

    Use the \c{*} operator or value() function to return the current entry
    pointed to by this iterator - a QBSP::Index value.  Use the key()
    function to return the key for the current entry - a QVector3D value.

    \sa QBSPTree
*/

const char *QBSP::InsertRecord::partition_names[] = {
    "EqualTo",
    "LessThanX",
    "GreaterThanX",
    "LessThanY",
    "GreaterThanY",
    "LessThanZ",
    "GreaterThanZ"
};

/*!
    \internal
    \fn QBSPTree::const_iterator::const_iterator(const QBSP::Data *d, QBSP::Index p, bool fi)
    Constructs a new iterator at \a p over a const QBPTree \a d.  If \a fi is true
    (the default) then this is a find iterator (which will return all entries which
    are equal to the one at \a p), otherwise it is a sequential iterator (which will
    return all entries from \a p to the end).

    Generally use QBSPTree::constBegin() or QBSPTree::constFind() to create new
    iterators.  This function is intended for internal use.
*/

/*!
    \fn QBSPTree::const_iterator::const_iterator &QBSPTree::const_iterator::operator++()
    Increment this iterator to point at the next entry.  In the case of a find
    iterator the next entry will be one equal to the current one.  In the case
    of a sequence iterator it will be next in-order in the tree.  If there is
    no such entry, this iterator will point at QBSPTree::constEnd().  Returns a
    reference to this iterator.
*/

/*!
    \fn QBSP::Index QBSPTree::const_iterator::operator*() const
    Dereference this iterator to find its value.

    \sa value()
*/

/*!
    \fn bool QBSPTree::const_iterator::operator!=(const const_iterator &other) const
    Returns true if this iterator is not equal to the \a other, false
    otherwise.
*/

/*!
    \fn bool QBSPTree::const_iterator::operator==(const const_iterator &other) const
    Returns true if this iterator is equal to the \a other, false
    otherwise.
*/

/*!
    \fn QVector3D QBSPTree::const_iterator::key() const
    Returns the key associated with this iterator.
*/

/*!
    \fn QBSP::Index QBSPTree::const_iterator::value() const
    Returns the value associated with this iterator.
*/
/*!
    \enum QBSPTree::Strategy

    This enum allows for tuning of the strategy used during insertions into
    the QBSPTree.  Balancing the tree during insertions costs more time when
    the tree is being built, but can reduce time for find operations.  If a
    tree is being created in key order, that is with vertices generated from
    high to low X, Y and Z values, the tree can degenerate to approximate a
    linked list structure and thus perform poorly - select Balanced in this
    case.  Generally however data can be presumed to be random, or nearly
    random and the default QBSPTree constructor setting of AdHoc is best.

    \value Balanced Optimize during insertion with tree balancing operations.
    \value AdHoc Allow the tree to grow as dictated by key order of the data.
*/

/*!
    Constructs a new QBSPTree instance based on the QVector3DArray \a v,
    and with the specified insertion \a strategy (which defaults to
    QBSPTree::AdHoc).
*/
QBSPTree::QBSPTree(const QArray<QVector3D> *v, QBSPTree::Strategy strategy)
    : d(v)
{
    d.setBalanced(strategy == Balanced);
}

/*!
    Destroys this QBSPTree instance recovering any resources.
*/
QBSPTree::~QBSPTree()
{
}

/*!
    This debugging aid prints out an indented representation of the
    QBSPTree and its QVector3D data.
*/
void QBSPTree::dump() const
{
    fprintf(stderr, "===== QBSPTree - %p - count: %d - height: %d ====\n",
            this, d.pointerData()->size(), d.height());
    d.recurseAndDump(d.root(), 0);
}

/*!
    \fn QBSP::Data *QBSPTree::data()
    This advanced function is mainly intended for testing use.
*/

/*!
    \fn const QBSP::Data *QBSPTree::data() const
    \overload
    This advanced function is mainly intended for testing use.
*/

/*!
    \fn QBSPTree::const_iterator QBSPTree::constBegin() const
    Returns an iterator pointing to the beginning (the root node) of
    this tree.  Incrementing this iterator will result in visiting
    all nodes of the tree in an undefined order.
*/

/*!
    \fn QBSPTree::const_iterator QBSPTree::constEnd() const
    Returns an iterator pointing past the end of
    this tree.  Dereferencing this iterator will result in undefined
    behaviour (generally an assert when in debug mode).
*/

/*!
    \fn void QBSPTree::reserve(int amount)
    Reserves enough space to fit \a amount indexes.  If this function
    is called prior to adding a large number of indexes is may result
    in faster insertion by avoiding the realloc overhead.
*/

/*!
    \fn const_iterator QBSPTree::constFind(const QVector3D &v) const
    Searches the tree for the first node with key \a v and returns an
    iterator pointing to it, if found; or otherwise constEnd().

    To retrieve \bold{all} nodes with that key, increment the iterator:
    \code
    QBSPTree::const_iterator it = tree.constFind(vector);
    for ( ; it != tree.constEnd(); ++it)
        doSomethingWithIndex(*it);
    \endcode
*/

QBSP::Data::Data(const QArray<QVector3D> *v)
    : m_vec(v)
    , m_maxHeight(0)
    , m_root(0)
{
}

inline static bool isLeft(QBSP::Partition part)
{
    return part % 2;
}

void QBSP::Data::rotateLLorRR(const QBSP::InsertRecord *bal)
{
    QBSP::Index i = bal->grand->ix;
    Partition p = bal->parent->part;
    Q_ASSERT(p != QBSP::EqualTo);
    m_ptrs[i].next[p] = QBSP::MaxIndex;
    i = bal->parent->ix;
    p = complement[bal->parent->part];
    m_ptrs[i].next[p] = bal->grand->ix;
    if (bal->height == 3)
    {
        m_root = bal->parent->ix;
    }
    else
    {
        i = bal->great->ix;
        p = bal->grand->part;
        Q_ASSERT(p != QBSP::EqualTo);
        m_ptrs[i].next[p] = bal->parent->ix;
    }
}

void QBSP::Data::rotateLRorRL(const QBSP::InsertRecord *bal)
{
    QBSP::Index i = bal->grand->ix;
    Partition p = bal->parent->part;
    Q_ASSERT(p != QBSP::EqualTo);
    m_ptrs[i].next[p] = QBSP::MaxIndex;
    i = bal->parent->ix;
    p = bal->child->part;
    Q_ASSERT(p != QBSP::EqualTo);
    m_ptrs[i].next[p] = QBSP::MaxIndex;
    i = bal->child->ix;
    p = complement[bal->child->part];
    m_ptrs[i].next[p] = bal->parent->ix;
    p = complement[bal->parent->part];
    m_ptrs[i].next[p] = bal->grand->ix;
    if (bal->height == 3)
    {
        m_root = bal->child->ix;
    }
    else
    {
        i = bal->great->ix;
        p = bal->grand->part;
        Q_ASSERT(p != QBSP::EqualTo);
        m_ptrs[i].next[p] = bal->child->ix;
    }
}

bool QBSP::Data::maybe_rebalance(const QBSP::InsertRecord *bal)
{
    bool doBalance = false;
    if (m_balanced && bal->height > 2)
    {
        if (m_ptrs[bal->parent->ix].branchingFactor() < 2 &&
            m_ptrs[bal->grand->ix].branchingFactor() < 2)
        {
            doBalance = true;
            if (isLeft(bal->grand->part))
            {
                if (isLeft(bal->parent->part))
                    rotateLLorRR(bal);   // LL case
                else
                    rotateLRorRL(bal);   // LR case
            }
            else
            {
                if (isLeft(bal->parent->part))
                    rotateLRorRL(bal);   // RL case
                else
                    rotateLLorRR(bal);   // RR case
            }
        }
    }
    return doBalance;
}

void QBSP::Data::recurseAndInsert(QBSP::InsertRecord *bal)
{
    Partition part = cmp(bal->vec, m_vec->at(bal->parent->ix));
    QBSP::Index next = m_ptrs[bal->parent->ix].next[part];
    if (next != QBSP::MaxIndex)
    {
        bal->shift_down(next, part);
        recurseAndInsert(bal);
    }
    else
    {
        ++bal->height;
        m_ptrs.append(Blank);
        m_ptrs[bal->parent->ix].next[part] = bal->child->ix;
        bal->child->part = part;
        if (maybe_rebalance(bal))
            --bal->height;
    }
}

/*!
    \fn void QBSPTree::insertMulti(const QVector3D &v, int i)
    This function simply calls insert() with arguments \a v and \a i.
    It exists purely for compatibility with other containers which
    can map multiple values to the same key.
*/

/*!
    \fn void QBSPTree::insert(const QVector3D &v, int i)
    Inserts into the tree the QVector3D \a v as a key mapping to
    the value \a i.  The value \a i must be the index of \a v in
    the underlying QVector3DArray passed to the QBSPTree's constructor.
*/

void QBSP::Data::insert(const QVector3D &v, int i)
{
    Q_ASSERT(m_vec->at(i) == v);
    if (m_ptrs.size() < (QBSP::MaxIndex - 1))
    {
        if (m_ptrs.size() > 0)
        {
            Index new_ix = m_ptrs.size();
            InsertRecord bal(v, i, m_root, new_ix);
            recurseAndInsert(&bal);
            if (m_maxHeight < bal.height)
                m_maxHeight = bal.height;
        }
        else
        {
            m_ptrs.append(Blank);
            m_maxHeight = 1;
        }
    }
#ifndef QT_NO_DEBUG
    else
    {
        qWarning("Overflowed QBSPTree");
    }
#endif
}

void QBSP::Data::recurseAndDump(QBSP::Index ix, int indent) const
{
    char ind[1024];
    qMemSet(ind, '\0', 1024);
    qMemSet(ind, ' ', indent < 341 ? indent * 3 : 1023);
    fprintf(stderr, "%sNode: %d - QVector3D(%0.3f, %0.3f, %0.3f)\n",
            ind, ix, m_vec->at(ix).x(), m_vec->at(ix).y(), m_vec->at(ix).z());
    fprintf(stderr, "%s      Children", ind);
    QArray<QBSP::Index> children;
    if (m_ptrs.at(ix).next[EqualTo] != QBSP::MaxIndex)
    {
        fprintf(stderr, " -- EqualTo: %d", m_ptrs.at(ix).next[EqualTo]);
        children << m_ptrs.at(ix).next[EqualTo];
    }

    if (m_ptrs.at(ix).next[LessThanX] != QBSP::MaxIndex)
    {
        fprintf(stderr, " -- LessThanX: %d", m_ptrs.at(ix).next[LessThanX]);
        children << m_ptrs.at(ix).next[LessThanX];
    }

    if (m_ptrs.at(ix).next[GreaterThanX] != QBSP::MaxIndex)
    {
        fprintf(stderr, " -- GreaterThanX: %d", m_ptrs.at(ix).next[GreaterThanX]);
        children << m_ptrs.at(ix).next[GreaterThanX];
    }

    if (m_ptrs.at(ix).next[LessThanY] != QBSP::MaxIndex)
    {
        fprintf(stderr, " -- LessThanY: %d", m_ptrs.at(ix).next[LessThanY]);
        children << m_ptrs.at(ix).next[LessThanY];
    }

    if (m_ptrs.at(ix).next[GreaterThanY] != QBSP::MaxIndex)
    {
        fprintf(stderr, " -- GreaterThanY: %d", m_ptrs.at(ix).next[GreaterThanY]);
        children << m_ptrs.at(ix).next[GreaterThanY];
    }

    if (m_ptrs.at(ix).next[LessThanZ] != QBSP::MaxIndex)
    {
        fprintf(stderr, " -- LessThanZ: %d", m_ptrs.at(ix).next[LessThanZ]);
        children << m_ptrs.at(ix).next[LessThanZ];
    }

    if (m_ptrs.at(ix).next[GreaterThanZ] != QBSP::MaxIndex)
    {
        fprintf(stderr, " -- GreaterThanZ: %d", m_ptrs.at(ix).next[GreaterThanZ]);
        children << m_ptrs.at(ix).next[GreaterThanZ];
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < children.count(); ++i)
        recurseAndDump(children[i], indent + 1);
}
