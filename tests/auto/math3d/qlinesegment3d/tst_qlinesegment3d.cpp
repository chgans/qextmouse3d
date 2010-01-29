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
#include "qlinesegment3d.h"

class tst_QLineSegment3D : public QObject
{
    Q_OBJECT
public:
    tst_QLineSegment3D() {}
    ~tst_QLineSegment3D() {}

private slots:
    void create();
    void endPoints();
    void toLine3D();
    void containsPoint_data();
    void containsPoint();
    void containsSegment_data();
    void containsSegment();
    void intersectSegment_data();
    void intersectSegment();
    void intersectLine_data();
    void intersectLine();
};

void tst_QLineSegment3D::create()
{
    QLineSegment3D segment1;
    QVERIFY(segment1.isPoint());
    QVERIFY(segment1.start().isNull());
    QVERIFY(segment1.end().isNull());
    QVERIFY(segment1.direction().isNull());
    QCOMPARE(segment1.length(), qreal(0.0f));

    QLineSegment3D segment2(QLine3D(QVector3D(1, 2, 3), QVector3D(0, 2, 0)), 5.0f);
    QVERIFY(!segment2.isPoint());
    QVERIFY(segment2.start() == QVector3D(1, 2, 3));
    QVERIFY(segment2.end() == QVector3D(1, 2 + 5, 3));
    QVERIFY(segment2.direction() == QVector3D(0, 1, 0));
    QCOMPARE(segment2.length(), qreal(5.0f));

    QLineSegment3D segment3(QVector3D(1, 2, 3), QVector3D(2, 4, 6));
    QVERIFY(!segment3.isPoint());
    QVERIFY(segment3.start() == QVector3D(1, 2, 3));
    QVERIFY(segment3.end() == QVector3D(2, 4, 6));
    QVector3D dir = segment3.end() - segment3.start();
    QVERIFY(qFuzzyCompare(segment3.direction(), dir.normalized()));
    QCOMPARE(segment3.length(), dir.length());
}

void tst_QLineSegment3D::endPoints()
{
    QLineSegment3D segment1;
    segment1.setStart(QVector3D(1, 2, 3));
    QVERIFY(!segment1.isPoint());
    QVERIFY(segment1.start() == QVector3D(1, 2, 3));
    QVERIFY(segment1.end().isNull());
    QVERIFY(segment1.vector() == QVector3D(-1, -2, -3));

    segment1.setEnd(QVector3D(2, 4, 6));
    QVERIFY(!segment1.isPoint());
    QVERIFY(segment1.start() == QVector3D(1, 2, 3));
    QVERIFY(segment1.end() == QVector3D(2, 4, 6));
    QVector3D dir = segment1.end() - segment1.start();
    QVERIFY(qFuzzyCompare(segment1.direction(), dir.normalized()));
    QCOMPARE(segment1.length(), dir.length());
}

void tst_QLineSegment3D::toLine3D()
{
    QLineSegment3D segment1(QVector3D(1, 2, 3), QVector3D(2, 4, 6));
    QLine3D line = segment1.toLine3D();
    QVector3D dir = segment1.end() - segment1.start();
    QVERIFY(line.origin() == segment1.start());
    QVERIFY(qFuzzyCompare(line.direction(), dir));
}

void tst_QLineSegment3D::containsPoint_data()
{
    QTest::addColumn<qreal>("x1");
    QTest::addColumn<qreal>("y1");
    QTest::addColumn<qreal>("z1");
    QTest::addColumn<qreal>("x2");
    QTest::addColumn<qreal>("y2");
    QTest::addColumn<qreal>("z2");
    QTest::addColumn<qreal>("x3");
    QTest::addColumn<qreal>("y3");
    QTest::addColumn<qreal>("z3");
    QTest::addColumn<bool>("contained");

    QTest::newRow("on line")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << true;

    QTest::newRow("abovex")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.75 << (qreal)3.0 << (qreal)4.5
        << false;

    QTest::newRow("belowx")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.25 << (qreal)3.0 << (qreal)4.5
        << false;

    QTest::newRow("abovey")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)3.5 << (qreal)4.5
        << false;

    QTest::newRow("belowy")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)2.5 << (qreal)4.5
        << false;

    QTest::newRow("abovez")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.75
        << false;

    QTest::newRow("belowz")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)2.0 << (qreal)4.25
        << false;

    QTest::newRow("on line, before start")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)0.5 << (qreal)1.0 << (qreal)1.5
        << false;

    QTest::newRow("on line, after end")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)2.5 << (qreal)5.0 << (qreal)7.5
        << false;
}

