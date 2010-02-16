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

// If this platform has std::multi_hash uncomment this to compare its
// performance against other containers
#define HAVE_STL_MULTI_HASH 1

class tst_QBSPTree : public QObject
{
    Q_OBJECT
public:
    enum TestType {
        Test_Ordered    = 0x00,
        Test_Random     = 0x01,
        Test_QHash      = 0x02,
        Test_AdHoc      = 0x04,
        Test_Balanced   = 0x08,
        Test_STLHash    = 0x10,
        Test_Structure  = Test_QHash | Test_AdHoc | Test_Balanced | Test_STLHash
    };
    Q_DECLARE_FLAGS(TestTypes, TestType);

    tst_QBSPTree() {}
    virtual ~tst_QBSPTree() {}

private:
    QVector3DArray makeValues(int size, TestTypes type);
    void makeData(int start = 0, int stop = 20000, int increment = 500);

private slots:
    void insertSmall_data();
    void insertSmall();
    void insert_data();
    void insert();
    void find_data();
    void find();
    void iterate_data();
    void iterate();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(tst_QBSPTree::TestTypes);

const char *test_names[] = {
    "QHash",
    "AdHoc",
    "Balanced",
    "STLHash"
};

uint qHash(float data)
{
    union U {
        quint32 n;
        float f;
    };
    U u;
    u.f = data;
    return u.f;
}

/* ********** QHash stuff ******************************/

uint qHash(double data)
{
    union U {
        quint64 n;
        double f;
    };
    U u;
    u.f = data;
    return u.f;
}

#define ROTL10(x) (((x) << 10) | (((x) >> 22) & 0x000000ff))

#define ROTL20(x) (((x) << 20) | (((x) >> 12) & 0x0000ffff))

uint qHash(const QVector3D &v)
{
    return qHash(v.x()) ^ ROTL10(qHash(v.y())) ^ ROTL20(qHash(v.z()));
}

/* ********** std::hash_map stuff ******************************/

#ifdef HAVE_STL_MULTI_HASH
#include <ext/hash_map>

uint qStlHash(double data)
{
    union U {
        size_t n;
        double f;
    };
    U u;
    u.f = data;
    return u.f;
}

const int YShift = sizeof(size_t) / 3;
const int YShiftR = sizeof(size_t) - YShift;
const int ZShift = (sizeof(size_t) / 3) * 2;
const int ZShiftR = sizeof(size_t) - ZShift;

#define ROTL_Y(x) (((x) << YShift) | (((x) >> YShiftR) & 0x000000ff))

#define ROTL_Z(x) (((x) << ZShift) | (((x) >> ZShiftR) & 0x0000ffff))

uint stlHash(const QVector3D &v)
{
    return qStlHash(v.x()) ^ ROTL_Y(qStlHash(v.y())) ^ ROTL_Z(qStlHash(v.z()));
}

struct vec3d_hash
{
    size_t operator()(const QVector3D &v) const
    {
        return stlHash(v);
    }
};

struct vec3d_equal_to
{
    bool operator()(const QVector3D&a, const QVector3D &b) const
    {
        return qFuzzyCompare(a, b);
    }
};

namespace __gnu_cxx {
typedef hash_multimap<QVector3D, int, vec3d_hash, vec3d_equal_to> StdHash;
typedef hash_multimap<QVector3D, int, vec3d_hash, vec3d_equal_to>::const_iterator StdHashIterator;
};

#endif



/* ********** assemble arrays of test data ******************************/

#define mkcoord(n) (((float)(n) / (float)RAND_MAX) * 1000.0f - 500.0f)

QVector3DArray tst_QBSPTree::makeValues(int size, tst_QBSPTree::TestTypes type)
{
    QVector3DArray data;
    if (size > 0)
    {
        qsrand(314159);
        int n = (qPow((qreal)size, 0.3333333f) + 1.0f) / 2.0f;
        int result_size = (n * 2) * (n * 2) * (n * 2);
        if ((type & ~Test_Structure) == Test_Ordered)
        {
            for (int i = -n; i < n && data.size() < size; ++i)
                for (int j = -n; j < n && data.size() < size; ++j)
                    for (int k = -n; k < n && data.size() < size; ++k)
                        data << QVector3D(i, j, k);
        }
        else if ((type & ~Test_Structure) == Test_Random)
        {
            for (int i = 0; i < result_size; ++i)
                data << QVector3D(mkcoord(qrand()), mkcoord(qrand()), mkcoord(qrand()));
        }
    }
    return data;
}

void tst_QBSPTree::makeData(int start, int stop, int increment)
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    TestTypes t;


