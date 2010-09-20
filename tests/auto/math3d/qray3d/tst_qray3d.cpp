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
#include "qray3d.h"

class tst_QRay3D : public QObject
{
    Q_OBJECT
public:
    tst_QRay3D() {}
    ~tst_QRay3D() {}

private slots:
    void create_data();
    void create();
    void projection_data();
    void projection();
    void point_data();
    void point();
    void contains_point_data();
    void contains_point();
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

// make failed QCOMPARE messages more detailed than "Compared values are not
// the same"
namespace QTest {

template <>
inline char *toString(const QVector3D& vec)
{
    return qstrdup(qPrintable(QString("(%1, %2, %3)").arg(vec.x()).arg(vec.y()).arg(vec.z())));
}

};

void tst_QRay3D::create_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("direction");

    // normalized direction vectors
    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(1.0f, 0.0f, 0.0f);

    QTest::newRow("line parallel -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -1.0f);

    QTest::newRow("vertical line (parallel to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 1.0f, 0.0f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.57735026919f, 0.57735026919f, 0.57735026919f);

    // non-normalized direction vectors
    QTest::newRow("line on x-axis from origin - B")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f);

    QTest::newRow("line parallel -z-axis from 3,3,3 - B")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f);

    QTest::newRow("vertical line (parallel to y-axis) - B")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f);

    QTest::newRow("equidistant from all 3 axes - B")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f);
}

void tst_QRay3D::create()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, direction);
    QRay3D ray(point, direction);
    QVERIFY(fuzzyCompare(ray.direction(), direction));
    QVERIFY(fuzzyCompare(ray.origin(), point));

    QRay3D ray2;
    QCOMPARE(ray2.origin(), QVector3D(0, 0, 0));
    QCOMPARE(ray2.direction(), QVector3D(1, 0, 0));
    ray2.setOrigin(point);
    ray2.setDirection(direction);
    QVERIFY(fuzzyCompare(ray.direction(), direction));
    QVERIFY(fuzzyCompare(ray.origin(), point));
}

void tst_QRay3D::projection_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("vector");
    QTest::addColumn<QVector3D>("expected");

    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(0.6f, 0.0f, 0.0f)
            << QVector3D(0.6f, 0.0f, 0.0f);

    QTest::newRow("line parallel -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(3.0f, 3.0f, 2.4f)
            << QVector3D(0.0f, 0.0f, 2.4f);

    QTest::newRow("vertical line (parallel to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f)
            << QVector3D(0.5f, 0.6f, 0.5f)
            << QVector3D(0.0f, 0.6f, 0.0f);

    QTest::newRow("equidistant from all 3 axes, project y-axis (with some z & x)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(0.5f, 5.0f, 0.5f)
            << QVector3D(2.0f, 2.0f, 2.0f);

    QTest::newRow("negative direction line, project +ve y-axis (with some z & x)")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f)
            << QVector3D(0.5f, 5.0f, 0.5f)
            << QVector3D(1.241379261016846f, 1.862068772315979f, 2.48275852203369f);
}

void tst_QRay3D::projection()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, vector);
    QFETCH(QVector3D, expected);
    QRay3D line(point, direction);
    QVector3D result = line.project(vector);
    QVERIFY(fuzzyCompare(result, expected));
}

void tst_QRay3D::point_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("point_on_line_pos_0_6");
    QTest::addColumn<QVector3D>("point_on_line_neg_7_2");

    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(1.2f, 0.0f, 0.0f)
            << QVector3D(-14.4f, 0.0f, 0.0f);

    QTest::newRow("line parallel -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(3.0f, 3.0f, 2.58f)
            << QVector3D(3.0f, 3.0f, 8.04f);

    QTest::newRow("vertical line (parallel to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f)
            << QVector3D(0.5f, 3.18f, 0.5f)
            << QVector3D(0.5f, -38.16f, 0.5f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(1.1f, 0.6f, 1.1f)
            << QVector3D(-6.7f, -7.2f, -6.7f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f)
            << QVector3D(-3.72f, -4.08f, -4.44f)
            << QVector3D(5.64f, 9.96f, 14.28f);
}

void tst_QRay3D::point()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point_on_line_pos_0_6);
    QFETCH(QVector3D, point_on_line_neg_7_2);
    QRay3D line(point, direction);
    QVERIFY(fuzzyCompare(line.point(0.6), point_on_line_pos_0_6));
    QVERIFY(fuzzyCompare(line.point(-7.2), point_on_line_neg_7_2));
    QVERIFY(fuzzyCompare(line.fromPoint(point_on_line_pos_0_6), 0.6));
    QVERIFY(fuzzyCompare(line.fromPoint(point_on_line_neg_7_2), -7.2));
}

void tst_QRay3D::contains_point_data()
{
    QTest::addColumn<QVector3D>("origin");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<bool>("contains");

    QTest::newRow("bogus this line with null direction")
            << QVector3D(1.0, 3.0, 3.0)
            << QVector3D(0.0, 0.0, 0.0)
            << QVector3D(1.0, 2.0, 4.0)
            << false;

    QTest::newRow("point at the origin")
            << QVector3D(0.0, 0.0, 0.0)
            << QVector3D(1.0, 3.0, 3.0)
            << QVector3D(0.0, 0.0, 0.0)
            << true;

    QTest::newRow("close to the origin")
            << QVector3D(1.0, 1.0, 1.0)
            << QVector3D(1.0, 3.0, 3.0)
            << QVector3D(1.0005, 1.0005, 1.0)
            << false;

    QTest::newRow("45 line line in plane x=1")
            << QVector3D(1.0, 3.0, 3.0)
            << QVector3D(0.0, -1.0, -1.0)
            << QVector3D(1.0, 4.0, 4.0)
            << true;
    {
        // This is to prove that the constructed approach give the
        // same results
        QVector3D p(1.0, 3.0, 3.0);
        QVector3D v(0.0, -1.0, -1.0);

        QTest::newRow("constructed 45 line line in plane x=1")
                << p
                << v
                << p + v
                << true;
    }

    QTest::newRow("intersection with negative s in plane z=-1")
        << QVector3D(1.0f, 2.0f, -1.0f)
        << QVector3D(1.0f, 1.0f, 0.0f)
        << QVector3D(2.0f, 1.0f, 0.0f)
        << false;

    QTest::newRow("45 angled line")
        << QVector3D(3.0f, 0.0f, -1.0f)
        << QVector3D(1.0f, -1.0f, 1.0f)
        << QVector3D(6.0f, -3.0f, 2.0f)
        << true;

    {
        QVector3D p(-10.0, 3.0, 3.0);
        QVector3D v(0.0, 20.0, -1.0);
        QTest::newRow("constructed vector close to axis")
                << p
                << v
                << p + v * 3.0
                << true;
    }

    {
        QVector3D p(1.0, 3.0, 3.0);
        QVector3D v(40.0, 500.0, -1.0);
        QTest::newRow("constructed larger values close to axis")
                << p
                << v
                << p + v
                << true;
    }
}

void tst_QRay3D::contains_point()
{
    QFETCH(QVector3D, origin);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point);
    QFETCH(bool, contains);

    QRay3D line(origin, direction);
    QCOMPARE(line.contains(point), contains);
}

QTEST_APPLESS_MAIN(tst_QRay3D)

#include "tst_qray3d.moc"
