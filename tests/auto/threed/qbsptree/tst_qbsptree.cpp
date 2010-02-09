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
    const char *index_names[] = {
        "EqualTo",
        "LessThanX",
        "GreaterThanX",
        "LessThanY",
        "GreaterThanY",
        "LessThanZ",
        "GreaterThanZ",
        "Stride"
    };
    char *toString(QBSPTree::Partition ix)
    {
        char *msg = new char[128];
        qt_snprintf(msg, 128, "%s",
                    index_names[ix]);
        return msg;
    }
    template<> bool qCompare<QBSPTree::Partition>(const QBSPTree::Partition &t1,
                                                  const QBSPTree::Partition &t2,
                                        const char *actual, const char *expected,
                                        const char *file, int line)
    {
        if (t1 == t2)
        {
            return compare_helper(true, "COMPARE()", file, line);
        }
        else
        {
            return compare_helper(false, "Compared QBSPTree::BSPIndex values are not the same:",
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
};

void tst_QBSPTree::create()
{
    QVector3DArray data;
    QBSPTree tree(&data);
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

    QCOMPARE(QBSPTree::cmp(data.at(0), data.at(0)), QBSPTree::EqualTo);
    QCOMPARE(QBSPTree::cmp(data.at(0), data.at(1)), QBSPTree::LessThanX);
    QCOMPARE(QBSPTree::cmp(data.at(0), data.at(2)), QBSPTree::GreaterThanX);
    QCOMPARE(QBSPTree::cmp(data.at(1), data.at(1)), QBSPTree::EqualTo);
    QCOMPARE(QBSPTree::cmp(data.at(1), data.at(2)), QBSPTree::GreaterThanX);
    QCOMPARE(QBSPTree::cmp(data.at(1), data.at(3)), QBSPTree::GreaterThanY);
    QCOMPARE(QBSPTree::cmp(data.at(1), data.at(4)), QBSPTree::GreaterThanZ);
    QCOMPARE(QBSPTree::cmp(data.at(1), data.at(5)), QBSPTree::GreaterThanY);
    QCOMPARE(QBSPTree::cmp(data.at(2), data.at(6)), QBSPTree::GreaterThanY);
    QCOMPARE(QBSPTree::cmp(data.at(2), data.at(7)), QBSPTree::GreaterThanZ);
    QCOMPARE(QBSPTree::cmp(data.at(2), data.at(8)), QBSPTree::GreaterThanY);
    QCOMPARE(QBSPTree::cmp(data.at(2), data.at(9)), QBSPTree::LessThanZ);
    QCOMPARE(QBSPTree::cmp(data.at(3), data.at(4)), QBSPTree::LessThanY);
}

void tst_QBSPTree::insert()
{
    QVector3DArray data;
    QBSPTree tree(&data);
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
    const QArray<QBSPTree::BSPIndex> *ptrs = tree.pointerData();
    for (int i = 0; i < ptrs->size(); i += QBSPTree::Stride)
    {
        int ix = i / QBSPTree::Stride;
        if (ptrs->at(i + QBSPTree::EqualTo) != QBSPTree::MaxIndex)
            QCOMPARE(data.at(ix), data.at(ptrs->at(i + QBSPTree::EqualTo)));

        if (ptrs->at(i + QBSPTree::LessThanX) != QBSPTree::MaxIndex)
            QVERIFY(data.at(ptrs->at(i + QBSPTree::LessThanX)).x() < data.at(ix).x());

        if (ptrs->at(i + QBSPTree::GreaterThanX) != QBSPTree::MaxIndex)
            QVERIFY(data.at(ptrs->at(i + QBSPTree::GreaterThanX)).x() > data.at(ix).x());

        if (ptrs->at(i + QBSPTree::LessThanY) != QBSPTree::MaxIndex)
            QVERIFY(data.at(ptrs->at(i + QBSPTree::LessThanY)).y() < data.at(ix).y());

        if (ptrs->at(i + QBSPTree::GreaterThanY) != QBSPTree::MaxIndex)
            QVERIFY(data.at(ptrs->at(i + QBSPTree::GreaterThanY)).y() > data.at(ix).y());

        if (ptrs->at(i + QBSPTree::LessThanZ) != QBSPTree::MaxIndex)
            QVERIFY(data.at(ptrs->at(i + QBSPTree::LessThanZ)).z() < data.at(ix).z());

        if (ptrs->at(i + QBSPTree::GreaterThanZ) != QBSPTree::MaxIndex)
            QVERIFY(data.at(ptrs->at(i + QBSPTree::GreaterThanZ)).z() > data.at(ix).z());
    }
}

void tst_QBSPTree::iterators()
{
    QVector3DArray data;
    QBSPTree tree(&data);
    QBSPTree::const_iterator it = tree.constBegin();
    QBSPTree::const_iterator et = tree.constEnd();
    QBSPTree::const_iterator ot(&tree, QBSPTree::MaxIndex);
    QVERIFY(it == ot);
    QVERIFY(it == et);
    QCOMPARE(it.value(), QBSPTree::MaxIndex);
    ++it;
    QCOMPARE(it.value(), QBSPTree::MaxIndex);
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
    QBSPTree::const_iterator ot2(&tree, 0);
    QBSPTree::const_iterator ot3(&tree, QBSPTree::MaxIndex);
    QVERIFY(it == ot2);
    QVERIFY(et == ot3);
    QCOMPARE(it.value(), (QBSPTree::BSPIndex)0);
    QCOMPARE(it.key(), data.at(0));
    ++it;
    QCOMPARE(it.value(), (QBSPTree::BSPIndex)1);
    QCOMPARE(it.key(), data.at(1));
    ++it;
    QCOMPARE(it.value(), (QBSPTree::BSPIndex)2);
    QCOMPARE(it.key(), data.at(2));
    for (int i = 2; i < 9; ++i)
        ++it;
    QCOMPARE(it.value(), (QBSPTree::BSPIndex)9);
    QCOMPARE(it.key(), data.at(9));
    ++it;
    QCOMPARE(it, et);
    QCOMPARE(it.value(), QBSPTree::MaxIndex);
}

void tst_QBSPTree::find()
{
    QVector3DArray data;
    QBSPTree tree(&data);
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
        QCOMPARE(it.value(), 0U);
        QCOMPARE(it.key(), QVector3D());
        QVERIFY(it != tree.constEnd());
        QVERIFY(it == tree.constBegin());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find at beginning + 1
        QBSPTree::const_iterator it = tree.constFind(QVector3D(1.0f, 2.0f, 3.0f));
        QCOMPARE(it.value(), 1U);
        QCOMPARE(it.key(), QVector3D(1.0f, 2.0f, 3.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find at end - 1
        QBSPTree::const_iterator it = tree.constFind(QVector3D(-1.0f, 2.0f, -3.0f));
        QCOMPARE(it.value(), 8U);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, -3.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find at end
        QBSPTree::const_iterator it = tree.constFind(QVector3D(-1.0f, 2.0f, 4.0f));
        QCOMPARE(it.value(), 9U);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, 4.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it == tree.constEnd());
    }
    {
        // find both instances
        QBSPTree::const_iterator it = tree.constFind(QVector3D(-1.0f, 2.0f, 3.0f));
        QCOMPARE(it.value(), 2U);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, 3.0f));
        QVERIFY(it != tree.constEnd());
        ++it;
        QVERIFY(it != tree.constEnd());
        QCOMPARE(it.value(), 7U);
        QCOMPARE(it.key(), QVector3D(-1.0f, 2.0f, 3.0f));
    }
}

QTEST_APPLESS_MAIN(tst_QBSPTree)

#include "tst_qbsptree.moc"
