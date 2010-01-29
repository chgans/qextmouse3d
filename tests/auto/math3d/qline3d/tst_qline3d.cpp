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
#include "qline3d.h"

extern char debug_out_str[4096];

class tst_QLine3D : public QObject
{
    Q_OBJECT
public:
    tst_QLine3D() {}
    ~tst_QLine3D() {}


private slots:
    void create_data();
    void create();
    void normalized_data();
    void normalized();
    void projection_data();
    void projection();
    void point_data();
    void point();
    void intersection_data();
    void intersection();
    void contains_point_data();
    void contains_point();
    void contains_line_data();
    void contains_line();
    void sameSide();
    void distance();
    void distanceFromOrigin();
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
    return fuzzyCompare(lhs.x(), rhs.x()) && fuzzyCompare(lhs.y(), rhs.y()) && fuzzyCompare(lhs.z(), rhs.z());
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

void tst_QLine3D::create_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("direction");

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

void tst_QLine3D::create()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, direction);
    QLine3D line(point, direction);
    QCOMPARE(line.direction(), direction);
    QCOMPARE(line.origin(), point);
}

void tst_QLine3D::normalized_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("direction");
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

void tst_QLine3D::normalized()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, normalized);
    QLine3D line(point, direction);
    // Recently QLine3D was changed so that the direction vector is not
    // internally maintained in a normalized state.  This test is retained
    // in this state - although it is something of a self-fulfilling prophecy.
    QVERIFY(qFuzzyCompare(line.direction().normalized(), normalized));
    QCOMPARE(line.origin(), point);
}

void tst_QLine3D::projection_data()
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

    QTest::newRow("line paralell -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(3.0f, 3.0f, 2.4f)
            << QVector3D(0.0f, 0.0f, 2.4f);

    QTest::newRow("vertical line (paralell to y-axis)")
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

void tst_QLine3D::projection()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, vector);
    QFETCH(QVector3D, expected);
    QLine3D line(point, direction);
    QVector3D result = line.project(vector);
    QVERIFY(fuzzyCompare(result, expected));
}

void tst_QLine3D::point_data()
{
    QTest::addColumn<QVector3D>("point");
    QTest::addColumn<QVector3D>("direction");
    QTest::addColumn<QVector3D>("point_on_line_pos_0_6");
    QTest::addColumn<QVector3D>("point_on_line_neg_7_2");

    QTest::newRow("line on x-axis from origin")
            << QVector3D()
            << QVector3D(2.0f, 0.0f, 0.0f)
            << QVector3D(0.6f, 0.0f, 0.0f)
            << QVector3D(-7.2f, 0.0f, 0.0f);

    QTest::newRow("line paralell -z-axis from 3,3,3")
            << QVector3D(3.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, -0.7f)
            << QVector3D(3.0f, 3.0f, 2.4f)
            << QVector3D(3.0f, 3.0f, 10.2f);

    QTest::newRow("vertical line (paralell to y-axis)")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(0.0f, 5.3f, 0.0f)
            << QVector3D(0.5f, 0.6f, 0.5f)
            << QVector3D(0.5f, -7.2f, 0.5f);

    QTest::newRow("equidistant from all 3 axes")
            << QVector3D(0.5f, 0.0f, 0.5f)
            << QVector3D(1.0f, 1.0f, 1.0f)
            << QVector3D(0.846410161514f, 0.346410161514f, 0.846410161514f)
            << QVector3D(-3.656921938168f, -4.156921938168f, -3.656921938168f);

    QTest::newRow("negative direction")
            << QVector3D(-3.0f, -3.0f, -3.0f)
            << QVector3D(-1.2f, -1.8f, -2.4f)
            << QVector3D(-3.2228343487f, -3.3342516422f, -3.4456686974f)
            << QVector3D(-0.3259873390f, 1.0110192299f, 2.3480253219f);
}

void tst_QLine3D::point()
{
    QFETCH(QVector3D, point);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point_on_line_pos_0_6);
    QFETCH(QVector3D, point_on_line_neg_7_2);
    QLine3D line(point, direction);
    QVERIFY(fuzzyCompare(line.point(0.6), point_on_line_pos_0_6));
    QVERIFY(fuzzyCompare(line.point(-7.2), point_on_line_neg_7_2));
}

