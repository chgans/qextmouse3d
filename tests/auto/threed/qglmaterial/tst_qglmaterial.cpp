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
#include "qglmaterial.h"

class tst_QGLMaterial : public QObject
{
    Q_OBJECT
public:
    tst_QGLMaterial() {}
    ~tst_QGLMaterial() {}

private slots:
    void create();
    void modify();
};

void tst_QGLMaterial::create()
{
    // Test that a newly created object has the correct defaults.
    QGLMaterial params;
    QCOMPARE(params.ambientColor().redF(), 0.2);
    QCOMPARE(params.ambientColor().greenF(), 0.2);
    QCOMPARE(params.ambientColor().blueF(), 0.2);
    QCOMPARE(params.ambientColor().alphaF(), 1.0);
    QCOMPARE(params.diffuseColor().redF(), 0.8);
    QCOMPARE(params.diffuseColor().greenF(), 0.8);
    QCOMPARE(params.diffuseColor().blueF(), 0.8);
    QCOMPARE(params.diffuseColor().alphaF(), 1.0);
    QCOMPARE(params.specularColor().red(), 0);
    QCOMPARE(params.specularColor().green(), 0);
    QCOMPARE(params.specularColor().blue(), 0);
    QCOMPARE(params.specularColor().alpha(), 255);
    QCOMPARE(params.emittedLight().red(), 0);
    QCOMPARE(params.emittedLight().green(), 0);
    QCOMPARE(params.emittedLight().blue(), 0);
    QCOMPARE(params.emittedLight().alpha(), 255);
    QCOMPARE(params.shininess(), qreal(0));
}

void tst_QGLMaterial::modify()
{
    // Test modifying each field individually.
    QGLMaterial params;
    params.setAmbientColor(Qt::green);
    QCOMPARE(params.ambientColor().red(), 0);
    QCOMPARE(params.ambientColor().green(), 255);
    QCOMPARE(params.ambientColor().blue(), 0);
    QCOMPARE(params.ambientColor().alpha(), 255);
    params.setDiffuseColor(Qt::blue);
    QCOMPARE(params.diffuseColor().red(), 0);
    QCOMPARE(params.diffuseColor().green(), 0);
    QCOMPARE(params.diffuseColor().blue(), 255);
    QCOMPARE(params.diffuseColor().alpha(), 255);
    params.setSpecularColor(Qt::cyan);
    QCOMPARE(params.specularColor().red(), 0);
    QCOMPARE(params.specularColor().green(), 255);
    QCOMPARE(params.specularColor().blue(), 255);
    QCOMPARE(params.specularColor().alpha(), 255);
    params.setEmittedLight(Qt::white);
    QCOMPARE(params.emittedLight().red(), 255);
    QCOMPARE(params.emittedLight().green(), 255);
    QCOMPARE(params.emittedLight().blue(), 255);
    QCOMPARE(params.emittedLight().alpha(), 255);
    params.setShininess(128);
    QCOMPARE(params.shininess(), qreal(128));

    // Test that we don't get any side effects between properties.
    QCOMPARE(params.ambientColor().red(), 0);
    QCOMPARE(params.ambientColor().green(), 255);
    QCOMPARE(params.ambientColor().blue(), 0);
    QCOMPARE(params.ambientColor().alpha(), 255);
    QCOMPARE(params.diffuseColor().red(), 0);
    QCOMPARE(params.diffuseColor().green(), 0);
    QCOMPARE(params.diffuseColor().blue(), 255);
    QCOMPARE(params.diffuseColor().alpha(), 255);
    QCOMPARE(params.specularColor().red(), 0);
    QCOMPARE(params.specularColor().green(), 255);
    QCOMPARE(params.specularColor().blue(), 255);
    QCOMPARE(params.specularColor().alpha(), 255);
    QCOMPARE(params.emittedLight().red(), 255);
    QCOMPARE(params.emittedLight().green(), 255);
    QCOMPARE(params.emittedLight().blue(), 255);
    QCOMPARE(params.emittedLight().alpha(), 255);
    QCOMPARE(params.shininess(), qreal(128));
}

QTEST_APPLESS_MAIN(tst_QGLMaterial)

#include "tst_qglmaterial.moc"