    for (int size = start; size < stop; size += increment)
    {
#ifdef HAVE_STL_MULTI_HASH
        for (int i = Test_QHash, j = 0; i <= Test_STLHash; i <<= 1, ++j)
#else
        for (int i = Test_QHash, j = 0; i <= Test_Balanced; i <<= 1, ++j)
#endif
        {
            t = Test_Ordered | (TestTypes)i;
            name = "Ordered--";
            name += QString::number(size);
            name.prepend(test_names[j]);
            QTest::newRow(name.constData()) << size << int(Test_Ordered | (TestTypes)i);

            t = Test_Ordered | (TestTypes)i;
            name = "Random--";
            name += QString::number(size);
            name.prepend(test_names[j]);
            QTest::newRow(name.constData()) << size << int(Test_Random | (TestTypes)i);
        }
    }
}

void tst_QBSPTree::insertSmall_data()
{
    makeData(0, 1000, 1);
}

void tst_QBSPTree::insertSmall()
{
    insert();
}

void tst_QBSPTree::insert_data()
{
    makeData(500, 50000, 500);
}

void tst_QBSPTree::insert()
{
    QFETCH(int, size);
    QFETCH(int, type);

    QVector3DArray data = makeValues(size, (TestTypes)type);
    if ((type & Test_Structure) == Test_QHash)
    {
        QBENCHMARK {
            QHash<QVector3D, int> hash;
            hash.reserve(data.size());
            for (int i = 0; i < data.size(); ++i)
            {
                hash.insertMulti(data.at(i), i);
            }
        }
    }
    else if ((type & Test_Structure) == Test_AdHoc)
    {
        QBENCHMARK {
            QBSPTree tree(&data, QBSPTree::AdHoc);
            tree.reserve(data.size());
            for (int i = 0; i < data.size(); ++i)
            {
                tree.insert(data.at(i), i);
            }
        }
    }
    else if ((type & Test_Structure) == Test_Balanced)
    {
        QBENCHMARK {
            QBSPTree tree(&data, QBSPTree::Balanced);
            tree.reserve(data.size());
            for (int i = 0; i < data.size(); ++i)
            {
                tree.insert(data.at(i), i);
            }
        }
    }
#ifdef HAVE_STL_MULTI_HASH
    else if ((type & Test_Structure) == Test_STLHash)
    {
        QBENCHMARK {
            __gnu_cxx::hash_multimap<QVector3D, int, vec3d_hash, vec3d_equal_to> stlhash;
            stlhash.resize(data.size());
            for (int i = 0; i < data.size(); ++i)
            {
                const QVector3D &v = data.at(i);
                stlhash.insert(std::pair<QVector3D,int>(v, i));
            }
        }
    }
#endif
}

void tst_QBSPTree::find_data()
{
    makeData(500, 20000, 500);
}

