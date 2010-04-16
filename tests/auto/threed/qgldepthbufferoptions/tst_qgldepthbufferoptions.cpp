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
#include "qgldepthbufferoptions.h"

class tst_QGLDepthBufferOptions : public QObject
{
    Q_OBJECT
public:
    tst_QGLDepthBufferOptions() {}
    ~tst_QGLDepthBufferOptions() {}

private slots:
    void defaultOptions();
};

void tst_QGLDepthBufferOptions::defaultOptions()
{
    {
        QGLDepthBufferOptions opt;
        // test defaults
        QCOMPARE(opt.function(), QGLDepthBufferOptions::Less);
        QCOMPARE(opt.isWriteEnabled(), true);
        QCOMPARE(opt.nearRange(), 0.0);
        QCOMPARE(opt.farRange(), 1.0);

        QVERIFY(opt == QGLDepthBufferOptions());
        QVERIFY(!(opt != QGLDepthBufferOptions()));
        QVERIFY(opt.isNull());

        QGLDepthBufferOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
    }

    // bit of white box testing here.  We know that changing the item
    // creates a new 'd' that is then used for the future results.
    // check that this new 'd' otherwise shares the same defaults.


    {
        QGLDepthBufferOptions opt;
        // test defaults
        QCOMPARE(opt.function(), QGLDepthBufferOptions::Less);
        QCOMPARE(opt.isWriteEnabled(), true);
        QCOMPARE(opt.nearRange(), 0.0);
        QCOMPARE(opt.farRange(), 1.0);

        QVERIFY(opt == QGLDepthBufferOptions());
        QVERIFY(!(opt != QGLDepthBufferOptions()));
        QVERIFY(opt.isNull());

        QGLDepthBufferOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
    }
    {
        QGLDepthBufferOptions opt;
        opt.setFunction(QGLDepthBufferOptions::Never);
        QCOMPARE(opt.function(), QGLDepthBufferOptions::Never);
        // test defaults
        QCOMPARE(opt.isWriteEnabled(), true);
        QCOMPARE(opt.nearRange(), 0.0);
        QCOMPARE(opt.farRange(), 1.0);

        QVERIFY(opt != QGLDepthBufferOptions());
        QVERIFY(!(opt == QGLDepthBufferOptions()));
        QVERIFY(!opt.isNull());

        QGLDepthBufferOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.function(), QGLDepthBufferOptions::Never);
    }
    {
        QGLDepthBufferOptions opt;
        opt.setWriteEnabled(false);
        QCOMPARE(opt.isWriteEnabled(), false);
        // test defaults
        QCOMPARE(opt.function(), QGLDepthBufferOptions::Less);
        QCOMPARE(opt.nearRange(), 0.0);
        QCOMPARE(opt.farRange(), 1.0);

        QVERIFY(opt != QGLDepthBufferOptions());
        QVERIFY(!(opt == QGLDepthBufferOptions()));
        QVERIFY(!opt.isNull());

        QGLDepthBufferOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.isWriteEnabled(), false);
    }
    {
        QGLDepthBufferOptions opt;
        opt.setRange(0.3, opt.farRange());
        QCOMPARE(opt.nearRange(), 0.3);
        // test defaults
        QCOMPARE(opt.function(), QGLDepthBufferOptions::Less);
        QCOMPARE(opt.isWriteEnabled(), true);
        QCOMPARE(opt.farRange(), 1.0);

        QVERIFY(opt != QGLDepthBufferOptions());
        QVERIFY(!(opt == QGLDepthBufferOptions()));
        QVERIFY(!opt.isNull());

        QGLDepthBufferOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.nearRange(), 0.3);
    }
    {
        QGLDepthBufferOptions opt;
        opt.setRange(opt.nearRange(), 0.8);
        QCOMPARE(opt.farRange(), 0.8);
        // test defaults
        QCOMPARE(opt.function(), QGLDepthBufferOptions::Less);
        QCOMPARE(opt.isWriteEnabled(), true);
        QCOMPARE(opt.nearRange(), 0.0);

        QVERIFY(opt != QGLDepthBufferOptions());
        QVERIFY(!(opt == QGLDepthBufferOptions()));
        QVERIFY(!opt.isNull());

        QGLDepthBufferOptions other = opt;
        QVERIFY(opt == other);
        QVERIFY(other == opt);
        QCOMPARE(other.farRange(), 0.8);
    }
}

QTEST_APPLESS_MAIN(tst_QGLDepthBufferOptions)

#include "tst_qgldepthbufferoptions.moc"
