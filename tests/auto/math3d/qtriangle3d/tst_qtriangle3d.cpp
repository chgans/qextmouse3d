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
#include "qtriangle3d.h"
#include "qplane3d.h"

class tst_QTriangle3D : public QObject
{
    Q_OBJECT
public:
    tst_QTriangle3D() {}
    ~tst_QTriangle3D() {}

private slots:
    void create_data();
    void create();

    void contains_data();
    void contains();

    void intersect_data();
    void intersect();

    void uv_data();
    void uv();

    void properties();
    void metaTypes();
};

void tst_QTriangle3D::create_data()
{
    QTest::addColumn<QVector3D>("p");
    QTest::addColumn<QVector3D>("q");
    QTest::addColumn<QVector3D>("r");
    QTest::addColumn<bool>("isDegenerate");
    QTest::addColumn<QPlane3D>("plane");
    QTest::addColumn<QVector3D>("centroid");

    QTest::newRow("minimal at null")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 0.0f);
    QTest::newRow("minimal at offset")
         << QVector3D(2.0f, 2.0f, 2.0f) // p
         << QVector3D(2.0f, 2.0f, 2.0f) // q
         << QVector3D(2.0f, 2.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(2.0f, 2.0f, 2.0f); // centroid
    QTest::newRow("line from 0(p) [a]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.5f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.5f/3.0f); // centroid
    QTest::newRow("line from 0(p) [b]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 1.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 1.0f); // centroid
    QTest::newRow("line from 0(p) [c]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 1.5f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 3.5f/3.0f); // centroid
    QTest::newRow("line from 0(q) [a]")
         << QVector3D(0.0f, 0.0f, 0.5f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.5f/3.0f); // centroid
    QTest::newRow("line from 0(q) [b]")
         << QVector3D(0.0f, 0.0f, 1.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 1.0f); // centroid
    QTest::newRow("line from 0(q) [c]")
         << QVector3D(0.0f, 0.0f, 1.5f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 3.5f/3.0f); // centroid
    QTest::newRow("line from 0(r) [a]")
         << QVector3D(0.0f, 0.0f, 0.5f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.5f/3.0f); // centroid
    QTest::newRow("line from 0(r) [b]")
         << QVector3D(0.0f, 0.0f, 1.0f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 1.0f); // centroid
    QTest::newRow("line from 0(r) [c]")
         << QVector3D(0.0f, 0.0f, 1.5f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 3.5f/3.0f); // centroid
    QTest::newRow("width 0 [a]")
         << QVector3D(0.0f, 0.0f, 2.0f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 4.0f/3.0f); // centroid
    QTest::newRow("width 0 [b]")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 2.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 2.0f/3.0f); // centroid
    QTest::newRow("width 0 [c]")
         << QVector3D(0.0f, 0.0f, 2.0f) // p
         << QVector3D(0.0f, 0.0f, 2.0f) // q
         << QVector3D(0.0f, 0.0f, 4.0f) // r
         << true // degenerate
         << QPlane3D() // plane (degenerate, meaning meaningless)
         << QVector3D(0.0f, 0.0f, 8.0f/3.0f); // centroid

    /*  The above only tests degenerate triangles colinear with the z axis.
        May also want to tests degenerate triangles offset from origin and
        degenerate triangles on other angles (e.g. x axis, y axis, not colinear
        with any axis)
        */

    QTest::newRow("simple at origin")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 4.0f, 0.0f) // q
         << QVector3D(0.0f, 2.0f, 5.0f) // r
         << false // degenerate
         << QPlane3D(QVector3D(0.0f, 0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)) // plane
         << QVector3D(0.0f, 6.0f/3.0f, 5.0f/3.0f); // centroid

    QTest::newRow("simple offset from origin")
         << QVector3D(1.0f, 1.0f, 0.0f) // p
         << QVector3D(1.0f, 5.0f, 0.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << false // degenerate
         << QPlane3D(QVector3D(1.0f, 0.0f, 0.0f), QVector3D(1.0f, 0.0f, 0.0f)) // plane
         << QVector3D(1.0f, 3.0f, 5.0f/3.0f); // centroid
}

void tst_QTriangle3D::create()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);
    QFETCH(bool, isDegenerate);
    QFETCH(QPlane3D, plane);
    QFETCH(QVector3D, centroid);

    QTriangle3D triangle(p, q, r);

    QCOMPARE(triangle.p(), p);
    QCOMPARE(triangle.q(), q);
    QCOMPARE(triangle.r(), r);

    //QCOMPARE(triangle.isDegenerate(), isDegenerate);

    //QCOMPARE(triangle.width(), width);
    //QCOMPARE(triangle.isDegenerate() ? 0.0f : triangle.height(), height);

    if (!isDegenerate) {
        QPlane3D trianglePlane = triangle.plane();
        QVERIFY(plane.contains(trianglePlane.origin()));
        QCOMPARE(plane.normal().normalized(),
                 trianglePlane.normal().normalized());
    }
    QCOMPARE(triangle.center(), centroid);
}

