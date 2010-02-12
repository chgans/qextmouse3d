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

#include <QtTest/QtTest>
#include "qbsptree.h"
#include "qvectorarray.h"
#include "qtest_helpers_p.h"

#include "stdlib.h"

namespace QTest {
    const char *qbsp_index_names[] = {
        "EqualTo",
        "LessThanX",
        "GreaterThanX",
        "LessThanY",
        "GreaterThanY",
        "LessThanZ",
        "GreaterThanZ",
        "Stride"
    };
    char *toString(QBSP::Partition ix)
    {
        char *msg = new char[128];
        qt_snprintf(msg, 128, "%s",
                    qbsp_index_names[ix]);
        return msg;
    }
    template<> bool qCompare<QBSP::Partition>(const QBSP::Partition &t1,
                                                  const QBSP::Partition &t2,
                                        const char *actual, const char *expected,
                                        const char *file, int line)
    {
        if (t1 == t2)
        {
            return compare_helper(true, "COMPARE()", file, line);
        }
        else
        {
            return compare_helper(false, "Compared QBSP::Index values are not the same:",
                                  toString(t1), toString(t2), actual, expected, file, line);
        }
    }
};

class tst_QBSPTree : public QObject
{
    Q_OBJECT
public:
    tst_QBSPTree() {}
    ~tst_QBSPTree() {}

private slots:
    void create();
    void cmp();
    void insert();
    void iterators();
    void find();
    void rotateLLorRR();
    void rotateLRorRL();
    void maybe_rebalance();
    void rotate();
};

class TestQBSPTree : public QBSPTree
{
public:
    TestQBSPTree(const QArray<QVector3D> *v) : QBSPTree(v) {}
    const QArray<QVector3D> *vectorData() const { return data()->vectorData(); }
    const QArray<QBSP::Node> *pointerData() const { return data()->pointerData(); }
    QBSP::Index root() const { return data()->root(); }
    int height() const { return data()->height(); }
    bool isEmpty() const { return data()->isEmpty(); }
    void reserve(int amount) { data()->reserve(amount); }

    void rotateLLorRR(const QBSP::InsertRecord *rec) { data()->rotateLLorRR(rec); }
    void rotateLRorRL(const QBSP::InsertRecord *rec) { data()->rotateLRorRL(rec); }
    void maybe_rebalance(const QBSP::InsertRecord *rec) { data()->maybe_rebalance(rec); }
    void recurseAndInsert(QBSP::InsertRecord *rec) { data()->recurseAndInsert(rec); }

    QBSP::Data *data() { return QBSPTree::data(); }
    const QBSP::Data *data() const { return QBSPTree::data(); }
};

void tst_QBSPTree::create()
{
    QVector3DArray data;
    TestQBSPTree tree(&data);
    QCOMPARE(tree.vectorData(), &data);
    QVERIFY(tree.pointerData() != NULL);
}

void tst_QBSPTree::cmp()
{
    QVector3DArray data;
    data.extend(10);
    data[0] = QVector3D();
    data[1] = QVector3D(1.0f, 2.0f, 3.0f);
    data[2] = QVector3D(-1.0f, 2.0f, 2.0f);
    data[3] = QVector3D(1.0f, -2.0f, 3.0f);
    data[4] = QVector3D(1.0f, 2.0f, -3.0f);
    data[5] = QVector3D(1.0f, -2.0f, -3.0f);
    data[6] = QVector3D(-1.0f, -2.0f, 3.0f);
    data[7] = QVector3D(-1.0f, 2.0f, -3.0f);
    data[8] = QVector3D(-1.0f, -2.0f, -3.0f);
    data[9] = QVector3D(-1.0f, 2.0f, 3.0f);

    QCOMPARE(QBSP::cmp(data.at(0), data.at(0)), QBSP::EqualTo);
    QCOMPARE(QBSP::cmp(data.at(0), data.at(1)), QBSP::LessThanX);
    QCOMPARE(QBSP::cmp(data.at(0), data.at(2)), QBSP::GreaterThanX);
    QCOMPARE(QBSP::cmp(data.at(1), data.at(1)), QBSP::EqualTo);
    QCOMPARE(QBSP::cmp(data.at(1), data.at(2)), QBSP::GreaterThanX);
    QCOMPARE(QBSP::cmp(data.at(1), data.at(3)), QBSP::GreaterThanY);
    QCOMPARE(QBSP::cmp(data.at(1), data.at(4)), QBSP::GreaterThanZ);
    QCOMPARE(QBSP::cmp(data.at(1), data.at(5)), QBSP::GreaterThanY);
    QCOMPARE(QBSP::cmp(data.at(2), data.at(6)), QBSP::GreaterThanY);
    QCOMPARE(QBSP::cmp(data.at(2), data.at(7)), QBSP::GreaterThanZ);
    QCOMPARE(QBSP::cmp(data.at(2), data.at(8)), QBSP::GreaterThanY);
    QCOMPARE(QBSP::cmp(data.at(2), data.at(9)), QBSP::LessThanZ);
    QCOMPARE(QBSP::cmp(data.at(3), data.at(4)), QBSP::LessThanY);
}

