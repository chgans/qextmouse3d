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
#include "qglblendoptions.h"

class tst_QGLBlendOptions : public QObject
{
    Q_OBJECT
public:
    tst_QGLBlendOptions() {}
    ~tst_QGLBlendOptions() {}

private slots:
    void defaultOptions();
};

void tst_QGLBlendOptions::defaultOptions()
{
    {
        QGLBlendOptions opt;
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt == QGLBlendOptions());
        QVERIFY(!(opt != QGLBlendOptions()));
        QVERIFY(opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
    }

    // bit of white box testing here.  We know that changing the item
    // creates a new 'd' that is then used for the future results.
    // check that this new 'd' otherwise shares the same defaults.


    {
        QGLBlendOptions opt;
        opt.setEnabled(true);
        QCOMPARE(opt.isEnabled(), true);
        // test defaults
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.isEnabled(), true);
    }
    {
        QGLBlendOptions opt;
        opt.setBlendColor(QColor(255,0,0,128));
        QCOMPARE(opt.blendColor(), QColor(255,0,0,128));
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.blendColor(), QColor(255,0,0,128));
    }
    {
        QGLBlendOptions opt;
        opt.setSourceColorFactor(QGLBlendOptions::SrcAlpha);
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::SrcAlpha);
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.sourceColorFactor(), QGLBlendOptions::SrcAlpha);
    }
    {
        QGLBlendOptions opt;
        opt.setSourceAlphaFactor(QGLBlendOptions::ConstantAlpha);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::ConstantAlpha);
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.sourceAlphaFactor(), QGLBlendOptions::ConstantAlpha);
    }
    {
        QGLBlendOptions opt;
        opt.setDestinationColorFactor(QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::One);
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.destinationColorFactor(), QGLBlendOptions::One);
    }
    {
        QGLBlendOptions opt;
        opt.setDestinationAlphaFactor(QGLBlendOptions::OneMinusDstAlpha);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::OneMinusDstAlpha);
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.destinationAlphaFactor(), QGLBlendOptions::OneMinusDstAlpha);
    }
    {
        QGLBlendOptions opt;
        opt.setColorEquation(QGLBlendOptions::Maximum);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Maximum);
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.colorEquation(), QGLBlendOptions::Maximum);
    }
    {
        QGLBlendOptions opt;
        opt.setAlphaEquation(QGLBlendOptions::Minimum);
        QCOMPARE(opt.alphaEquation(), QGLBlendOptions::Minimum);
        // test defaults
        QCOMPARE(opt.isEnabled(), false);
        QCOMPARE(opt.blendColor(), QColor(0,0,0,0));
        QCOMPARE(opt.sourceColorFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.sourceAlphaFactor(), QGLBlendOptions::One);
        QCOMPARE(opt.destinationColorFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.destinationAlphaFactor(), QGLBlendOptions::Zero);
        QCOMPARE(opt.colorEquation(), QGLBlendOptions::Add);

        QVERIFY(opt != QGLBlendOptions());
        QVERIFY(!(opt == QGLBlendOptions()));
        QVERIFY(!opt.isNull());

        QGLBlendOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.alphaEquation(), QGLBlendOptions::Minimum);
    }
}

QTEST_APPLESS_MAIN(tst_QGLBlendOptions)

#include "tst_qglblendoptions.moc"