void tst_QLineSegment3D::containsPoint()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(qreal, x3);
    QFETCH(qreal, y3);
    QFETCH(qreal, z3);
    QFETCH(bool, contained);

    QLineSegment3D segment(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QVERIFY(segment.contains(segment.start()));
    QVERIFY(segment.contains(segment.end()));
    if (contained)
        QVERIFY(segment.contains(QVector3D(x3, y3, z3)));
    else
        QVERIFY(!segment.contains(QVector3D(x3, y3, z3)));
}

void tst_QLineSegment3D::containsSegment_data()
{
    QTest::addColumn<qreal>("x1");
    QTest::addColumn<qreal>("y1");
    QTest::addColumn<qreal>("z1");
    QTest::addColumn<qreal>("x2");
    QTest::addColumn<qreal>("y2");
    QTest::addColumn<qreal>("z2");
    QTest::addColumn<qreal>("x3");
    QTest::addColumn<qreal>("y3");
    QTest::addColumn<qreal>("z3");
    QTest::addColumn<qreal>("x4");
    QTest::addColumn<qreal>("y4");
    QTest::addColumn<qreal>("z4");
    QTest::addColumn<bool>("contained");

    QTest::newRow("on line")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << true;

    QTest::newRow("on line 2")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << true;

    QTest::newRow("partial intersect")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << (qreal)2.5 << (qreal)5.0 << (qreal)7.5
        << false;

    QTest::newRow("partial intersect 2")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)0.5 << (qreal)1.0 << (qreal)1.5
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << false;

    QTest::newRow("parallel")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)2.5 << (qreal)3.5
        << (qreal)2.5 << (qreal)4.5 << (qreal)6.5
        << false;

    QTest::newRow("point")
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << true;

    QTest::newRow("point 2")
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << (qreal)1.5 << (qreal)3.0 << (qreal)4.5
        << (qreal)1.0 << (qreal)2.0 << (qreal)3.0
        << (qreal)2.0 << (qreal)4.0 << (qreal)6.0
        << false;
}

void tst_QLineSegment3D::containsSegment()
{
    QFETCH(qreal, x1);
    QFETCH(qreal, y1);
    QFETCH(qreal, z1);
    QFETCH(qreal, x2);
    QFETCH(qreal, y2);
    QFETCH(qreal, z2);
    QFETCH(qreal, x3);
    QFETCH(qreal, y3);
    QFETCH(qreal, z3);
    QFETCH(qreal, x4);
    QFETCH(qreal, y4);
    QFETCH(qreal, z4);
    QFETCH(bool, contained);

    QLineSegment3D segment1(QVector3D(x1, y1, z1), QVector3D(x2, y2, z2));
    QLineSegment3D segment2(QVector3D(x3, y3, z3), QVector3D(x4, y4, z4));
    QVERIFY(segment1.contains(segment1));
    if (contained)
        QVERIFY(segment1.contains(segment2));
    else
        QVERIFY(!segment1.contains(segment2));
}

