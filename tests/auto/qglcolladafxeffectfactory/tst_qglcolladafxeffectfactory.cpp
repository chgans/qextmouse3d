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
#include "qglcolladafxeffectfactory.h"
#include <QXmlStreamReader>
#include <QColor>

class QGLColladaFxEffect;

Q_DECLARE_METATYPE(QVector<float>)

static void cleanupEffectList(QList<QGLColladaFxEffect*> &effects)
{
    while(effects.count())
    {
        delete effects.back();
        effects.pop_back();
    }
}



// Inherit from QGLColladaFxEffectFactory to get access to it's innards
class tst_QGLColladaFxEffectFactory : public QObject, QGLColladaFxEffectFactory
{
    Q_OBJECT
public:
    tst_QGLColladaFxEffectFactory() {}
    ~tst_QGLColladaFxEffectFactory() {}

private slots:
    void create_data();
    void create();
    void processFloatList_data();
    void processFloatList();
    void processLibraryImagesElement();
    void loadEffectsFromFile();
};



void tst_QGLColladaFxEffectFactory::create_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<int>("effectCount");

    QTest::newRow("null") << "" << 0;
    QTest::newRow("nonexistent file") << "nonexistentfile.dae" << 0;
    QTest::newRow("collada cube example") << ":/collada_cube.xml" << 1;
}



void tst_QGLColladaFxEffectFactory::create()
{
    QFETCH(QString, fileName);
    QFETCH(int, effectCount);

    QList<QGLColladaFxEffect*> effects = QGLColladaFxEffectFactory::loadEffectsFromFile(fileName);
    QCOMPARE(effects.count(), effectCount);
    cleanupEffectList(effects);
}



void tst_QGLColladaFxEffectFactory::processFloatList_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QVector<float> >("expectedResult");

    QTest::newRow("null") << "" << QVector<float>();

    QVector<float> expectedSingleResult;
    expectedSingleResult << 0.456f;
    QTest::newRow("single_float") << "<floats>0.456</floats>" << expectedSingleResult;

    QVector<float> expectedSimpleResult;
    expectedSimpleResult << 0.0f << 1.1f << 2.2f << 3.3f;
    QTest::newRow("simple_floats") << "<floats>0.0 1.1 2.2 3.3</floats>" << expectedSimpleResult;

    QVector<float> expectedColorResult;
    expectedColorResult << 0.0f << 1.1f << 2.2f << 3.3f;
    QTest::newRow("color") << "<color>0.0 1.1 2.2 3.3</color>" << expectedColorResult;

    QVector<float> expectedSevenDigitResult;
    expectedSevenDigitResult << 1234567.0f << 1.234567f << 76.54321f << 1111.111f << 22222.22f << 7645213.0f;
    QTest::newRow("seven_digits") << "<floats>1234567.0 1.234567 76.54321 1111.111 22222.22 7645213.0</floats>" << expectedSevenDigitResult;
}



void tst_QGLColladaFxEffectFactory::processFloatList()
{
    QFETCH(QString, xmlString);
    QFETCH(QVector<float>, expectedResult);

    QXmlStreamReader xml(xmlString);

    xml.readNext(); // startDocument
    xml.readNext();

    QVector<float> result = QGLColladaFxEffectFactory::processFloatList( xml );
    QCOMPARE(result, expectedResult);
}



void tst_QGLColladaFxEffectFactory::processLibraryImagesElement()
{
    // Example from Collada Fx 1.4 (escapes and ':' character added):
    QXmlStreamReader xml("<library_images>\
                         <image name=\"Rose\">\
                                     <init_from>\
                                         :../flowers/rose01.jpg\
                                         </init_from>\
                                         </image>\
                                         </library_images>");

    xml.readNext(); // startDocument
    xml.readNext();

    StateStack state;
    QGLColladaFxEffectFactory::processLibraryImagesElement( xml, state );

    // The structure is just an implementation detail, but the param and image
    // should be well formed:
    QGLColladaParam* param = state.at(0).at(0);
    QCOMPARE(param->type(), int(QGLColladaParam::ImageType));
    QGLColladaImageParam* imageParam = static_cast<QGLColladaImageParam*>(param);
    QCOMPARE(imageParam->name(), QString("Rose"));
    QCOMPARE(imageParam->image().size(), QSize(50,75));
}

void tst_QGLColladaFxEffectFactory::loadEffectsFromFile()
{
    QList<QGLColladaFxEffect*> cubeResult =
            QGLColladaFxEffectFactory::loadEffectsFromFile(":/collada_cube.xml");

    QCOMPARE(cubeResult.count(), 1);

    QGLColladaFxEffect* cubeEffect = cubeResult.at(0);

    QEXPECT_FAIL("", "Missing functionality", Continue);
    QCOMPARE(cubeEffect->id(), QString("whitePhong"));

    QCOMPARE(cubeEffect->sid(), QString("phong1"));
    QVERIFY2(cubeEffect->material()->emittedLight() ==
             QColor::fromRgbF(0.0f, 0.0f, 0.3f, 1.0f), "Emission color doesn't match");
    QVERIFY2(cubeEffect->material()->ambientColor() ==
             QColor::fromRgbF(1.0f, 0.0f, 0.0f, 1.0f), "Ambient color doesn't match");
    QVERIFY2(cubeEffect->material()->diffuseColor() == QColor::fromRgbF(0.0f, 1.0f, 0.0f, 1.0f),
             "Diffuse color doesn't match");
    QVERIFY2(cubeEffect->material()->specularColor() ==
             QColor::fromRgbF(1.0f, 1.0f, 1.0f, 1.0f),
             "Specular color doesn't match");
    QVERIFY2(cubeEffect->material()->shininess() == 20, "Shininess doesn't match");
}


QTEST_APPLESS_MAIN(tst_QGLColladaFxEffectFactory)

#include "tst_qglcolladafxeffectfactory.moc"
