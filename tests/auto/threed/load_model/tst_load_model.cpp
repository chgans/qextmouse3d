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

#include "qglabstractscene.h"

class tst_LoadModel : public QObject
{
    Q_OBJECT
public:
    tst_LoadModel() {}
    ~tst_LoadModel() {}

private slots:
    void create_data();
    void create();
};

void tst_LoadModel::create_data()
{
    QTest::addColumn<QString>("model");
    QTest::addColumn<QString>("options");
    QTest::addColumn<QString>("importName");
    QTest::addColumn<QString>("nodeName");
    QTest::addColumn<QString>("materialName");
    QTest::addColumn<QRgb>("materialDiffuse");
    QTest::addColumn<QString>("texture");
    QTest::addColumn<int>("expected_vertices");
    QTest::addColumn<int>("expected_indices");

    ////// --- OBJ ---

    // default is faceted for obj
    QTest::newRow("cube-obj-default")
            << "basic-cube.obj" << ""
            << "__main" << "CubeObject_CubeMesh" << "Red"
            << qRgb(188, 32, 32) << "tex"
            << 24 << 36;

    // force a faceted load
    QTest::newRow("cube-obj-faceted")
            << "basic-cube.obj" << "ForceFaceted"
            << "__main" << "CubeObject_CubeMesh" << "Red"
            << qRgb(188, 32, 32) << "tex"
            << 24 << 36;

    // force a smooth load
    QTest::newRow("cube-obj-smooth")
            << "basic-cube.obj" << "ForceSmooth"
            << "__main" << "CubeObject_CubeMesh" << "Red"
            << qRgb(188, 32, 32) << "tex"
            << 8 << 36;


    ////// --- 3DS ---

    // default is faceted
    QTest::newRow("cube-3ds-default")
            << "basic-cube.3ds" << ""
            << "LIB3DS" << "CubeObject" << "Red"
            << qRgb(234, 40, 40) << "tex"
            << 24 << 36;

    // force smooth
    QTest::newRow("cube-3ds-smooth")
            << "basic-cube.3ds" << "ForceSmooth"
            << "LIB3DS" << "CubeObject" << "Red"
            << qRgb(234, 40, 40) << "tex"
            << 8 << 36;


    ////// --- wave model ---

    // default is faceted
    QTest::newRow("wave-obj-default")
            << "wave.obj" << ""
               << "__main" << "Wave_Obj" << ""
                  << qRgb(0, 0, 0) << ""
                     << 1296 << 1296;

    // force smooth
    QTest::newRow("wave-obj-default")
            << "wave.obj" << "ForceSmooth"
               << "__main" << "Wave_Obj" << ""
                  << qRgb(0, 0, 0) << ""
                     << 259 << 1296;
}

void tst_LoadModel::create()
{
    QFETCH(QString, model);
    QFETCH(QString, options);
    QFETCH(QString, importName);
    QFETCH(QString, nodeName);
    QFETCH(QString, materialName);
    QFETCH(QRgb, materialDiffuse);
    QFETCH(QString, texture);
    QFETCH(int, expected_vertices);
    QFETCH(int, expected_indices);

    QGLAbstractScene *scene = 0;
    QString model_path(QLatin1String(":/data/models/%1"));
    model_path = model_path.arg(model);

    scene = QGLAbstractScene::loadScene(model_path, QString(), options);
    QVERIFY(scene != 0);

    QGLSceneNode *node = scene->mainNode();
    QCOMPARE(node->objectName(), importName);

    QGLSceneNode *foundGeometry = 0;
    QGLSceneNode *foundNode = nodeName.isEmpty() ? (QGLSceneNode *)1 : 0;
    QGLSceneNode *foundMaterial = materialName.isEmpty() ? (QGLSceneNode *)1 : 0;
    QList<QGLSceneNode*> list = node->allChildren();
    for (int i = 0; i < list.size(); ++ i)
    {
        if (!foundGeometry && list.at(i)->geometry().count() > 0)
            foundGeometry = list.at(i);
        if (!foundNode && list.at(i)->objectName() == nodeName)
            foundNode = list.at(i);
        if (!foundMaterial && list.at(i)->material() &&
                (list.at(i)->material()->objectName() == materialName))
            foundMaterial = list.at(i);
    }

    QVERIFY(foundGeometry != 0);
    QVERIFY(foundNode != 0);
    QVERIFY(foundMaterial != 0);

    int r, g, b;
    if (!materialName.isEmpty())
    {
        foundMaterial->material()->diffuseColor().getRgb(&r, &g, &b);
        QCOMPARE(qRgb(r, g, b), materialDiffuse);
    }

    QGeometryData data = foundGeometry->geometry();
    QVERIFY(data.count() > 0);

    QCOMPARE(data.vertices().count(), expected_vertices);
    QCOMPARE(data.normals().count(), expected_vertices);
    QCOMPARE(data.indices().count(), expected_indices);
}

QTEST_APPLESS_MAIN(tst_LoadModel)

#include "tst_load_model.moc"
