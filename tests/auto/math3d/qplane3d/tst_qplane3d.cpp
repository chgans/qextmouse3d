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

#include "qplane3d.h"
#include "qray3d.h"

class tst_QPlane3D : public QObject
{
    Q_OBJECT
public:
    tst_QPlane3D() {}
    ~tst_QPlane3D() {}

private slots:
    void create_data();
    void create();
    void intersection_data();
    void intersection();
    void noIntersection_data();
    void noIntersection();
    void properties();
    void metaTypes();
};

// since all calculations involved QVector3D are producing values with only
// float precision those calculations can at best be float precision
// if you assign the results of the calculation to a qreal then qFuzzyCompare
// will quite happily use a much higher standard of precision than it is
// possible to acheive - hence redefine it here to always use the float
// Also while on the job fix the problem where a compared value happens
// to be zero (and you cannot always predict this, and should not predict it
// since then you produce self-fulling prophecies instead of tests).
// In that case qFuzzyCompare has a completely strict criterion since
// it finds the "fudge factor" by multiplying by zero...
static inline bool fuzzyCompare(qreal p1, qreal p2)
{
    float fac = qMin(qAbs(p1), qAbs(p2));
    return (qAbs(p1 - p2) <= (qIsNull(fac) ? 0.00001f : 0.00001f * fac));
}

static inline bool fuzzyCompare(const QVector3D &lhs, const QVector3D &rhs)
{
    if (fuzzyCompare(lhs.x(), rhs.x()) &&
            fuzzyCompare(lhs.y(), rhs.y()) &&
            fuzzyCompare(lhs.z(), rhs.z()))
        return true;
    qWarning() << "actual:" << lhs;
    qWarning() << "expected:" << rhs;
    return false;
}

void tst_QPlane3D::create_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("normal");

    // normalized vectors for the normals.
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

    // Unnormalized vectors for the normals.
    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f);

    QTest::newRow("line paralell -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f);

    QTest::newRow("vertical line (paralell to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f);
}

void tst_QPlane3D::create()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, normal);
    QPlane3D plane(point, normal);
    QVERIFY(fuzzyCompare(plane.normal(), normal));
    QVERIFY(fuzzyCompare(plane.origin(), point));
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

    QRay3D line(point1, direction);
    QPlane3D plane(point2, normal);

    qreal t = plane.intersection(line);
    QVERIFY(!qIsNaN(t));
    QVERIFY(fuzzyCompare(line.point(t), intersection));
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
    QRay3D line(point2, direction);

    qreal t = plane.intersection(line);
    QVERIFY(qIsNaN(t));
}

class tst_QPlane3DProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QPlane3D plane READ plane WRITE setPlane)
public:
    tst_QPlane3DProperties(QObject *parent = 0) : QObject(parent) {}

    QPlane3D plane() const { return p; }
    void setPlane(const QPlane3D& value) { p = value; }

private:
    QPlane3D p;
};

// Test getting and setting properties via the metaobject system.
void tst_QPlane3D::properties()
{
    tst_QPlane3DProperties obj;

    qRegisterMetaType<QPlane3D>();

    obj.setPlane(QPlane3D(QVector3D(1, 2, 3), QVector3D(4, 5, 6)));

    QPlane3D p = qVariantValue<QPlane3D>(obj.property("plane"));
    QCOMPARE(p.origin(), QVector3D(1, 2, 3));
    QCOMPARE(p.normal(), QVector3D(4, 5, 6));

    obj.setProperty("plane",
                    qVariantFromValue
                        (QPlane3D(QVector3D(-1, -2, -3), QVector3D(-4, -5, -6))));

    p = qVariantValue<QPlane3D>(obj.property("plane"));
    QCOMPARE(p.origin(), QVector3D(-1, -2, -3));
    QCOMPARE(p.normal(), QVector3D(-4, -5, -6));
}

void tst_QPlane3D::metaTypes()
{
    int id = qMetaTypeId<QPlane3D>();
    QVERIFY(QMetaType::type("QPlane3D") == id);
    QCOMPARE(QByteArray(QMetaType::typeName(id)), QByteArray("QPlane3D"));
    QVERIFY(QMetaType::isRegistered(id));
}

QTEST_APPLESS_MAIN(tst_QPlane3D)

#include "tst_qplane3d.moc"
