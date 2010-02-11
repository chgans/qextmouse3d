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

    The tree can store up to QBSPTree::MaxIndex - 1 values - currently this
    limit is 65534 entries.

    At present many typical data-structure functions are not supported in
    QBSPTree, but may be added in the future.  It is a simple, highly-
    optimised structure tailored for insert and search of QVector3D values.

    \sa QBSPTree::const_iterator
*/

/*!
    \internal
    A blank QBSPTree entry, used for quickly creating new entries.
*/
const QBSPTree::BSPIndex QBSPTree::blank[Stride] = {
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex,
    MaxIndex
};

/*!
    \typedef QBSPTree::BSPIndex
    Synonym for the type of indexes used by QBSPTree.  Currently this is
    a 16-bit unsigned integer, which has a maximum value of 65535.
*/

/*!
    Constant representing the theoretical maximum index value, used as
    a flag value to indicate null indices.  This value is dictated by
    the size of the data type QBSPTree::BSPIndex.  The actual highest
    possible index in a QBSPTree is one less than this number.
*/
const QBSPTree::BSPIndex QBSPTree::MaxIndex = UINT16_MAX;

/*!
    \enum Partition

    This enum manages the offsets for the partitions within the
    QBSPTree's index data.  Use of this enum is for advanced
    low-level applications only.  For example to find the value
    for the entry with a y-coord less than the 5th entry:
    \code
    QVector3D result;
    BSPIndex ix = tree.pointerData()[5 * QBSPTree::Stride + LessThanY];
    if (ix != QBSPTree::MaxIndex)
        result = tree.vectorData()[ix];
    \endcode

    \value EqualTo Points to another tree entry equal to this one
    \value LessThanX Points to another tree entry equal to this one
    \value GreaterThanX Points to another tree entry equal to this one
    \value LessThanY Points to another tree entry equal to this one
    \value GreaterThanY Points to another tree entry equal to this one
    \value LessThanZ Points to another tree entry equal to this one
    \value GreaterThanZ Points to another tree entry equal to this one
    \value Stride The increment to the next tree entry
*/

/*!
    \fn const QArray<QVector3D> *QBSPTree::vectorData() const
    Returns a pointer to the underlying vector data.  This is the
    same as the value passed to the QBSPTree constructor.

    This function is intended for testing and advanced low-level
    applications.

    See the QBSPTree::Partition enum for an example of use.

    \sa pointerData()
*/

/*!
    \fn const QArray<BSPIndex> *QBSPTree::pointerData() const
    Returns a pointer to the underlying pointer data.  This is the
    data used to actually form the tree, and contains indexs into the
    QBSPTree's vector data.

    This function is intended for testing and advanced low-level
    applications.

    See the QBSPTree::Partition enum for an example of use.

    \sa vectorData()
*/

/*!
    \fn Partition QBSPTree::cmp(const QVector3D &v, const QVector3D &w)
    Compares the two QVector3D coordinate values \a v and \a w, and
    returns the appropriate partitioning.

    Note that this partition test is done in order down through the
    QBSPTree::Partition enum, so (for example) if both the x-coordinate
    and y-coordinate are less QBSPTree::LessThanX will be returned:

    \code
    QVector3D a(1.0f, 2.0f, 3.0f);
    QVector3D b(-1.0f, -2.0f, 3.0f);

    // p == QBSPTree::LessThanX - not LessThanY
    QBSPTree::Partition p = QBSPTree::cmp(a, b);
    \endcode

    This function is used by QBSPTree for constructing its indices.
*/

