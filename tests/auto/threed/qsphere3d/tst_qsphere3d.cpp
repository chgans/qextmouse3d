/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qsphere3d.h"
#include <QtGui/qmatrix4x4.h>

class tst_QSphere3D : public QObject
{
    Q_OBJECT
public:
    tst_QSphere3D() {}
    ~tst_QSphere3D() {}

private slots:
    void create();
    void contains_data();
    void contains();
    void compare();
    void dataStream();
    void transform();
    void properties();
    void metaTypes();
};

void tst_QSphere3D::create()
{
    QSphere3D sphere1;
    QCOMPARE(sphere1.center(), QVector3D(0, 0, 0));
    QCOMPARE(sphere1.radius(), qreal(1.0f));

    QSphere3D sphere2(QVector3D(1.0, 2.5f, -4.0f), 5.25f);
    QCOMPARE(sphere2.center(), QVector3D(1.0f, 2.5f, -4.0f));
    QCOMPARE(sphere2.radius(), qreal(5.25f));

    sphere2.setCenter(QVector3D(-1.0, -2.5f, 4.0f));
    sphere2.setRadius(10.0f);
    QCOMPARE(sphere2.center(), QVector3D(-1.0f, -2.5f, 4.0f));
    QCOMPARE(sphere2.radius(), qreal(10.0f));
}

void tst_QSphere3D::contains_data()
{
    QTest::addColumn<QVector3D>("center");
    QTest::addColumn<qreal>("radius");
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<bool>("contained");

    QTest::newRow("origin-inside")
        << QVector3D(0, 0, 0) << qreal(0)
        << QVector3D(0, 0, 0) << true;
    QTest::newRow("origin-outside")
        << QVector3D(0, 0, 0) << qreal(0)
        << QVector3D(0, 0.25f, 0) << false;

    QTest::newRow("general-inside")
        << QVector3D(1, 2, 3) << qreal(5)
        << QVector3D(1, 2, 3) << true;

    QTest::newRow("general-edge-x")
        << QVector3D(1, 2, 3) << qreal(5)
        << QVector3D(6, 2, 3) << true;
    QTest::newRow("general-edge-y")
        << QVector3D(1, 2, 3) << qreal(5)
        << QVector3D(1, -3, 3) << true;
    QTest::newRow("general-edge-z")
        << QVector3D(1, 2, 3) << qreal(5)
        << QVector3D(1, 2, 8) << true;

    QTest::newRow("general-outside-x")
        << QVector3D(1, 2, 3) << qreal(5)
        << QVector3D(6.25f, 2, 3) << false;
    QTest::newRow("general-outside-y")
        << QVector3D(1, 2, 3) << qreal(5)
        << QVector3D(1, -3.5f, 3) << false;
    QTest::newRow("general-outside-z")
        << QVector3D(1, 2, 3) << qreal(5)
        << QVector3D(1, 2, 8.25f) << false;
}

void tst_QSphere3D::contains()
{
    QFETCH(QVector3D, center);
    QFETCH(qreal, radius);
    QFETCH(QVector3D, point);
    QFETCH(bool, contained);

    QSphere3D sphere(center, radius);
    QCOMPARE(sphere.contains(point), contained);
}

void tst_QSphere3D::compare()
{
    QSphere3D sphere1(QVector3D(10, 20, 30), 5.25f);
    QSphere3D sphere2(QVector3D(10, 20, 30), 1.0f);
    QSphere3D sphere3(QVector3D(0, 20, 30), 5.25f);
    QVERIFY(sphere1 == sphere1);
    QVERIFY(!(sphere1 != sphere1));
    QVERIFY(qFuzzyCompare(sphere1, sphere1));
    QVERIFY(sphere1 != sphere2);
    QVERIFY(!(sphere1 == sphere2));
    QVERIFY(!qFuzzyCompare(sphere1, sphere2));
    QVERIFY(sphere1 != sphere3);
    QVERIFY(!(sphere1 == sphere3));
    QVERIFY(!qFuzzyCompare(sphere1, sphere3));
}

void tst_QSphere3D::dataStream()
{
#ifndef QT_NO_DATASTREAM
    QSphere3D sphere(QVector3D(1.0f, 2.0f, 3.0f), 4.0f);

    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << sphere;
    }

    QSphere3D sphere2;
    {
        QDataStream stream2(data);
        stream2 >> sphere2;
    }

    QVERIFY(sphere == sphere2);
#endif
}

void tst_QSphere3D::transform()
{
    QVector3D center(1, 2, 3);
    qreal radius = 5.25f;

    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);

    QSphere3D sphere1(center, radius);
    QSphere3D sphere2(sphere1);
    QSphere3D sphere3;

    sphere1.transform(m);
    sphere3 = sphere2.transformed(m);

    QCOMPARE(sphere1.center(), sphere3.center());
    QCOMPARE(sphere1.radius(), sphere3.radius());

    qreal tradius = m.mapVector(QVector3D(0, radius, 0)).length();

    QCOMPARE(sphere1.center(), m * center);
    QCOMPARE(float(sphere1.radius()), float(tradius));
}

class tst_QSphere3DProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QSphere3D sphere READ sphere WRITE setSphere)
public:
    tst_QSphere3DProperties(QObject *parent = 0) : QObject(parent) {}

    QSphere3D sphere() const { return s; }
    void setSphere(const QSphere3D& value) { s = value; }

private:
    QSphere3D s;
};

// Test getting and setting properties via the metaobject system.
void tst_QSphere3D::properties()
{
    tst_QSphere3DProperties obj;

    qRegisterMetaType<QSphere3D>();

    obj.setSphere(QSphere3D(QVector3D(1, 2, 3), 4));

    QSphere3D s = qVariantValue<QSphere3D>(obj.property("sphere"));
    QCOMPARE(s.center(), QVector3D(1, 2, 3));
    QCOMPARE(s.radius(), qreal(4.0f));

    obj.setProperty("sphere",
                    qVariantFromValue(QSphere3D(QVector3D(-1, -2, -3), -4)));

    s = qVariantValue<QSphere3D>(obj.property("sphere"));
    QCOMPARE(s.center(), QVector3D(-1, -2, -3));
    QCOMPARE(s.radius(), qreal(-4.0f));
}

void tst_QSphere3D::metaTypes()
{
    int id = qMetaTypeId<QSphere3D>();
    QVERIFY(QMetaType::type("QSphere3D") == id);
    QCOMPARE(QByteArray(QMetaType::typeName(id)), QByteArray("QSphere3D"));
    QVERIFY(QMetaType::isRegistered(id));
}

QTEST_APPLESS_MAIN(tst_QSphere3D)

#include "tst_qsphere3d.moc"
