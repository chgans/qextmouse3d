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
    QTest::addColumn<int>("expected_vertices");
    QTest::addColumn<int>("expected_indices");

    QTest::newRow("cube-obj-faceted")
            << "basic-cube.obj" << ""
               << 24 << 36;
    QTest::newRow("cube-obj-smooth")
            << "basic-cube.obj" << "ForceSmooth"
               << 8 << 36;
}

void tst_LoadModel::create()
{
    QFETCH(QString, model);
    QFETCH(QString, options);
    QFETCH(int, expected_vertices);
    QFETCH(int, expected_indices);

    QGLAbstractScene *scene = 0;
    QString model_path(QLatin1String(":/data/models/%1"));
    model_path = model_path.arg(model);

    scene = QGLAbstractScene::loadScene(model_path, QString(), options);
    QVERIFY(scene != 0);

    QGLSceneNode *node = scene->mainNode();
    //qDumpScene(node);

    QGeometryData data = node->children().at(0)->geometry();
    //qDebug() << data;

    QCOMPARE(data.vertices().count(), expected_vertices);
    QCOMPARE(data.indices().count(), expected_indices);
}

QTEST_APPLESS_MAIN(tst_LoadModel)

#include "tst_load_model.moc"