void tst_QBSPTree::insert()
{
    QVector3DArray data;
    TestQBSPTree tree(&data);
    data << QVector3D();
    data << QVector3D(1.0f, 2.0f, 3.0f);
    data << QVector3D(-1.0f, 2.0f, 3.0f);
    data << QVector3D(1.0f, -2.0f, 3.0f);
    data << QVector3D(1.0f, 2.0f, -3.0f);
    data << QVector3D(1.0f, -2.0f, -3.0f);
    data << QVector3D(-1.0f, -2.0f, 3.0f);
    data << QVector3D(-1.0f, 2.0f, -3.0f);
    data << QVector3D(-1.0f, -2.0f, -3.0f);
    data << QVector3D(-1.0f, 2.0f, 3.0f);
    for (int i = 0; i < data.size(); ++i)
    {
        tree.insert(data[i], i);
    }
    const QArray<QBSP::Node> *ptrs = tree.pointerData();
    for (int i = 0; i < ptrs->size(); i += QBSP::Stride)
    {
        if (ptrs->at(i).next[QBSP::EqualTo] != QBSP::MaxIndex)
            QCOMPARE(data.at(i), data.at(ptrs->at(i).next[QBSP::EqualTo]));

        if (ptrs->at(i).next[QBSP::LessThanX] != QBSP::MaxIndex)
            QVERIFY(data.at(ptrs->at(i).next[QBSP::LessThanX]).x() < data.at(i).x());

        if (ptrs->at(i).next[QBSP::GreaterThanX] != QBSP::MaxIndex)
            QVERIFY(data.at(ptrs->at(i).next[QBSP::GreaterThanX]).x() > data.at(i).x());

        if (ptrs->at(i).next[QBSP::LessThanY] != QBSP::MaxIndex)
            QVERIFY(data.at(ptrs->at(i).next[QBSP::LessThanY]).y() < data.at(i).y());

        if (ptrs->at(i).next[QBSP::GreaterThanY] != QBSP::MaxIndex)
            QVERIFY(data.at(ptrs->at(i).next[QBSP::GreaterThanY]).y() > data.at(i).y());

        if (ptrs->at(i).next[QBSP::LessThanZ] != QBSP::MaxIndex)
            QVERIFY(data.at(ptrs->at(i).next[QBSP::LessThanZ]).z() < data.at(i).z());

        if (ptrs->at(i).next[QBSP::GreaterThanZ] != QBSP::MaxIndex)
            QVERIFY(data.at(ptrs->at(i).next[QBSP::GreaterThanZ]).z() > data.at(i).z());
    }
}