/*!
    \internal
    Mapping of partitions to their complements; for example the entry
    at 1 - that is QBSPTree::LessThanX - is QBSPTree::GreaterThanX.
    This is used in tree balancing.
*/
const QBSPTree::Partition QBSPTree::complement[Stride] = {
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
    \brief The QBSPTree::const_iterator class is an iterator over a const QBSPTree.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::geometry

    QBSPTree functions constFind() and constBegin() return iterator instances
    which can be used to retrieve all relevant values.

    Use the \c{*} operator or value() function to return the current entry
    pointed to by this iterator - a QBSPTree::BSPIndex value.  Use the key()
    function to return the key for the current entry - a QVector3D value.

    \sa QBSPTree
*/

/*!
    \fn QBSPTree::const_iterator::const_iterator(const QBSPTree *d, BSPIndex p, bool fi)
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
    iterator the next entry will be the one \l{QBSPTree::EqualTo}{equal to} the
    current one.  In the case of a sequence iterator it will be next in-order
    in the tree.  If there is no such entry, this iterator will point at
    QBSPTree::constEnd().  Returns a reference to this iterator.
*/

/*!
    \fn QBSPTree::BSPIndex QBSPTree::const_iterator::operator*() const
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
    \fn BSPIndex QBSPTree::const_iterator::value() const
    Returns the value associated with this iterator.
*/

/*!
    Constructs a new QBSPTree instance based on the QVector3DArray \a data.
*/
QBSPTree::QBSPTree(const QArray<QVector3D> *v)
    : m_vec(v)
    , m_maxHeight(0)
    , m_root(0)
{
}

/*!
    Destroys this QBSPTree instance recovering any resources.
*/
QBSPTree::~QBSPTree()
{
}

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

struct insert_record
{
    int height;
    QVector3D vec;
    QBSPTree::BSPIndex vec_ix;
    QBSPTree::BSPIndex child;
    QBSPTree::BSPIndex parent;
    QBSPTree::Partition parent_part;
    QBSPTree::BSPIndex grandparent;
    QBSPTree::Partition grandparent_part;
    QBSPTree::BSPIndex greatgrandparent;
    QBSPTree::Partition greatgrandparent_part;

    insert_record(const QVector3D &v, QBSPTree::BSPIndex vi,
                  QBSPTree::BSPIndex pointer, QBSPTree::BSPIndex new_index)
        : height(0)
        , vec(v)
        , vec_ix(vi)
        , child(new_index)
        , parent(pointer)
    {}

    void shift_down(QBSPTree::BSPIndex ix, QBSPTree::Partition part)
    {
        ++height;
        greatgrandparent = grandparent;
        greatgrandparent_part = grandparent_part;
        grandparent = parent;
        grandparent_part = parent_part;
        parent = ix;
        parent_part = part;
    }
};

inline static bool isLeft(QBSPTree::Partition part)
{
    return part % 2;
}

void QBSPTree::rotateLLorRR(const insert_record *bal)
{
    // because of the use of complement the two symmetric cases
    // can be treated as one
    BSPIndex i = bal->grandparent;
    Partition p = bal->grandparent_part;
    m_ptrs[i * Stride + p] = MaxIndex;
    i = bal->parent;
    p = complement[bal->grandparent_part];
    m_ptrs[i * Stride + p] = bal->grandparent;
    if (bal->height == 2)
    {
        m_root = bal->parent;
    }
    else
    {
        i = bal->greatgrandparent;
        p = bal->greatgrandparent_part;
        m_ptrs[i * Stride + p] = bal->parent;
    }
}

void QBSPTree::rotateLRorRL(const insert_record *bal)
{
    // because of the use of complement the two symmetric cases
    // can be treated as one
    BSPIndex i = bal->grandparent;
    Partition p = bal->grandparent_part;
    m_ptrs[i * Stride + p] = MaxIndex;
    i = bal->parent;
    p = bal->parent_part;
    m_ptrs[i * Stride + p] = MaxIndex;
    i = bal->child;
    p = complement[bal->parent_part];
    m_ptrs[i * Stride + p] = bal->parent;
    p = complement[bal->grandparent_part];
    m_ptrs[i * Stride + p] = bal->grandparent;
    if (bal->height == 2)
    {
        m_root = bal->child;
    }
    else
    {
        i = bal->greatgrandparent;
        p = bal->greatgrandparent_part;
        m_ptrs[i * Stride + p] = bal->child;
    }
}

void QBSPTree::maybe_rebalance(const insert_record *bal)
{
    int h = bal->height - m_maxHeight;
    if (h > 0)
    {
        m_maxHeight = bal->height;
        if (h > 1)
        {
            if (isLeft(bal->grandparent_part))
            {
                if (isLeft(bal->parent_part))
                    rotateLLorRR(bal);   // LL case
                else
                    rotateLRorRL(bal);   // LR case
            }
            else
            {
                if (isLeft(bal->parent_part))
                    rotateLRorRL(bal);   // RL case
                else
                    rotateLLorRR(bal);   // RR case
            }
        }
    }
}

void QBSPTree::recurseAndInsert(insert_record *bal)
{
    Partition part = cmp(bal->vec, m_vec->at(bal->parent));
    BSPIndex next = m_ptrs[bal->parent * Stride + part];
    if (next != MaxIndex)
    {
        bal->shift_down(next, part);
        recurseAndInsert(bal);
    }
    else
    {
        m_ptrs.append(blank, Stride);
        m_ptrs[bal->parent * Stride + part] = bal->child;
        maybe_rebalance(bal);
    }
}

/*!
    \fn void QBSPTree::insertMulti(const QVector3D &v, int i)
    This function simply calls insert() with arguments \a v and \a i.
    It exists purely for compatibility with other containers which
    can map multiple values to the same key.
*/

/*!
    Inserts into the tree the QVector3D \a v as a key mapping to
    the value \a i.  The value \a i must be the index of \a v in
    the underlying QVector3DArray passed to the QBSPTree's constructor.
*/
void QBSPTree::insert(const QVector3D &v, int i)
{
    Q_ASSERT(m_vec->at(i) == v);
    if (static_cast<BSPIndex>(m_ptrs.size()) < (MaxIndex - 1) * Stride)
    {
        if (m_ptrs.size() > 0)
        {
            BSPIndex new_ix = m_ptrs.size() / Stride;
            insert_record bal(v, i, m_root, new_ix);
            recurseAndInsert(&bal);
        }
        else
        {
            m_ptrs.append(blank, Stride);
        }
    }
#ifndef QT_NO_DEBUG
    else
    {
        qWarning("Overflowed QBSPTree");
    }
#endif
}

void QBSPTree::recurseAndDump(QBSPTree::BSPIndex ix, int indent) const
{
    char ind[1024];
    qMemSet(ind, '\0', 1024);
    qMemSet(ind, ' ', indent < 341 ? indent * 3 : 1023);
    fprintf(stderr, "%sNode: %d - QVector3D(%0.3f, %0.3f, %0.3f) - max height: %d\n",
            ind, ix, m_vec->at(ix).x(), m_vec->at(ix).y(), m_vec->at(ix).z(),
            m_maxHeight);
    fprintf(stderr, "%s      Children", ind);
    int i = ix * Stride;
    QArray<BSPIndex> children;
    if (m_ptrs.at(i + EqualTo) != MaxIndex)
    {
        fprintf(stderr, " -- EqualTo: %d", m_ptrs.at(i + EqualTo));
        children << m_ptrs.at(i + EqualTo);
    }

    if (m_ptrs.at(i + LessThanX) != MaxIndex)
    {
        fprintf(stderr, " -- LessThanX: %d", m_ptrs.at(i + LessThanX));
        children << m_ptrs.at(i + LessThanX);
    }

    if (m_ptrs.at(i + GreaterThanX) != MaxIndex)
    {
        fprintf(stderr, " -- GreaterThanX: %d", m_ptrs.at(i + GreaterThanX));
        children << m_ptrs.at(i + GreaterThanX);
    }

    if (m_ptrs.at(i + LessThanY) != MaxIndex)
    {
        fprintf(stderr, " -- LessThanY: %d", m_ptrs.at(i + LessThanY));
        children << m_ptrs.at(i + LessThanY);
    }

    if (m_ptrs.at(i + GreaterThanY) != MaxIndex)
    {
        fprintf(stderr, " -- GreaterThanY: %d", m_ptrs.at(i + GreaterThanY));
        children << m_ptrs.at(i + GreaterThanY);
    }

    if (m_ptrs.at(i + LessThanZ) != MaxIndex)
    {
        fprintf(stderr, " -- LessThanZ: %d", m_ptrs.at(i + LessThanZ));
        children << m_ptrs.at(i + LessThanZ);
    }

    if (m_ptrs.at(i + GreaterThanZ) != MaxIndex)
    {
        fprintf(stderr, " -- GreaterThanZ: %d", m_ptrs.at(i + GreaterThanZ));
        children << m_ptrs.at(i + GreaterThanZ);
    }
    fprintf(stderr, "\n");
    for (int i = 0; i < children.count(); ++i)
        recurseAndDump(children[i], indent + 1);
}

/*!
    This debugging aid prints out an indented representation of the
    QBSPTree and its QVector3D data.
*/
void QBSPTree::dump() const
{
    fprintf(stderr, "======================== QBSPTree - %p ========================\n", this);
    recurseAndDump(m_root, 0);
}
