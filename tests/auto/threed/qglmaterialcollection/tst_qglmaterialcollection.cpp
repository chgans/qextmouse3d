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
#include "qglmaterialcollection.h"

class tst_QGLMaterialCollection : public QObject
{
    Q_OBJECT
public:
    tst_QGLMaterialCollection() : destroyCount(0) {}
    ~tst_QGLMaterialCollection() {}

private slots:
    void simple();
    void addMaterial();
    void ownership();

public slots:
    void materialDestroyed() { ++destroyCount; }

private:
    int destroyCount;
};

// Simple sanity check on the collection.
void tst_QGLMaterialCollection::simple()
{
    QGLMaterialCollection *coll = new QGLMaterialCollection();
    QVERIFY(coll->isEmpty());
    QCOMPARE(coll->size(), 0);
    QVERIFY(!coll->material(0));
    QCOMPARE(coll->indexOf(QLatin1String("foo")), -1);

    destroyCount = 0;

    QGLMaterial *mat = new QGLMaterial(this);
    mat->setObjectName(QLatin1String("foo"));
    connect(mat, SIGNAL(destroyed()), this, SLOT(materialDestroyed()));
    QCOMPARE(coll->addMaterial(mat), 0);
    QVERIFY(mat->parent() == coll); // Ownership transfers to collection.

    QVERIFY(!coll->isEmpty());
    QCOMPARE(coll->size(), 1);
    QVERIFY(coll->material(0) == mat);
    QCOMPARE(coll->indexOf(QLatin1String("foo")), 0);
    QCOMPARE(coll->indexOf(QLatin1String("bar")), -1);

    QCOMPARE(destroyCount, 0);
    delete coll;
    QCOMPARE(destroyCount, 1);  // Check that the material was destroyed.
}

// Test adding lots of materials and then looking them up.
// Also acts as a torture test for the underlying data structures.
void tst_QGLMaterialCollection::addMaterial()
{
    QGLMaterialCollection *coll = new QGLMaterialCollection();

    const int Size = 2048;

    QGLMaterial **materials = new QGLMaterial *[Size];
    for (int index = 0; index < Size; ++index) {
        QGLMaterial *mat = new QGLMaterial();
        mat->setObjectName(QString::number(index));
        connect(mat, SIGNAL(destroyed()), this, SLOT(materialDestroyed()));
        materials[index] = mat;
        QCOMPARE(coll->addMaterial(mat), index);
    }

    for (int index = 0; index < Size; ++index) {
        QVERIFY(coll->material(index) == materials[index]);
        QCOMPARE(coll->indexOf(QString::number(index)), index);
        QCOMPARE(coll->materialName(index), QString::number(index));
    }

    destroyCount = 0;
    delete coll;
    QCOMPARE(destroyCount, Size);
}

// Test collection ownership.
void tst_QGLMaterialCollection::ownership()
{
    QGLMaterialCollection coll;

    QGLMaterial *mat = new QGLMaterial(this);
    mat->setObjectName(QLatin1String("foo"));

    QGLMaterial *mat2 = new QGLMaterial(this);
    mat2->setObjectName(QLatin1String("bar"));

    // Adding the same material multiple times returns the same index.
    QCOMPARE(coll.addMaterial(mat), 0);
    QCOMPARE(coll.addMaterial(mat2), 1);
    QCOMPARE(coll.addMaterial(mat), 0);
    QVERIFY(coll.material(0) == mat);
    QVERIFY(coll.material(1) == mat2);
    QCOMPARE(coll.indexOf(QLatin1String("foo")), 0);
    QCOMPARE(coll.indexOf(QLatin1String("bar")), 1);
    QCOMPARE(coll.size(), 2);

    // Shouldn't be able to add to another collection yet.
    QGLMaterialCollection coll2;
    QCOMPARE(coll2.addMaterial(mat), -1);
    QVERIFY(coll2.isEmpty());

    // Remove a material from the first collection and add it to the second.
    coll.removeMaterial(mat);
    QCOMPARE(coll.size(), 2);   // Still two elements, one is null.
    QVERIFY(coll.material(0) == 0);
    QVERIFY(coll.material(1) == mat2);
    QCOMPARE(coll.indexOf(QLatin1String("foo")), -1);
    QCOMPARE(coll.indexOf(QLatin1String("bar")), 1);
    QCOMPARE(coll2.addMaterial(mat), 0);
    QCOMPARE(coll2.size(), 1);
    QVERIFY(coll2.material(0) == mat);
    QCOMPARE(coll2.indexOf(QLatin1String("foo")), 0);
}

QTEST_APPLESS_MAIN(tst_QGLMaterialCollection)

#include "tst_qglmaterialcollection.moc"
