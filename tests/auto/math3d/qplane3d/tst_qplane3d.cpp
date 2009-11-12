/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "qplane3d.h"
#include "qline3d.h"

class tst_QPlane3D : public QObject
{
    Q_OBJECT
public:
    tst_QPlane3D() {}
    ~tst_QPlane3D() {}

private slots:
    void create_data();
    void create();
    void normalized_data();
    void normalized();
    void intersection_data();
    void intersection();
    void noIntersection_data();
    void noIntersection();
};

void tst_QPlane3D::create_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("normal");

    // Note - these vectors are all pre-normalized
    // and therefore should not change on construction
    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(1.0f, 0.0f, 0.0f);

    QTest::newRow("line paralell -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -1.0f);

    QTest::newRow("vertical line (paralell to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 1.0f, 0.0f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.57735026919f, 0.57735026919f, 0.57735026919f);
}

void tst_QPlane3D::create()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, normal);
    QPlane3D plane(point, normal);
    QCOMPARE(plane.normal(), normal);
    QCOMPARE(plane.origin(), point);
}

void tst_QPlane3D::normalized_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("normal");
    QTest::addColumn<QVector3D>("normalized");

    // These direction vectors will get normalized
    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(1.0f, 0.0f, 0.0f);

    QTest::newRow("line paralell -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(0.0f, 0.0f, -1.0f);

    QTest::newRow("vertical line (paralell to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f)
            << QVector3D(0.0f, 1.0f, 0.0f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(0.57735026919f, 0.57735026919f, 0.57735026919f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f)
            << QVector3D(-0.3713906763f, -0.55708601453f, -0.7427813527f);
}

void tst_QPlane3D::normalized()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, normal);
    QFETCH(QVector3D, normalized);
    QPlane3D plane(point, normal);
    QVERIFY(qFuzzyCompare(plane.normal(), normalized));
    QCOMPARE(plane.origin(), point);
}

void tst_QPlane3D::intersection_data()
{
    // Line
    QTest::addColumn<QVector3D>("point1");
    QTest::addColumn<QVector3D>("direction");

    // Plane
    QTest::addColumn<QVector3D>("point2");
    QTest::addColumn<QVector3D>("normal");

    // Resulting intersection
    QTest::addColumn<QVector3D>("intersection");

    // These direction vectors will get normalized
    QTest::newRow("line on x-axis, plane in z-y")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(4.0f, 1.0f, 1.0f)
            << QVector3D(1.0f, 0.0f, 0.0f)
            << QVector3D(4.0f, 0.0f, 0.0f);

    QTest::newRow("line -z-axis, plane in x-y")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(1.0f, 1.0f, -2.4f)
            << QVector3D(0.0f, 0.0f, -1.0f)
            << QVector3D(3.0f, 3.0f, -2.4f);

    QTest::newRow("line y-axis, plane in x-z")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f)
            << QVector3D(1.5f, 0.6f, 1.5f)
            << QVector3D(0.0f, -7.2f, 0.0f)
            << QVector3D(0.5f, 0.6f, 0.5f);

    QTest::newRow("line equidistant from axes, plane in y-z")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(5.0f, 3.0f, 3.0f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(3.8333332539f, 3.3333332539f, 3.8333332539f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f)
            << QVector3D(5.0f, 3.0f, 3.0f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D( 1.4444446564f, 3.6666665077f, 5.8888893127f);
}

void tst_QPlane3D::intersection()
{
    QFETCH(QVector3D, point1);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point2);
    QFETCH(QVector3D, normal);
    QFETCH(QVector3D, intersection);

    QLine3D line(point1, direction);
    QPlane3D plane(point2, normal);

    QResult<QVector3D> res = plane.intersection(line);
    // qDebug("got: %0.10f, %0.10f, %0.10f -- exp: %0.10f, %0.10f, %0.10f",
    //        res.value().x(), res.value().y(), res.value().z(),
    //        intersection.x(), intersection.y(), intersection.z());
    QVERIFY(res.isValid());
    QCOMPARE(res.value(), intersection);
    res = plane.intersection(line);
    QVERIFY(res.isValid());
    QCOMPARE(res.value(), intersection);
}

void tst_QPlane3D::noIntersection_data()
{
    QTest::addColumn<QVector3D>("point1");
    QTest::addColumn<QVector3D>("normal");
    QTest::addColumn<QVector3D>("point2");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("intersection");

    // These direction vectors will get normalized
    QTest::newRow("line on x-axis, plane in z-x")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(4.0f, 1.0f, 1.0f)
            << QVector3D(0.0f, 1.0f, 0.0f);

    QTest::newRow("line -z-axis, lies on plane in z-x")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(1.0f, 3.0f, 1.0f)
            << QVector3D(0.0f, -0.7f, 0.0f);

    QTest::newRow("line on an angle, never meets plane on angle")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.1f, 0.0f, 0.0f)
            << QVector3D(1.0f, 1.0f, 3.0f)
            << QVector3D(0.0f, 1.0f, 0.0f);
}

void tst_QPlane3D::noIntersection()
{
    QFETCH(QVector3D, point1);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point2);
    QFETCH(QVector3D, normal);

    QPlane3D plane(point1, normal);
    QLine3D line(point2, direction);

    QResult<QVector3D> res = plane.intersection(line);
    QVERIFY(!res.isValid());
    QCOMPARE(res.value(), QVector3D());
    res = plane.intersection(line);
    QVERIFY(!res.isValid());
    QCOMPARE(res.value(), QVector3D());
}

QTEST_APPLESS_MAIN(tst_QPlane3D)

#include "tst_qplane3d.moc"