void tst_QLineSegment3D::intersectSegment_data()
{
    QTest::addColumn<QVector3D>("segmentA_point1");
    QTest::addColumn<QVector3D>("segmentA_point2");
    QTest::addColumn<QVector3D>("segmentB_point1");
    QTest::addColumn<QVector3D>("segmentB_point2");
    QTest::addColumn<QVector3D>("intersection_point");
    QTest::addColumn<bool>("intersects");

    QTest::newRow("x-ax & ||y-ax (2-len segments) cross on x-axis")
        << QVector3D(1.0, 0.0, 0.0)
        << QVector3D(3.0, 0.0, 0.0)
        << QVector3D(2.0, 1.0, 0.0)
        << QVector3D(2.0, -1.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << true;

    QTest::newRow("x-axis & zero len segment lying on x-axis")
        << QVector3D(1.0, 0.0, 0.0)
        << QVector3D(3.0, 0.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << true;

    QTest::newRow("zero len segment lying on x-axis & x-axis")
        << QVector3D(2.0, 0.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << QVector3D(1.0, 0.0, 0.0)
        << QVector3D(3.0, 0.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << true;

    QTest::newRow("no intersection")
        << QVector3D(1.0, 0.0, 0.0)
        << QVector3D(3.0, 0.0, 0.0)
        << QVector3D(2.0, 1.0, 0.0)
        << QVector3D(2.0, 0.5, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << false;
}

void tst_QLineSegment3D::intersectSegment()
{
    QFETCH(QVector3D, segmentA_point1);
    QFETCH(QVector3D, segmentA_point2);
    QFETCH(QVector3D, segmentB_point1);
    QFETCH(QVector3D, segmentB_point2);
    QFETCH(QVector3D, intersection_point);
    QFETCH(bool, intersects);

    QLineSegment3D segment1(segmentA_point1, segmentA_point2);
    QLineSegment3D segment2(segmentB_point1, segmentB_point2);

    QResult<QVector3D> intersection = segment1.intersection(segment2);

    if (intersects) {
        QVERIFY(segment1.intersects(segment2));
        QVERIFY(intersection.isValid());
        QVERIFY(intersection.value() == intersection_point);
    } else {
        QVERIFY(!segment1.intersects(segment2));
        QVERIFY(!intersection.isValid());
        QVERIFY(intersection.validity() == QResult<QVector3D>::NoResult);
    }
}

void tst_QLineSegment3D::intersectLine_data()
{
    QTest::addColumn<QVector3D>("segment_point1");
    QTest::addColumn<QVector3D>("segment_point2");
    QTest::addColumn<QVector3D>("line_origin");
    QTest::addColumn<QVector3D>("line_direction");
    QTest::addColumn<QVector3D>("intersection_point");
    QTest::addColumn<bool>("intersects");

    QTest::newRow("x-ax 2-len segment & line ||y-ax cross on x-axis")
        << QVector3D(1.0, 0.0, 0.0)
        << QVector3D(3.0, 0.0, 0.0)
        << QVector3D(2.0, 1.0, 0.0)
        << QVector3D(0.0, -2.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << true;

    QTest::newRow("zero len segment lying on x-axis \"intersects\" x-axis")
        << QVector3D(2.0, 0.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << QVector3D(1.0, 0.0, 0.0)
        << QVector3D(3.0, 0.0, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << true;

    QTest::newRow("no intersection")
        << QVector3D(1.0, 0.0, 0.0)
        << QVector3D(4.0, 0.0, 0.0)
        << QVector3D(2.0, 1.0, 0.0)
        << QVector3D(0.0, -0.5, 0.0)
        << QVector3D(2.0, 0.0, 0.0)
        << true;
}

void tst_QLineSegment3D::intersectLine()
{
    QFETCH(QVector3D, segment_point1);
    QFETCH(QVector3D, segment_point2);
    QFETCH(QVector3D, line_origin);
    QFETCH(QVector3D, line_direction);
    QFETCH(QVector3D, intersection_point);
    QFETCH(bool, intersects);

    QLineSegment3D segment(segment_point1, segment_point2);
    QLine3D line(line_origin, line_direction);

    QResult<QVector3D> intersection = segment.intersection(line);

    if (intersects) {
        QVERIFY(segment.intersects(line));
        QVERIFY(intersection.isValid());
        QVERIFY(intersection.value() == intersection_point);
    } else {
        QVERIFY(segment.intersects(line));
        QVERIFY(!intersection.isValid());
        QVERIFY(intersection.validity() == QResult<QVector3D>::NoResult);
    }
}

QTEST_APPLESS_MAIN(tst_QLineSegment3D)

#include "tst_qlinesegment3d.moc"
