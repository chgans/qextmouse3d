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
#include "qgldisplaylist.h"
#include "qgloperation.h"
#include "qglteapot.h"
#include "qglsection_p.h"

class TestList : public QGLDisplayList
{
public:
    QGLSection *section() { return currentSection(); }
    void setDefThreshold(int t) { setDefaultThreshold(t); }
};

class tst_QGLDisplayList : public QObject
{
    Q_OBJECT
public:
    tst_QGLDisplayList() {}
    virtual ~tst_QGLDisplayList() {}
    void addQuadBenchMarks(const QVector3DArray &data, int type);

private slots:
    void addQuadRandom_data();
    void addQuadRandom();
    void addQuadOrdered_data();
    void addQuadOrdered();
    void teapot();
};

enum {
    Test_3,
    Test_7,
    Test_10,
    Test_20
};

void tst_QGLDisplayList::addQuadRandom_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 10; size < 10000; size += 10)
    {
        name = "T3--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_3);

        name = "T7--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_7);

        name = "T10--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_10);

        name = "T20--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_20);
    }
}

static inline qreal randCoord()
{
    return (200.0f * ((qreal)qrand() / (qreal)RAND_MAX)) - 100.0f;
}

QVector3D randVector()
{
    static bool seeded = false;
    if (!seeded)
    {
        qsrand(time(0));
        seeded = true;
    }
    return QVector3D(randCoord(), randCoord(), randCoord());
}

void tst_QGLDisplayList::addQuadRandom()
{
    QFETCH(int, size);
    QFETCH(int, type);

    int n = qSqrt(size);
    size = n * n;
    QVector3DArray data;
    data.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        // make sure (in face of randomness) we get a planar quad
        QVector3D origin = randVector();
        QVector3D a;
        while (a.isNull())
            a = randVector();
        QVector3D b;
        while (b.isNull())
            b = randVector();
        data.append(origin, a, a+b, b);
    }
    addQuadBenchMarks(data, type);
}

void tst_QGLDisplayList::addQuadBenchMarks(const QVector3DArray &data, int type)
{
    int size = data.size();
    if (type == Test_3)
    {
        QBENCHMARK {
            TestList list;
            list.newSection(QGL::Smooth);
            list.section()->setMapThreshold(3);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGLOperation op(&list, QGL::QUAD);
                op << data[i] << data[i+1] << data[i+2] << data[i+3];
            }
            list.finalize();
        }
    }
    else if (type == Test_7)
    {
        QBENCHMARK {
            TestList list;
            list.newSection(QGL::Smooth);
            list.section()->setMapThreshold(7);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGLOperation op(&list, QGL::QUAD);
                op << data[i] << data[i+1] << data[i+2] << data[i+3];
            }
            list.finalize();
        }
    }
    else if (type == Test_10)
    {
        QBENCHMARK {
            TestList list;
            list.newSection(QGL::Smooth);
            list.section()->setMapThreshold(10);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGLOperation op(&list, QGL::QUAD);
                op << data[i] << data[i+1] << data[i+2] << data[i+3];
            }
            list.finalize();
        }
    }
    else if (type == Test_20)
    {
        QBENCHMARK {
            TestList list;
            list.newSection(QGL::Smooth);
            list.section()->setMapThreshold(20);
            for (int i = 0; (i+3) < size; i += 4)
            {
                QGLOperation op(&list, QGL::QUAD);
                op << data[i] << data[i+1] << data[i+2] << data[i+3];
            }
            list.finalize();
        }
    }
}

void tst_QGLDisplayList::addQuadOrdered_data()
{
    addQuadRandom_data();
}

void tst_QGLDisplayList::addQuadOrdered()
{
    QFETCH(int, size);
    QFETCH(int, type);

    int n = qSqrt(size);
    size = n * n;
    QVector3DArray data;
    data.reserve(size);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            data.append(QVector3D(1.0f * i, 1.0f * j, 0.0f),
                        QVector3D(1.0f * (i+1), 1.0f * j, 0.0f),
                        QVector3D(1.0f * (i+1), 1.0f * (j+1), 0.0f),
                        QVector3D(1.0f * i, 1.0f * (j+1), 0.0f));
    addQuadBenchMarks(data, type);
}

void tst_QGLDisplayList::teapot()
{
    QBENCHMARK {
        QGLDisplayList list;
        list << QGLTeapot();
        list.finalize();
    }
}


QTEST_MAIN(tst_QGLDisplayList)

#include "tst_qgldisplaylist_perf.moc"