void tst_QBSPTree::find()
{
    QFETCH(int, size);
    QFETCH(int, type);

    QVector3DArray data = makeValues(size, (TestTypes)type);
    if ((type & Test_Structure) == Test_QHash)
    {
        QHash<QVector3D, int> hash;
        for (int i = 0; i < data.size(); ++i)
            hash.insertMulti(data.at(i), i);
        QBENCHMARK {
            QHash<QVector3D, int>::const_iterator it = hash.constFind(data[0]);
            Q_UNUSED(it);
            QHash<QVector3D, int>::const_iterator it2 = hash.constFind(data[data.size()/2]);
            Q_UNUSED(it2);
            QHash<QVector3D, int>::const_iterator it3 = hash.constFind(data[data.size() - 1]);
            Q_UNUSED(it3);
        }
    }
    else if ((type & Test_Structure) == Test_AdHoc)
    {
        QBSPTree tree(&data, QBSPTree::AdHoc);
        for (int i = 0; i < data.size(); ++i)
            tree.insert(data.at(i), i);
        QBENCHMARK {
            QBSPTree::const_iterator it = tree.constFind(data[0]);
            Q_UNUSED(it);
            QBSPTree::const_iterator it2 = tree.constFind(data[data.size()/2]);
            Q_UNUSED(it2);
            QBSPTree::const_iterator it3 = tree.constFind(data[data.size() - 1]);
            Q_UNUSED(it3);
        }
    }
    else if ((type & Test_Structure) == Test_Balanced)
    {
        QBSPTree tree(&data, QBSPTree::Balanced);
        for (int i = 0; i < data.size(); ++i)
            tree.insert(data.at(i), i);
        //tree.dump();
        QBENCHMARK {
            QBSPTree::const_iterator it = tree.constFind(data[0]);
            Q_UNUSED(it);
            QBSPTree::const_iterator it2 = tree.constFind(data[data.size()/2]);
            Q_UNUSED(it2);
            QBSPTree::const_iterator it3 = tree.constFind(data[data.size() - 1]);
            Q_UNUSED(it3);
        }
    }
#ifdef HAVE_STL_MULTI_HASH
    else if ((type & Test_Structure) == Test_STLHash)
    {
        __gnu_cxx::StdHash hash;
        for (int i = 0; i < data.size(); ++i)
            hash.insert(std::pair<QVector3D, int>(data[i], i));
        QBENCHMARK {
            __gnu_cxx::StdHashIterator it = hash.find(data[0]);
            Q_ASSERT(it != hash.end());
            __gnu_cxx::StdHashIterator it2 = hash.find(data[data.size()/2]);
            Q_ASSERT(it2 != hash.end());
            __gnu_cxx::StdHashIterator it3 = hash.find(data[data.size()-1]);
            Q_ASSERT(it3 != hash.end());
        }
    }
#endif
}

void tst_QBSPTree::iterate_data()
{
    makeData(100, 5000, 100);
}

void tst_QBSPTree::iterate()
{
    QFETCH(int, size);
    QFETCH(int, type);

    qsrand(314159);
    int n = qPow((qreal)size, 0.3333333f) / 2.0f;
    size = (n * 2) * (n * 2) * (n * 2);
    int n8 = size / 7 - 1;
    Q_ASSERT(n8 > 0);
    QVector3D n8v;     // have 8 copies of this one
    QVector3DArray data;
    if (type == Test_Ordered)
    {
        int tot = 0;
        for (int i = -n; i < n; ++i)
        {
            for (int j = -n; j < n; ++j)
            {
                for (int k = -n; k < n; ++k)
                {
                    data << QVector3D(i, j, k);
                    if ((tot % n8) == 0)
                    {
                        if (n8v.isNull())
                            n8v = QVector3D(i, j, k);
                        else
                            data << n8v;
                    }
                    ++tot;
                }
            }
        }
    }
    else if (type == Test_Random)
    {
        for (int i = 0; i < size; ++i)
        {
            QVector3D randvec(mkcoord(qrand()), mkcoord(qrand()), mkcoord(qrand()));
            data << randvec;
            if ((i % n8) == 0)
            {
                if (n8v.isNull())
                    n8v = randvec;
                else
                    data << n8v;
            }
        }
    }
    QBSPTree tree(&data);
    for (int i = 0; i < data.size(); ++i)
    {
        tree.insert(data.at(i), i);
    }
    QBENCHMARK {
        QBSPTree::const_iterator it2 = tree.constBegin();
        for ( ; it2 != tree.constEnd(); ++it2)
        {
            QVERIFY(it2.key() == data.at(it2.value()));
        }
    }
}

QTEST_MAIN(tst_QBSPTree)

#include "tst_qbsptree_perf.moc"