void tst_QLine3D::intersection_data()
{
    QTest::addColumn<QVector3D>("point1");
    QTest::addColumn<QVector3D>("direction1");
    QTest::addColumn<QVector3D>("point2");
    QTest::addColumn<QVector3D>("direction2");
    QTest::addColumn<int>("validity");
    QTest::addColumn<QVector3D>("intersection");

    QTest::newRow("bogus this line with null direction")
            << QVector3D(1.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, 0.0f, 0.0f)
            << QVector3D(1.0f, 2.0f, 4.0f)
            << QVector3D(0.0f, 0.0f, -1.0f)
            << int(QResult<QVector3D>::InvalidArgument)
            << QVector3D(0.0f, 0.0f, 0.0f);

    QTest::newRow("bogus argument line with null direction")
            << QVector3D(1.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, -1.0f, -1.0f)
            << QVector3D(1.0f, 2.0f, 4.0f)
            << QVector3D(0.0f, 0.0f, 0.0f)
            << int(QResult<QVector3D>::InvalidArgument)
            << QVector3D(0.0f, 0.0f, 0.0f);

    QTest::newRow("45 line and horiz line in plane x=1")
            << QVector3D(1.0f, 3.0f, 3.0f)
            << QVector3D(0.0f, -1.0f, -1.0f)
            << QVector3D(1.0f, 2.0f, 4.0f)
            << QVector3D(0.0f, 0.0f, -1.0f)
            << int(QResult<QVector3D>::ResultExists)
            << QVector3D(1.0f, 2.0f, 2.0f);

    QTest::newRow("intersection with negative s in plane z=-1")
        << QVector3D(1.0f, 2.0f, -1.0f)
        << QVector3D(1.0f, 1.0f, 0.0f)
        << QVector3D(1.0f, 1.0f, -1.0f)
        << QVector3D(2.0f, 1.0f, 0.0f)
        << int(QResult<QVector3D>::ResultExists)
        << QVector3D(-1.0f, 0.0f, -1.0f);

    {
        // This is to prove that the constructed approach give the
        // same results
        QVector3D inter(-1.0, 0.0, -1.0);

        QVector3D v(1.0, 1.0, 0.0);
        QVector3D w(2.0, 1.0, 0.0);

        QLine3D l1(inter, v);
        QLine3D l2(inter, w);

        QTest::newRow("constructed negative s in plane z=-1 intersection")
                << (inter + (v * 2.0))
                << v
                << (inter + (w * 1.0))
                << w
                << int(QResult<QVector3D>::ResultExists)
                << inter;
    }

    QTest::newRow("lines in plane xy rot 45 about y obtuse intersect angle")
        << QVector3D(3.0f, 0.0f, -1.0f)
        << QVector3D(1.0f, -1.0f, 1.0f)
        << QVector3D(3.0f, 8.0f, -1.0f)
        << QVector3D(1.0f, 1.0f, 1.0f)
        << int(QResult<QVector3D>::ResultExists)
        << QVector3D(-1.0f, 4.0f, -5.0f);

    QTest::newRow("lines in plane xy rot 45 about y acute intersect angle")
        << QVector3D(3.0f, 5.0f, -1.0f)
        << QVector3D(4.0f, 1.0f, 4.0f)
        << QVector3D(3.0f, 8.0f, -1.0f)
        << QVector3D(1.0f, 1.0f, 1.0f)
        << int(QResult<QVector3D>::ResultExists)
        << QVector3D(-1.0f, 4.0f, -5.0f);

    {
        QVector3D inter(3.0, 5.0, 4.0);

        QVector3D v(-1.0, 4.0, 2.0);
        QVector3D w(2.0, 0.0, 4.0);

        QLine3D l1(inter, v);
        QLine3D l2(inter, w);

        QTest::newRow("constructed intersection")
                << l1.point(5.0)
                << v
                << l2.point(3.0)
                << w
                << int(QResult<QVector3D>::ResultExists)
                << inter;
    }
    {
        QVector3D inter(2.0, -5.0, 4.0);

        QVector3D v(0.0, 4.0, 0.0);
        QVector3D w(2.0, 0.0, 4.0);

        QLine3D l1(inter, v);
        QLine3D l2(inter, w);

        QTest::newRow("zero values in directions")
                << l1.point(5.0)
                << v
                << l2.point(3.0)
                << w
                << int(QResult<QVector3D>::ResultExists)
                << inter;
    }
    {
        QVector3D inter(2.0, 2.0, 2.0);

        QVector3D v(3.4, 4.0, -2.0);
        QVector3D w(2.2, 10.0, 1.3);

        QLine3D l1(inter, v);
        QLine3D l2(inter, w);

        QTest::newRow("that directions nearly axis-aligned")
                << l1.point(-10.0)
                << v
                << l2.point(3.0)
                << w
                << int(QResult<QVector3D>::ResultExists)
                << inter;
    }
    {
        QVector3D inter(0.0, 0.0, 0.0);

        QVector3D v(3.4, 4.0, -2.0);
        QVector3D w(2.2, 5.0, 1.3);

        QLine3D l1(inter, v);
        QLine3D l2(inter, w);

        QTest::newRow("int at origin")
                << l1.point(-10.0)
                << v
                << l2.point(3.0)
                << w
                << int(QResult<QVector3D>::ResultExists)
                << inter;
    }
    {
        QVector3D inter(-3.0, -5.0, -4.0);

        QVector3D v(5.0, 4.0, 5.0);
        QVector3D w(2.0, 0.0, 4.0);

        QLine3D l1(inter, v);
        QLine3D l2(inter, w);

        QTest::newRow("origins far out, negative intersection")
                << l1.point(25.0)
                << v
                << l2.point(35.0)
                << w
                << int(QResult<QVector3D>::ResultExists)
                << inter;
    }
    {
        QVector3D inter(3.0, 5.0, 4.0);

        QVector3D v(5.0, 4.0, 5.0);
        QVector3D w(2.0, 0.0, 4.0);

        QLine3D l1(inter, v);
        QLine3D l2(inter, w);

        QTest::newRow("negative co-efficients")
                << l1.point(-2.0)
                << v
                << l2.point(-5.0)
                << w
                << int(QResult<QVector3D>::ResultExists)
                << inter;
    }
    {
        QVector3D inter(0.0, 0.0, 0.0);

        QVector3D v(3.4, 4.0, -2.0);
        QVector3D w(2.2, 5.0, 1.3);

        QLine3D l1(inter, v);
        QLine3D l2(inter + QVector3D(1.0, 1.0, 1.0), w);

        QTest::newRow("no int at origin")
                << l1.point(-10.0)
                << v
                << l2.point(3.0)
                << w
                << int(QResult<QVector3D>::NoResult)
                << QVector3D(0.0, 0.0, 0.0);
    }
    {
        QVector3D inter(-3.0, -5.0, -4.0);

        QVector3D v(5.0, 4.0, 5.0);
        QVector3D w(2.0, 0.0, 4.0);

        QLine3D l1(inter + QVector3D(1.0, 1.0, 1.0), v);
        QLine3D l2(inter, w);

        QTest::newRow("no intersection, negative values")
                << l1.point(25.0)
                << v
                << l2.point(35.0)
                << w
                << int(QResult<QVector3D>::NoResult)
                << QVector3D(0.0, 0.0, 0.0);
    }
    {
        QVector3D inter(3.0, 5.0, 4.0);

        QVector3D v(5.0, 4.0, 5.0);
        QVector3D w(2.0, 0.0, 4.0);

        QLine3D l1(inter, v);
        QLine3D l2(inter + QVector3D(10.0, 10.0, 10.0), w);

        QTest::newRow("no intersection, bigger offset")
                << l1.point(-2.0)
                << v
                << l2.point(-5.0)
                << w
                << int(QResult<QVector3D>::NoResult)
                << QVector3D(0.0, 0.0, 0.0);
    }
}

