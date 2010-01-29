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
#include "qglfogparameters.h"

class tst_QGLFogParameters : public QObject
{
    Q_OBJECT
public:
    tst_QGLFogParameters() {}
    ~tst_QGLFogParameters() {}

private slots:
    void create();
    void modify();
};

void tst_QGLFogParameters::create()
{
    // Test that a newly created object has the correct defaults.
    QGLFogParameters params;
    QVERIFY(params.mode() == QGLFogParameters::Linear);
    QCOMPARE(params.density(), (qreal)1.0f);
    QCOMPARE(params.nearDistance(), (qreal)0.0f);
    QCOMPARE(params.farDistance(), (qreal)1.0f);
    QCOMPARE(params.color().red(), 0);
    QCOMPARE(params.color().green(), 0);
    QCOMPARE(params.color().blue(), 0);
    QCOMPARE(params.color().alpha(), 0);
}

void tst_QGLFogParameters::modify()
{
    // Test modifying each field individually.
    QGLFogParameters params;
    params.setMode(QGLFogParameters::Exponential);
    QVERIFY(params.mode() == QGLFogParameters::Exponential);
    params.setDensity(2.5f);
    QCOMPARE(params.density(), (qreal)2.5f);
    params.setNearDistance(-3.0f);
    QCOMPARE(params.nearDistance(), (qreal)-3.0f);
    params.setFarDistance(4.0f);
    QCOMPARE(params.farDistance(), (qreal)4.0f);
    params.setColor(Qt::red);
    QCOMPARE(params.color().red(), 255);
    QCOMPARE(params.color().green(), 0);
    QCOMPARE(params.color().blue(), 0);
    QCOMPARE(params.color().alpha(), 255);

    // Test that we don't get any side effects between properties.
    QVERIFY(params.mode() == QGLFogParameters::Exponential);
    QCOMPARE(params.density(), (qreal)2.5f);
    QCOMPARE(params.nearDistance(), (qreal)-3.0f);
    QCOMPARE(params.farDistance(), (qreal)4.0f);
}

QTEST_APPLESS_MAIN(tst_QGLFogParameters)

#include "tst_qglfogparameters.moc"
