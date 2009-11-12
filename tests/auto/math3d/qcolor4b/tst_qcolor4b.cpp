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
#include "qcolor4b.h"

class tst_QColor4b : public QObject
{
    Q_OBJECT
public:
    tst_QColor4b() {}
    ~tst_QColor4b() {}

private slots:
    void create();
    void modify();
    void copy();
    void compare_data();
    void compare();
    void toColor();
    void checkSize();
};

void tst_QColor4b::create()
{
    QColor4b c1;
    QCOMPARE(c1.red(), 0);
    QCOMPARE(c1.green(), 0);
    QCOMPARE(c1.blue(), 0);
    QCOMPARE(c1.alpha(), 255);
    QCOMPARE(c1.redF(), qreal(0.0));
    QCOMPARE(c1.greenF(), qreal(0.0));
    QCOMPARE(c1.blueF(), qreal(0.0));
    QCOMPARE(c1.alphaF(), qreal(1.0));

    QColor4b c2(162, 54, 39);
    QCOMPARE(c2.red(), 162);
    QCOMPARE(c2.green(), 54);
    QCOMPARE(c2.blue(), 39);
    QCOMPARE(c2.alpha(), 255);
    QCOMPARE(c2.redF(), qreal(162 / 255.0));
    QCOMPARE(c2.greenF(), qreal(54 / 255.0));
    QCOMPARE(c2.blueF(), qreal(39 / 255.0));
    QCOMPARE(c2.alphaF(), qreal(1.0));

    QColor4b c3(162, 54, 39, 41);
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 41);
    QCOMPARE(c3.redF(), qreal(162 / 255.0));
    QCOMPARE(c3.greenF(), qreal(54 / 255.0));
    QCOMPARE(c3.blueF(), qreal(39 / 255.0));
    QCOMPARE(c3.alphaF(), qreal(41 / 255.0));

    QColor4b c4(QColor(162, 54, 39, 41));
    QCOMPARE(c4.red(), 162);
    QCOMPARE(c4.green(), 54);
    QCOMPARE(c4.blue(), 39);
    QCOMPARE(c4.alpha(), 41);

    QColor4b c5(Qt::red);
    QCOMPARE(c5.red(), 255);
    QCOMPARE(c5.green(), 0);
    QCOMPARE(c5.blue(), 0);
    QCOMPARE(c5.alpha(), 255);

    QColor4b c6(qRgb(162, 54, 39));
    QCOMPARE(c6.red(), 162);
    QCOMPARE(c6.green(), 54);
    QCOMPARE(c6.blue(), 39);
    QCOMPARE(c6.alpha(), 255);

    QColor4b c7(qRgba(162, 54, 39, 41));
    QCOMPARE(c7.red(), 162);
    QCOMPARE(c7.green(), 54);
    QCOMPARE(c7.blue(), 39);
    QCOMPARE(c7.alpha(), 41);

    QColor4b c8(256, -3, 1);    // truncates from int to uchar
    QCOMPARE(c8.red(), 0);
    QCOMPARE(c8.green(), 253);
    QCOMPARE(c8.blue(), 1);
    QCOMPARE(c8.alpha(), 255);

    QColor4b c9 = QColor4b::fromRgb(162, 54, 39);
    QCOMPARE(c9.red(), 162);
    QCOMPARE(c9.green(), 54);
    QCOMPARE(c9.blue(), 39);
    QCOMPARE(c9.alpha(), 255);
    c9 = QColor4b::fromRgb(162, 54, 39, 41);
    QCOMPARE(c9.red(), 162);
    QCOMPARE(c9.green(), 54);
    QCOMPARE(c9.blue(), 39);
    QCOMPARE(c9.alpha(), 41);

    QColor4b c10 = QColor4b::fromRgbF(1.0f, 0.5f, 0.25f);
    QCOMPARE(c10.red(), 255);
    QCOMPARE(c10.green(), 128);
    QCOMPARE(c10.blue(), 64);
    QCOMPARE(c10.alpha(), 255);
    c10 = QColor4b::fromRgbF(1.0f, 0.5f, 0.25f, 0.75f);
    QCOMPARE(c10.red(), 255);
    QCOMPARE(c10.green(), 128);
    QCOMPARE(c10.blue(), 64);
    QCOMPARE(c10.alpha(), 191);

    static uchar const data[4] = {uchar(162), uchar(54), uchar(39), uchar(255)};
    QColor4b c11 = QColor4b::fromRaw(data);
    QCOMPARE(c11.red(), 162);
    QCOMPARE(c11.green(), 54);
    QCOMPARE(c11.blue(), 39);
    QCOMPARE(c11.alpha(), 255);
}