void tst_QLine3D::intersection()
{
    QFETCH(QVector3D, point1);
    QFETCH(QVector3D, direction1);
    QFETCH(QVector3D, point2);
    QFETCH(QVector3D, direction2);
    QFETCH(int, validity);
    QFETCH(QVector3D, intersection);

    QLine3D line1(point1, direction1);
    QLine3D line2(point2, direction2);
    QResult<QVector3D> res = line1.intersection(line2);
    QCOMPARE(int(res.validity()), validity);
    QVector3D r = res.value();
    // qDebug("Result: (%0.10f, %0.10f, %0.10f)", r.x(), r.y(), r.z());
    // qDebug("Expect: (%0.10f, %0.10f, %0.10f)", intersection.x(), intersection.y(), intersection.z());

    QVERIFY(fuzzyCompare(res.value(), intersection));
}

void tst_QLine3D::contains_point_data()
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

void tst_QLine3D::contains_point()
{
    QFETCH(QVector3D, origin);
    QFETCH(QVector3D, direction);
    QFETCH(QVector3D, point);
    QFETCH(bool, contains);

    QLine3D line(origin, direction);
    // qDebug() << "Line - origin:" << origin << "  direction:" << direction;
    // qDebug() << "Point:" << point;
    QCOMPARE(line.contains(point), contains);
}

void tst_QLine3D::contains_line_data()
{
}

void tst_QLine3D::contains_line()
{
    // TODO
}

void tst_QLine3D::sameSide()
{
    // TODO
}

void tst_QLine3D::distance()
{
    // TODO
}

void tst_QLine3D::distanceFromOrigin()
{
    // TODO
}

QTEST_APPLESS_MAIN(tst_QLine3D)

#include "tst_qline3d.moc"
