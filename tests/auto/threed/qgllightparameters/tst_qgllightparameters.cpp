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
#include "qgllightparameters.h"

class tst_QGLLightParameters : public QObject
{
    Q_OBJECT
public:
    tst_QGLLightParameters() {}
    ~tst_QGLLightParameters() {}

private slots:
    void create();
    void modify();
    void type();
};

void tst_QGLLightParameters::create()
{
    // Test that a newly created object has the correct defaults.
    QGLLightParameters params;
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QVERIFY(params.position() == QVector3D(0, 0, 0));
    QVERIFY(params.direction() == QVector3D(0, 0, 1));
    QVERIFY(params.ambientColor() == QColor(0, 0, 0, 255));
    QVERIFY(params.diffuseColor() == QColor(255, 255, 255, 255));
    QVERIFY(params.specularColor() == QColor(255, 255, 255, 255));
    QVERIFY(params.spotDirection() == QVector3D(0, 0, -1));
    QCOMPARE(params.spotExponent(), (qreal)0.0f);
    QCOMPARE(params.spotAngle(), (qreal)180.0f);
    QCOMPARE(params.spotCosAngle(), (qreal)-1.0f);
    QCOMPARE(params.constantAttenuation(), (qreal)1.0f);
    QCOMPARE(params.linearAttenuation(), (qreal)0.0f);
    QCOMPARE(params.quadraticAttenuation(), (qreal)0.0f);
}

void tst_QGLLightParameters::modify()
{
    // Test modifying each field individually.
    QGLLightParameters params;
    params.setPosition(QVector3D(1, 2, -3));
    QVERIFY(params.position() == QVector3D(1.0f, 2.0f, -3.0f));
    QVERIFY(params.direction() == QVector3D(0, 0, 0));
    QVERIFY(params.type() == QGLLightParameters::Positional);
    params.setDirection(QVector3D(-1, -2, 3));
    QVERIFY(params.position() == QVector3D(0, 0, 0));
    QVERIFY(params.direction() == QVector3D(-1.0f, -2.0f, 3.0f));
    QVERIFY(params.type() == QGLLightParameters::Directional);
    params.setAmbientColor(Qt::red);
    QCOMPARE(params.ambientColor().red(), 255);
    QCOMPARE(params.ambientColor().green(), 0);
    QCOMPARE(params.ambientColor().blue(), 0);
    QCOMPARE(params.ambientColor().alpha(), 255);
    params.setDiffuseColor(Qt::green);
    QCOMPARE(params.diffuseColor().red(), 0);
    QCOMPARE(params.diffuseColor().green(), 255);
    QCOMPARE(params.diffuseColor().blue(), 0);
    QCOMPARE(params.diffuseColor().alpha(), 255);
    params.setSpecularColor(Qt::black);
    QCOMPARE(params.specularColor().red(), 0);
    QCOMPARE(params.specularColor().green(), 0);
    QCOMPARE(params.specularColor().blue(), 0);
    QCOMPARE(params.specularColor().alpha(), 255);
    params.setSpotDirection(QVector3D(0, 1, 0));
    QVERIFY(params.spotDirection() == QVector3D(0.0f, 1.0f, 0.0f));
    params.setSpotExponent(23.5f);
    QCOMPARE(params.spotExponent(), (qreal)23.5f);
    params.setSpotAngle(90.0f);
    QCOMPARE(params.spotAngle(), (qreal)90.0f);
    QVERIFY(params.spotCosAngle() <= 0.000001f);    // Fuzzy compare is not fuzzy enough!
    params.setSpotAngle(180.0f);
    QCOMPARE(params.spotCosAngle(), (qreal)-1.0f);
    params.setConstantAttenuation(16.0f);
    QCOMPARE(params.constantAttenuation(), (qreal)16.0f);
    params.setLinearAttenuation(-3.5f);
    QCOMPARE(params.linearAttenuation(), (qreal)-3.5f);
    params.setQuadraticAttenuation(4.0f);
    QCOMPARE(params.quadraticAttenuation(), (qreal)4.0f);

    // Test that we don't get any side effects between properties.
    QVERIFY(params.position() == QVector3D(0, 0, 0));
    QVERIFY(params.direction() == QVector3D(-1.0f, -2.0f, 3.0f));
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QCOMPARE(params.ambientColor().red(), 255);
    QCOMPARE(params.ambientColor().green(), 0);
    QCOMPARE(params.ambientColor().blue(), 0);
    QCOMPARE(params.ambientColor().alpha(), 255);
    QCOMPARE(params.diffuseColor().red(), 0);
    QCOMPARE(params.diffuseColor().green(), 255);
    QCOMPARE(params.diffuseColor().blue(), 0);
    QCOMPARE(params.diffuseColor().alpha(), 255);
    QCOMPARE(params.specularColor().red(), 0);
    QCOMPARE(params.specularColor().green(), 0);
    QCOMPARE(params.specularColor().blue(), 0);
    QCOMPARE(params.specularColor().alpha(), 255);
    QVERIFY(params.spotDirection() == QVector3D(0.0f, 1.0f, 0.0f));
    QCOMPARE(params.spotExponent(), (qreal)23.5f);
    QCOMPARE(params.spotAngle(), (qreal)180.0f);
    QCOMPARE(params.spotCosAngle(), (qreal)-1.0f);
    QCOMPARE(params.constantAttenuation(), (qreal)16.0f);
    QCOMPARE(params.linearAttenuation(), (qreal)-3.5f);
    QCOMPARE(params.quadraticAttenuation(), (qreal)4.0f);
}

void tst_QGLLightParameters::type()
{
    QGLLightParameters params;
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QCOMPARE(params.position(), QVector3D(0.0f, 0.0f, 0.0f));
    QCOMPARE(params.direction(), QVector3D(0.0f, 0.0f, 1.0f));
    params.setDirection(QVector3D(1.0f, 0.0f, 1.0f));
    QVERIFY(params.type() == QGLLightParameters::Directional);
    QCOMPARE(params.position(), QVector3D(0.0f, 0.0f, 0.0f));
    QCOMPARE(params.direction(), QVector3D(1.0f, 0.0f, 1.0f));
    params.setPosition(QVector3D(2.0f, 0.0f, 2.0f));
    QVERIFY(params.type() == QGLLightParameters::Positional);
    QCOMPARE(params.position(), QVector3D(2.0f, 0.0f, 2.0f));
    QCOMPARE(params.direction(), QVector3D(0.0f, 0.0f, 0.0f));
}

QTEST_APPLESS_MAIN(tst_QGLLightParameters)

#include "tst_qgllightparameters.moc"
