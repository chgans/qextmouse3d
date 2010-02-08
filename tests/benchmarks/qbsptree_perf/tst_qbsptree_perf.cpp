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

class tst_QBSPTree : public QObject
{
    Q_OBJECT
public:
    tst_QBSPTree() {}
    virtual ~tst_QBSPTree() {}

private slots:
    void insert_data();
    void insert();
};

enum {
    Test_Ordered,
    Test_Random
};

void tst_QBSPTree::insert_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 500; size < 50000; size += 500)
    {
        name = "Ordered--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Ordered);

        name = "Random--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Random);
    }
}

#define mkcoord(n) (((float)(n) / (float)RAND_MAX) * 1000.0f - 500.0f)

void tst_QBSPTree::insert()
{
    QFETCH(int, size);
    QFETCH(int, type);

    qsrand(314159);
    int n = (qPow(size, 0.3333333f) + 1.0f) / 2.0f;
    size = (n * 2) * (n * 2) * (n * 2);
    QVector3DArray data;
    if (type == Test_Ordered)
    {
        qDebug() << "Ordered: adding" << n << "x" << n << "x" << n <<
                "=" << (n*n*n) << "verts";
        for (int i = -n; i < n; ++i)
            for (int j = -n; j < n; ++j)
                for (int k = -n; k < n; ++k)
                    data << QVector3D(i, j, k);
    }
    else if (type == Test_Random)
    {
        qDebug() << "Random: adding" << size << "verts";
        for (int i = 0; i < size; ++i)
            data << QVector3D(mkcoord(qrand()), mkcoord(qrand()), mkcoord(qrand()));
    }
    QBSPTree tree(&data);
    QBENCHMARK {
        for (int i = 0; i < data.size(); ++i)
        {
            tree.insert(data.at(i), i);
        }
    }
}

QTEST_MAIN(tst_QBSPTree)

#include "tst_qbsptree_perf.moc"