void tst_QBSPTree::iterators()
{
    QVector3DArray data;
    TestQBSPTree tree(&data);
    QBSPTree::const_iterator it = tree.constBegin();
    QBSPTree::const_iterator et = tree.constEnd();
    QBSPTree::const_iterator ot(tree.data(), QBSP::MaxIndex);
    QVERIFY(it == ot);
    QVERIFY(it == et);
    QCOMPARE(it.value(), (int)QBSP::MaxIndex);
    ++it;
    QCOMPARE(it.value(), (int)QBSP::MaxIndex);
    data.extend(10);
    data[0] = QVector3D();
    data[1] = QVector3D(1.0f, 2.0f, 3.0f);
    data[2] = QVector3D(-1.0f, 2.0f, 2.0f);
    data[3] = QVector3D(1.0f, -2.0f, 3.0f);
    data[4] = QVector3D(1.0f, 2.0f, -3.0f);
    data[5] = QVector3D(1.0f, -2.0f, -3.0f);
    data[6] = QVector3D(-1.0f, -2.0f, 3.0f);
    data[7] = QVector3D(-1.0f, 2.0f, -3.0f);
    data[8] = QVector3D(-1.0f, -2.0f, -3.0f);
    data[9] = QVector3D(-1.0f, 2.0f, 3.0f);
    for (int i = 0; i < data.size(); ++i)
    {
        tree.insert(data[i], i);
    }
    it = tree.constBegin();
    et = tree.constEnd();
    QBSPTree::const_iterator ot2(tree.data(), 0);
    QBSPTree::const_iterator ot3(tree.data(), QBSP::MaxIndex);
    QVERIFY(it == ot2);
    QVERIFY(et == ot3);
    QCOMPARE(it.value(), 0);
    QCOMPARE(it.key(), data.at(0));
    ++it;
    QCOMPARE(it.value(), 1);
    QCOMPARE(it.key(), data.at(1));
    ++it;
    QCOMPARE(it.value(), 2);
    QCOMPARE(it.key(), data.at(2));
    for (int i = 2; i < 9; ++i)
        ++it;
    QCOMPARE(it.value(), 9);
    QCOMPARE(it.key(), data.at(9));
    ++it;
    QVERIFY(it == et);
    QCOMPARE(it.value(), (int)QBSP::MaxIndex);
}

void tst_QBSPTree::find()
{
    QVector3DArray data;
    TestQBSPTree tree(&data);
    data.extend(10);
    data[0] = QVector3D();
    data[1] = QVector3D(1.0f, 2.0f, 3.0f);
    data[2] = QVector3D(-1.0f, 2.0f, 3.0f);
    data[3] = QVector3D(1.0f, -2.0f, 3.0f);
    data[4] = QVector3D(1.0f, 2.0f, -3.0f);
    data[5] = QVector3D(1.0f, -2.0f, -3.0f);
    data[6] = QVector3D(-1.0f, -2.0f, 3.0f);
    data[7] = QVector3D(-1.0f, 2.0f, 3.0f);
    data[8] = QVector3D(-1.0f, 2.0f, -3.0f);
    data[9] = QVector3D(-1.0f, 2.0f, 4.0f);
    for (int i = 0; i < data.size(); ++i)
    {
        tree.insert(data[i], i);
    }
    {
        // find at beginning
        QBSPTree::const_iterator it = tree.constFind(QVector3D());
        QCOMPARE(it.value(), 0);
        QCOMPARE(it.key(), QVector3D());
        QVERIFY(it != tree.constEnd());
        QVERIFY(it == tree.constBegin());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find at beginning + 1
        QBSPTree::const_iterator it = tree.constFind(QVector3D(1.0f, 2.0f, 3.0f));
        QCOMPARE(it.value(), 1);
        QCOMPARE(it.key(), QVector3D(1.0f, 2.0f, 3.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find at end - 1
        QBSPTree::const_iterator it = tree.constFind(QVector3D(-1.0f, 2.0f, -3.0f));
        QCOMPARE(it.value(), 8);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, -3.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find at end
        QBSPTree::const_iterator it = tree.constFind(QVector3D(-1.0f, 2.0f, 4.0f));
        QCOMPARE(it.value(), 9);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, 4.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find both instances
        QBSPTree::const_iterator it = tree.constFind(QVector3D(-1.0f, 2.0f, 3.0f));
        QCOMPARE(it.value(), 2);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, 3.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it != tree.constEnd());
        QCOMPARE(it.value(), 7);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, 3.0f));
    }
}

