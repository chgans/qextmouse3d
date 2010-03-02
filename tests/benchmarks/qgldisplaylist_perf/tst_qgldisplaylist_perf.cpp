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

class tst_QGLDisplayList : public QObject
{
    Q_OBJECT
public:
    tst_QGLDisplayList() {}
    virtual ~tst_QGLDisplayList() {}

private slots:
    void addQuad_data();
    void addQuad();
    void teapot_data();
    void teapot();
};

enum {
    Test_Baseline,
    Test_Random,
    Test_Sections,
    Test_None,
    Test_Hash,
    Test_Map
};

void tst_QGLDisplayList::addQuad_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 5000; size < 10000; size += 200)
    {
        name = "Baseline--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Baseline);

        name = "Random--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Random);

        name = "Sections--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Sections);
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

void tst_QGLDisplayList::addQuad()
{
    QFETCH(int, size);
    QFETCH(int, type);

    int n = qSqrt(size);
    if (type == Test_Baseline)
    {
        QBENCHMARK {
            QGLDisplayList list;
            list.newSection(QGL::Smooth, QGL::MapLookup);
            for (int i = 0; i < n; ++i)
            {
                for (int j = 0; j < n; ++j)
                {
                    QGLOperation op(&list, QGL::QUAD);
                    op << QVector3D(1.0f * i, 1.0f * j, 0.0f);
                    op << QVector3D(1.0f * (i+1), 1.0f * j, 0.0f);
                    op << QVector3D(1.0f * (i+1), 1.0f * (j+1), 0.0f);
                    op << QVector3D(1.0f * i, 1.0f * (j+1), 0.0f);
                }
            }
            list.finalize();
        }
    }
    else if (type == Test_Random)
    {
        QBENCHMARK {
            QGLDisplayList list;
            for (int i = 0; i < n; ++i)
            {
                list.newSection(QGL::Smooth, QGL::MapLookup);
                for (int j = 0; j < n; ++j)
                {
                    QGLOperation op(&list, QGL::QUAD);
                    QVector3D origin = randVector();
                    QVector3D a;
                    while (a.isNull())
                        a = randVector();
                    QVector3D b;
                    while (b.isNull())
                        b = randVector();
                    op << origin;
                    op << (origin + a);
                    op << (origin + a + b);
                    op << (origin + b);
                }
            }
            list.finalize();
        }
    }
    else if (type == Test_Sections)
    {
        QBENCHMARK {
            QGLDisplayList list;
            for (int i = 0; i < n; ++i)
            {
                list.newSection(QGL::Smooth, QGL::MapLookup);
                for (int j = 0; j < n; ++j)
                {
                    QGLOperation op(&list, QGL::QUAD);
                    op << QVector3D(1.0f * i, 1.0f * j, 0.0f);
                    op << QVector3D(1.0f * (i+1), 1.0f * j, 0.0f);
                    op << QVector3D(1.0f * (i+1), 1.0f * (j+1), 0.0f);
                    op << QVector3D(1.0f * i, 1.0f * (j+1), 0.0f);
                }
            }
            list.finalize();
        }
    }
}

void tst_QGLDisplayList::teapot_data()
{
    QTest::addColumn<int>("type");

    QByteArray name;
    name = "None--";
    QTest::newRow(name.constData()) << int(Test_None);

    name = "QHash--";
    QTest::newRow(name.constData()) << int(Test_Hash);

    name = "QMap--";
    QTest::newRow(name.constData()) << int(Test_Map);
}

void tst_QGLDisplayList::teapot()
{
    QFETCH(int, type);

}


QTEST_MAIN(tst_QGLDisplayList)

#include "tst_qgldisplaylist_perf.moc"
