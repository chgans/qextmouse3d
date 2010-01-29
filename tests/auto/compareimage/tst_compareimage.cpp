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
#include "../../shared/compareimage.h"
#include <QPainter>

class tst_CompareImage : public QObject
{
    Q_OBJECT
public:
    tst_CompareImage() {}
    ~tst_CompareImage() {}

private slots:
    void fuzzyCompare_data();
    void fuzzyCompare();
    void pass_data();
    void pass();
    void fail_data();
    void fail();
    //void differentformats(); // TODO
};


void tst_CompareImage::fuzzyCompare_data()
{
    QTest::addColumn<QImage>("image1");
    QTest::addColumn<QImage>("image2");
    QTest::addColumn<bool>("expectedOutcome");

    QImage emptyImage1(100,100, QImage::Format_ARGB32_Premultiplied );
    emptyImage1.fill(0);

    QImage emptyImage2(100,100, QImage::Format_ARGB32_Premultiplied );
    emptyImage2.fill(0);

    QImage blueImage(100,100, QImage::Format_ARGB32_Premultiplied );
    QPainter bluePainter( &blueImage );
    bluePainter.fillRect(QRect(0,0,100,100),QColor(0,0,255,255));

    QImage ninetyFourPercentBlue(100,100, QImage::Format_ARGB32_Premultiplied );
    ninetyFourPercentBlue.fill(0);

    QPainter ninetyFourPainter( &ninetyFourPercentBlue );
    ninetyFourPainter.fillRect(QRect(0,0, 94, 100), QColor(0,0,255,255));

    QImage ninetyFivePercentBlue(100,100, QImage::Format_ARGB32_Premultiplied );
    ninetyFivePercentBlue.fill(0);

    QPainter ninetyFivePainter( &ninetyFivePercentBlue );
    ninetyFivePainter.fillRect(QRect(0,0, 95, 100), QColor(0,0,255,255));

    QTest::newRow("The Same Image")
        << emptyImage1
        << emptyImage1
        << true;

    QTest::newRow("Empty Images")
        << emptyImage1
        << emptyImage2
        << true;


    QTest::newRow("95 Percent Blue")
        << blueImage
        << ninetyFivePercentBlue
        << true;
    

    QTest::newRow("94 Percent Blue")
        << blueImage
        << ninetyFourPercentBlue
        << false;
}

void tst_CompareImage::fuzzyCompare()
{
    QFETCH(QImage, image1);
    QFETCH(QImage, image2);
    QFETCH(bool, expectedOutcome);
    QCOMPARE(qFuzzyCompare(image1,image2), expectedOutcome);
}

void tst_CompareImage::pass_data()
{
    QTest::addColumn<QImage>("image1");
    QTest::addColumn<QImage>("image2");
    QTest::addColumn<int>("threshold");

    QImage emptyImage1(128,128, QImage::Format_ARGB32_Premultiplied );
    emptyImage1.fill(0);

    QImage emptyImage2(128,128, QImage::Format_ARGB32_Premultiplied );
    emptyImage2.fill(0);

    QImage filledImage1(128,128, QImage::Format_ARGB32_Premultiplied );
    filledImage1.fill(0xffffffff);
    QImage filledImage2(128,128, QImage::Format_ARGB32_Premultiplied );
    filledImage2.fill(0xffffffff);

    QImage filledImage_ff(128,128, QImage::Format_ARGB32_Premultiplied );
    filledImage_ff.fill(0xffffffff);
    QImage filledImage_fe(128,128, QImage::Format_ARGB32_Premultiplied );
    filledImage_fe.fill(0xfefefeff);


    QTest::newRow("Empty Images")
        << emptyImage1
        << emptyImage2
        << 0;

    QTest::newRow("Filled images")
        << filledImage1
        << filledImage2
        << 0;

    QTest::newRow("Low Threshold")
        << filledImage_ff
        << filledImage_fe
        << 1;

    QTest::newRow("Complete Threshold")
        << emptyImage1
        << filledImage1
        << 256;

}

void tst_CompareImage::pass()
{
    QFETCH(QImage, image1);
    QFETCH(QImage, image2);
    QFETCH(int, threshold);
    QVERIFY(qProportionDifferentRGBPixels(image1,image2,threshold,threshold, threshold) == 0.0f);
}

void tst_CompareImage::fail_data()
{
    QTest::addColumn<QImage>("image1");
    QTest::addColumn<QImage>("image2");
    QTest::addColumn<int>("threshold");

    QImage fill_ff(128,128, QImage::Format_ARGB32_Premultiplied );
    fill_ff.fill(0xffffffff);

    QImage fill_fe(128,128, QImage::Format_ARGB32_Premultiplied );
    fill_ff.fill(0xfefefeff);
    QTest::newRow("Low Threshold")
        << fill_ff
        << fill_fe
        << 0;
}

void tst_CompareImage::fail()
{
    QFETCH(QImage, image1);
    QFETCH(QImage, image2);
    QFETCH(int, threshold);
    QCOMPARE(qProportionDifferentRGBPixels(image1,image2,threshold,threshold, threshold), 1.0f);
}


QTEST_APPLESS_MAIN(tst_CompareImage)

#include "tst_compareimage.moc"