void tst_QColor4b::modify()
{
    QColor4b c1(162, 54, 39, 41);
    c1.setRed(34);
    c1.setGreen(163);
    c1.setBlue(2);
    c1.setAlpha(200);
    QCOMPARE(c1.red(), 34);
    QCOMPARE(c1.green(), 163);
    QCOMPARE(c1.blue(), 2);
    QCOMPARE(c1.alpha(), 200);
    QCOMPARE(c1.redF(), qreal(34 / 255.0));
    QCOMPARE(c1.greenF(), qreal(163 / 255.0));
    QCOMPARE(c1.blueF(), qreal(2 / 255.0));
    QCOMPARE(c1.alphaF(), qreal(200 / 255.0));

    QColor4b c2;
    c2.setRedF(1.0f);
    c2.setGreenF(0.5f);
    c2.setBlueF(0.25f);
    c2.setAlphaF(0.75f);
    QCOMPARE(c2.red(), 255);
    QCOMPARE(c2.green(), 128);
    QCOMPARE(c2.blue(), 64);
    QCOMPARE(c2.alpha(), 191);
    QCOMPARE(c2.redF(), qreal(1.0));
    QCOMPARE(c2.greenF(), qreal(128 / 255.0));
    QCOMPARE(c2.blueF(), qreal(64 / 255.0));
    QCOMPARE(c2.alphaF(), qreal(191 / 255.0));

    QColor4b c3;
    c3.setRgb(162, 54, 39);
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 255);
    c3.setRgb(162, 54, 39, 41);
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 41);

    QColor4b c4;
    c4.setRgbF(1.0f, 0.5f, 0.25f);
    QCOMPARE(c4.red(), 255);
    QCOMPARE(c4.green(), 128);
    QCOMPARE(c4.blue(), 64);
    QCOMPARE(c4.alpha(), 255);
    c4.setRgbF(1.0f, 0.5f, 0.25f, 0.75f);
    QCOMPARE(c4.red(), 255);
    QCOMPARE(c4.green(), 128);
    QCOMPARE(c4.blue(), 64);
    QCOMPARE(c4.alpha(), 191);
}

void tst_QColor4b::copy()
{
    QColor4b c1(162, 54, 39, 41);
    QColor4b c2(c1);
    QCOMPARE(c2.red(), 162);
    QCOMPARE(c2.green(), 54);
    QCOMPARE(c2.blue(), 39);
    QCOMPARE(c2.alpha(), 41);

    QColor4b c3;
    c3 = c2;
    QCOMPARE(c3.red(), 162);
    QCOMPARE(c3.green(), 54);
    QCOMPARE(c3.blue(), 39);
    QCOMPARE(c3.alpha(), 41);
}

void tst_QColor4b::compare_data()
{
    QTest::addColumn<int>("red1");
    QTest::addColumn<int>("green1");
    QTest::addColumn<int>("blue1");
    QTest::addColumn<int>("alpha1");
    QTest::addColumn<int>("red2");
    QTest::addColumn<int>("green2");
    QTest::addColumn<int>("blue2");
    QTest::addColumn<int>("alpha2");
    QTest::addColumn<bool>("isEqual");

    QTest::newRow("black")
        << 0 << 0 << 0 << 0
        << 0 << 0 << 0 << 0
        << true;

    QTest::newRow("white")
        << 255 << 255 << 255 << 255
        << 255 << 255 << 255 << 255
        << true;

    QTest::newRow("mixed")
        << 162 << 54 << 39 << 41
        << 162 << 54 << 39 << 41
        << true;

    QTest::newRow("red-diff")
        << 162 << 54 << 39 << 41
        << 161 << 54 << 39 << 41
        << false;

    QTest::newRow("green-diff")
        << 162 << 54 << 39 << 41
        << 162 << 53 << 39 << 41
        << false;

    QTest::newRow("blue-diff")
        << 162 << 54 << 39 << 41
        << 162 << 54 << 38 << 41
        << false;

    QTest::newRow("alpha-diff")
        << 162 << 54 << 39 << 41
        << 162 << 54 << 39 << 40
        << false;

    QTest::newRow("all-diff")
        << 162 << 54 << 39 << 41
        << 161 << 53 << 38 << 40
        << false;
}

void tst_QColor4b::compare()
{
    QFETCH(int, red1);
    QFETCH(int, green1);
    QFETCH(int, blue1);
    QFETCH(int, alpha1);
    QFETCH(int, red2);
    QFETCH(int, green2);
    QFETCH(int, blue2);
    QFETCH(int, alpha2);
    QFETCH(bool, isEqual);

    QColor4b c1(red1, green1, blue1, alpha1);
    QColor4b c2(red2, green2, blue2, alpha2);

    if (isEqual) {
        QVERIFY(c1 == c2);
        QVERIFY(!(c1 != c2));
    } else {
        QVERIFY(!(c1 == c2));
        QVERIFY(c1 != c2);
    }
}

void tst_QColor4b::toColor()
{
    QColor4b c1(162, 54, 39, 41);
    QColor c2 = c1.toColor();
    QCOMPARE(c2.red(), 162);
    QCOMPARE(c2.green(), 54);
    QCOMPARE(c2.blue(), 39);
    QCOMPARE(c2.alpha(), 41);
}

void tst_QColor4b::checkSize()
{
    // We need QColor4b to be precisely packed as four single-byte values.
    QCOMPARE(sizeof(QColor4b), sizeof(uchar) * 4);
}

QTEST_APPLESS_MAIN(tst_QColor4b)

#include "tst_qcolor4b.moc"