void tst_QTriangle3D::contains_data()
{
    QTest::addColumn<QVector3D>("p");
    QTest::addColumn<QVector3D>("q");
    QTest::addColumn<QVector3D>("r");
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<bool>("contains");

    QTest::newRow("minimal at null, same")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << QVector3D(0.0f, 0.0f, 0.0f) // point
         << false; // contains
    QTest::newRow("acute at p")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.0f, 2.0f) // point
         << true; // contains
    QTest::newRow("acute at q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.0f, 4.0f) // point
         << true; // contains
    QTest::newRow("acute at r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 3.0f, 3.0f) // point
         << true; // contains
    QTest::newRow("acute at p-q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.0f, 3.0f) // point
         << true; // contains
    QTest::newRow("acute at p-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.5f, 2.5f) // point
         << true; // contains
    QTest::newRow("acute at q-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.5f, 3.5f) // point
         << true; // contains
    QTest::newRow("acute internal")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.5f, 3.0f) // point
         << true; // contains
    QTest::newRow("acute external a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.8f, 2.0f) // point
         << false; // contains
    QTest::newRow("acute external b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 2.8f, 4.0f) // point
         << false; // contains
    QTest::newRow("acute external c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.0f, 1.4f, 3.0f) // point
         << false; // contains
    QTest::newRow("acute above plane")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(1.3f, 2.5f, 3.0f) // point
         << false; // contains
    QTest::newRow("acute below plane")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.8f, 2.5f, 3.0f) // point
         << false; // contains
    QTest::newRow("obtuse a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.0f, 2.5f, 4.8f) // point
         << false; // contains
    QTest::newRow("obtuse b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.0f, 2.5f, 4.3f) // point
         << true; // contains
    QTest::newRow("obtuse c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.0f, 2.5f, 4.5f) // point
         << true; // contains
    QTest::newRow("obtuse d")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(1.2f, 2.5f, 4.3f) // point
         << false; // contains
    QTest::newRow("obtuse e")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.8f, 2.5f, 4.3f) // point
         << false; // contains
}

void tst_QTriangle3D::contains()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);
    QFETCH(QVector3D, point);
    QFETCH(bool, contains);
    // qDebug("Triangle: P(%0.10f, %0.10f, %0.10f) Q(%0.10f, %0.10f, %0.10f) R(%0.10f, %0.10f, %0.10f)",
    //       p.x(), p.y(), p.z(), q.x(), q.y(), q.z(), r.x(), r.y(), r.z());
    // qDebug("Point: (%0.10f, %0.10f, %0.10f)", point.x(), point.y(), point.z());
    QCOMPARE(QTriangle3D(p, q, r).contains(point), contains);
}

void tst_QTriangle3D::intersect_data()
{
    QTest::addColumn<QVector3D>("p");
    QTest::addColumn<QVector3D>("q");
    QTest::addColumn<QVector3D>("r");
    QTest::addColumn<QVector3D>("origin");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("intersection");
    QTest::addColumn<bool>("doesIntersect");

    QTest::newRow("minimal at null, same")
         << QVector3D(0.0f, 0.0f, 0.0f) // p
         << QVector3D(0.0f, 0.0f, 0.0f) // q
         << QVector3D(0.0f, 0.0f, 0.0f) // r
         << QVector3D(0.0f, 0.0f, 0.0f) // origin
         << QVector3D(0.0f, 0.0f, 1.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // degenerate triangles cannot intersect

    /*
        lines perpendicular to triangle plane.
    */
    QTest::newRow("acute at p")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.0f, 2.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.0f, 2.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.0f, 4.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.0f, 4.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 3.0f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 3.0f, 3.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at p-q")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.0f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.0f, 3.0f) // intersection
         << true; // intersects
    QTest::newRow("acute at p-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.5f, 2.5f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 2.5f) // intersection
         << true; // intersects
    QTest::newRow("acute at q-r")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.5f, 3.5f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 3.5f) // intersection
         << true; // intersects
    QTest::newRow("acute internal")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.5f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 3.0f) // intersection
         << true; // intersects
    QTest::newRow("acute external a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.8f, 2.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("acute external b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 2.8f, 4.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("acute external c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 3.0f) // r
         << QVector3D(0.0f, 1.4f, 3.0f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("obtuse a")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.0f, 2.5f, 4.8f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(0.0f, 0.0f, 0.0f) // intersection
         << false; // intersects
    QTest::newRow("obtuse b")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.0f, 2.5f, 4.3f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 4.3f) // intersection
         << true; // intersects
    QTest::newRow("obtuse c")
         << QVector3D(1.0f, 2.0f, 2.0f) // p
         << QVector3D(1.0f, 2.0f, 4.0f) // q
         << QVector3D(1.0f, 3.0f, 5.0f) // r
         << QVector3D(0.0f, 2.5f, 4.5f) // origin
         << QVector3D(1.0f, 0.0f, 0.0f) // direction
         << QVector3D(1.0f, 2.5f, 4.5f) // intersection
         << true; // intersects
}

void tst_QTriangle3D::intersect()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);
    QFETCH(QVector3D, origin);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, intersection);
    QFETCH(bool, doesIntersect);

    QTriangle3D triangle(p, q, r);
    QRay3D line(origin, direction);
    QCOMPARE(triangle.intersects(line), doesIntersect);

    qreal result = triangle.intersection(line);
    if (doesIntersect)
        QCOMPARE(line.point(result), intersection);
    else
        QVERIFY(qIsNaN(result));
}

void tst_QTriangle3D::uv_data()
{
    contains_data();
}

void tst_QTriangle3D::uv()
{
    QFETCH(QVector3D, p);
    QFETCH(QVector3D, q);
    QFETCH(QVector3D, r);

    if (p == q || p == r || q == r)
        return;     // Ignore degenerate triangles for this test.

    QTriangle3D triangle(p, q, r);

    QCOMPARE(triangle.uv(p), QVector2D(1, 0));
    QCOMPARE(triangle.uv(q), QVector2D(0, 1));
    QCOMPARE(triangle.uv(r), QVector2D(0, 0));

    QCOMPARE(triangle.uv((p + q) / 2.0f), QVector2D(0.5f, 0.5f));
    QCOMPARE(triangle.uv((p + r) / 2.0f), QVector2D(0.5f, 0.0f));
    QCOMPARE(triangle.uv((q + r) / 2.0f), QVector2D(0.0f, 0.5f));

    QVector2D v1(triangle.uv((p + q + r) / 3.0f));
    QVector2D v2(1.0f / 3.0f, 1.0f / 3.0f);
    QVERIFY(qFuzzyCompare(float(v1.x()), float(v2.x())));
    QVERIFY(qFuzzyCompare(float(v1.y()), float(v2.y())));
}

class tst_QTriangle3DProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QTriangle3D triangle READ triangle WRITE setTriangle)
public:
    tst_QTriangle3DProperties(QObject *parent = 0) : QObject(parent) {}

    QTriangle3D triangle() const { return t; }
    void setTriangle(const QTriangle3D& value) { t = value; }

private:
    QTriangle3D t;
};

// Test getting and setting properties via the metaobject system.
void tst_QTriangle3D::properties()
{
    tst_QTriangle3DProperties obj;

    qRegisterMetaType<QTriangle3D>();

    obj.setTriangle(QTriangle3D(QVector3D(1, 2, 3), QVector3D(4, 5, 6),
                                QVector3D(7, 8, 9)));

    QTriangle3D t = qVariantValue<QTriangle3D>(obj.property("triangle"));
    QCOMPARE(t.p(), QVector3D(1, 2, 3));
    QCOMPARE(t.q(), QVector3D(4, 5, 6));
    QCOMPARE(t.r(), QVector3D(7, 8, 9));

    obj.setProperty("triangle",
                    qVariantFromValue
                        (QTriangle3D(QVector3D(-1, -2, -3),
                                     QVector3D(-4, -5, -6),
                                     QVector3D(-7, -8, -9))));

    t = qVariantValue<QTriangle3D>(obj.property("triangle"));
    QCOMPARE(t.p(), QVector3D(-1, -2, -3));
    QCOMPARE(t.q(), QVector3D(-4, -5, -6));
    QCOMPARE(t.r(), QVector3D(-7, -8, -9));
}

void tst_QTriangle3D::metaTypes()
{
    int id = qMetaTypeId<QTriangle3D>();
    QVERIFY(QMetaType::type("QTriangle3D") == id);
    QCOMPARE(QByteArray(QMetaType::typeName(id)), QByteArray("QTriangle3D"));
    QVERIFY(QMetaType::isRegistered(id));
}

QTEST_APPLESS_MAIN(tst_QTriangle3D)

#include "tst_qtriangle3d.moc"