void tst_QBSPTree::rotateLLorRR()
{
    QVector3DArray data;
    data.extend(10);
    data[0] = QVector3D();
    data[1] = QVector3D(1.0f, 2.0f, 3.0f);
    data[2] = QVector3D(2.0f, 2.0f, 3.0f);
    data[3] = QVector3D(3.0f, 2.0f, 3.0f);
    data[4] = QVector3D(4.0f, 2.0f, 3.0f);
    data[5] = QVector3D(5.0f, 2.0f, 3.0f);
    // this data should create an RR case
    TestQBSPTree tree(&data);
    for (int i = 0; i < data.count(); ++i)
        tree.insert(data[i], i);

    QBSP::InsertRecord rec(data[5], 5, 2, 5);
    rec.shift_down(3, QBSP::GreaterThanX);
    rec.shift_down(4, QBSP::GreaterThanX);
    QCOMPARE(rec.parent->ix, (QBSP::Index)4);
    QCOMPARE(rec.parent->part, QBSP::GreaterThanX);
    QCOMPARE(rec.grand->ix, (QBSP::Index)3);
    QCOMPARE(rec.grand->part, QBSP::GreaterThanX);
    QCOMPARE(rec.great->ix, (QBSP::Index)2);
    QCOMPARE(rec.great->part, QBSP::GreaterThanX);
    tree.data()->rotateLLorRR(&rec);
    const QArray<QBSP::Node> *ptrs = tree.data()->pointerData();
    // what was grandparent is now a leaf node
    for (QBSP::Index i = 0; i < QBSP::Stride; ++i)
        QCOMPARE(ptrs->at(3).next[i], QBSP::MaxIndex);
    // child is still a leaf node
    for (QBSP::Index i = 0; i < QBSP::Stride; ++i)
        QCOMPARE(ptrs->at(5).next[i], QBSP::MaxIndex);
    // parent should have rotated up and have 3 & 5 as children
    for (QBSP::Index i = 0; i < QBSP::Stride; ++i)
    {
        if (i == QBSP::LessThanX)
            QCOMPARE(ptrs->at(4).next[i], (QBSP::Index)3);
        else if (i == QBSP::GreaterThanX)
            QCOMPARE(ptrs->at(4).next[i], (QBSP::Index)5);
        else
            QCOMPARE(ptrs->at(4).next[i], QBSP::MaxIndex);
    }
    // great-grandparent should now point to rotated up parent
    for (QBSP::Index i = 0; i < QBSP::Stride; ++i)
    {
        if (i == QBSP::GreaterThanX)
            QCOMPARE(ptrs->at(2).next[i], (QBSP::Index)4);
        else
            QCOMPARE(ptrs->at(2).next[i], QBSP::MaxIndex);
    }
}

void tst_QBSPTree::rotateLRorRL()
{

}

void tst_QBSPTree::maybe_rebalance()
{

}

void tst_QBSPTree::rotate()
{
    QVector3DArray data;
    data.extend(10);
    data[0] = QVector3D();
    data[1] = QVector3D(1.0f, 2.0f, 3.0f);
    data[2] = QVector3D(2.0f, 2.0f, 3.0f);
    data[3] = QVector3D(3.0f, 2.0f, 3.0f);
    data[4] = QVector3D(4.0f, 2.0f, 3.0f);
    data[5] = QVector3D(5.0f, 2.0f, 3.0f);
    data[6] = QVector3D(6.0f, 2.0f, 3.0f);
    data[7] = QVector3D(7.0f, 2.0f, 3.0f);
    data[8] = QVector3D(8.0f, 2.0f, 3.0f);
    data[9] = QVector3D(9.0f, 2.0f, 3.0f);
    TestQBSPTree tree(&data);
    for (int i = 0; i < data.count(); ++i)
        tree.insert(data[i], i);
    tree.dump();
    // with rotations tree height should now be less log base 2 of 10, ie 3-4
    QVERIFY(tree.height() < 5);
    QBSPTree::const_iterator it = tree.constFind(QVector3D(4.0f, 2.0f, 3.0f));
    QVERIFY(it != tree.constEnd());
    it = tree.constFind(QVector3D(9.0f, 2.0f, 3.0f));
    QVERIFY(it != tree.constEnd());
}

QTEST_APPLESS_MAIN(tst_QBSPTree)

#include "tst_qbsptree.moc"
