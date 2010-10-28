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
#include "qglscenenode.h"
#include "qglpainter.h"
#include "qglabstracteffect.h"
#include "qglpicknode.h"

class tst_QGLSceneNode : public QObject
{
    Q_OBJECT
public:
    tst_QGLSceneNode() {}
    ~tst_QGLSceneNode() {}

private slots:
    void defaultValues();
    void modify();
};

// Check that all properties have their expected defaults.
void tst_QGLSceneNode::defaultValues()
{
    QGLSceneNode node;
    QVERIFY(node.options() == QGLSceneNode::CullBoundingBox);
    QVERIFY(node.geometry().isNull());
    QVERIFY(node.boundingBox().isNull());
    QVERIFY(node.localTransform().isIdentity());

    QVERIFY(node.rotation() == QVector3D(0, 0, 0));
    QCOMPARE(node.rotX(), qreal(0.0f));
    QCOMPARE(node.rotY(), qreal(0.0f));
    QCOMPARE(node.rotZ(), qreal(0.0f));

    QVERIFY(node.position() == QVector3D(0, 0, 0));
    QCOMPARE(node.x(), qreal(0.0f));
    QCOMPARE(node.y(), qreal(0.0f));
    QCOMPARE(node.z(), qreal(0.0f));

    QVERIFY(node.scale() == QVector3D(1, 1, 1));

    QVERIFY(node.drawingMode() == QGL::Triangles);
    QVERIFY(node.effect() == QGL::FlatColor);
    QVERIFY(node.userEffect() == 0);
    QVERIFY(!node.hasEffect());

    QCOMPARE(node.start(), 0);
    QCOMPARE(node.count(), 0);

    QCOMPARE(node.materialIndex(), -1);
    QCOMPARE(node.backMaterialIndex(), -1);

    QVERIFY(node.material() == 0);
    QVERIFY(node.backMaterial() == 0);
    QVERIFY(node.palette() == 0);
    QVERIFY(node.pickNode() == 0);

    QVERIFY(node.allChildren().isEmpty());
    QVERIFY(node.childNodeList().isEmpty());
}

class TestEffect : public QGLAbstractEffect
{
public:
    void setActive(QGLPainter *, bool) {}
    void update(QGLPainter *, QGLPainter::Updates) {}
};

// Modify properties and check that the modifications propagate.
void tst_QGLSceneNode::modify()
{
    QGLSceneNode node;

    QSignalSpy updatedSpy(&node, SIGNAL(updated()));

    node.setOptions(QGLSceneNode::ViewNormals);
    QCOMPARE(updatedSpy.count(), 1);
    QVERIFY(node.options() == QGLSceneNode::ViewNormals);

    QMatrix4x4 m;
    m.translate(-1.0f, 2.5f, 5.0f);
    m.rotate(45.0f, 1.0f, 1.0f, 1.0f);
    m.scale(23.5f);

    node.setLocalTransform(m);
    QCOMPARE(updatedSpy.count(), 2);
    QVERIFY(node.localTransform() == m);

    node.setRotation(QVector3D(-1, 2, -3));
    QCOMPARE(updatedSpy.count(), 3);
    QVERIFY(node.rotation() == QVector3D(-1, 2, -3));

    node.setRotX(45.0f);
    QCOMPARE(updatedSpy.count(), 4);
    QVERIFY(node.rotation() == QVector3D(45.0f, 2, -3));

    node.setRotY(-67.0f);
    QCOMPARE(updatedSpy.count(), 5);
    QVERIFY(node.rotation() == QVector3D(45.0f, -67.0f, -3));

    node.setRotZ(23.5f);
    QCOMPARE(updatedSpy.count(), 6);
    QVERIFY(node.rotation() == QVector3D(45.0f, -67.0f, 23.5f));

    node.setPosition(QVector3D(1, -2, 3));
    QCOMPARE(updatedSpy.count(), 7);
    QVERIFY(node.position() == QVector3D(1, -2, 3));

    node.setX(-45.0f);
    QCOMPARE(updatedSpy.count(), 8);
    QVERIFY(node.position() == QVector3D(-45.0f, -2, 3));

    node.setY(67.0f);
    QCOMPARE(updatedSpy.count(), 9);
    QVERIFY(node.position() == QVector3D(-45.0f, 67.0f, 3));

    node.setZ(-23.5f);
    QCOMPARE(updatedSpy.count(), 10);
    QVERIFY(node.position() == QVector3D(-45.0f, 67.0f, -23.5f));

    node.setScale(QVector3D(1.5f, -2.0f, 1.0f));
    QCOMPARE(updatedSpy.count(), 11);
    QVERIFY(node.scale() == QVector3D(1.5f, -2.0f, 1.0f));

    node.setDrawingMode(QGL::Points);
    QCOMPARE(updatedSpy.count(), 12);
    QVERIFY(node.drawingMode() == QGL::Points);

    node.setEffect(QGL::LitMaterial);
    QCOMPARE(updatedSpy.count(), 13);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(node.hasEffect());

    node.setEffectEnabled(false);
    QCOMPARE(updatedSpy.count(), 14);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(!node.hasEffect());

    TestEffect userEffect;
    node.setUserEffect(&userEffect);
    QCOMPARE(updatedSpy.count(), 15);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(node.userEffect() == &userEffect);
    QVERIFY(node.hasEffect());

    node.setMaterialIndex(3);
    QCOMPARE(updatedSpy.count(), 16);
    QCOMPARE(node.materialIndex(), 3);
    QVERIFY(node.material() == 0);  // material index is out of range

    node.setBackMaterialIndex(5);
    QCOMPARE(updatedSpy.count(), 17);
    QCOMPARE(node.backMaterialIndex(), 5);
    QVERIFY(node.backMaterial() == 0);  // material index is out of range

    QVERIFY(node.palette() == 0);

    QGLMaterial *mat1 = new QGLMaterial();
    node.setMaterial(mat1);
    QCOMPARE(updatedSpy.count(), 18);
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 5);
    QVERIFY(node.material() == mat1);
    QVERIFY(node.backMaterial() == 0);
    QVERIFY(node.palette() != 0);

    QGLMaterial *mat2 = new QGLMaterial();
    node.setBackMaterial(mat2);
    QCOMPARE(updatedSpy.count(), 19);
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.material() == mat1);
    QVERIFY(node.backMaterial() == mat2);

    QGLMaterial *mat3 = new QGLMaterial();
    node.setMaterial(mat3);
    QCOMPARE(updatedSpy.count(), 20);
    QCOMPARE(node.materialIndex(), 2);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.material() == mat3);
    QVERIFY(node.backMaterial() == mat2);

    node.setMaterial(mat1);
    QCOMPARE(updatedSpy.count(), 21);
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.material() == mat1);
    QVERIFY(node.backMaterial() == mat2);

    QVERIFY(node.palette()->material(0) == mat1);
    QVERIFY(node.palette()->material(1) == mat2);
    QVERIFY(node.palette()->material(2) == mat3);
    QCOMPARE(node.palette()->size(), 3);

    QGLPickNode pick;
    QVERIFY(pick.target() == 0);
    QCOMPARE(pick.id(), -1);
    node.setPickNode(&pick);
    QVERIFY(node.pickNode() == &pick);
    QVERIFY(pick.target() == &node);
    QCOMPARE(pick.id(), -1);
    pick.setId(4);
    QCOMPARE(pick.id(), 4);

    // One last check to make sure there are no leaks between properties.
    // e.g. the position actually changing the rotation, etc.
    QVERIFY(node.options() == QGLSceneNode::ViewNormals);
    QVERIFY(node.localTransform() == m);
    QVERIFY(node.rotation() == QVector3D(45.0f, -67.0f, 23.5f));
    QVERIFY(node.position() == QVector3D(-45.0f, 67.0f, -23.5f));
    QVERIFY(node.scale() == QVector3D(1.5f, -2.0f, 1.0f));
    QVERIFY(node.drawingMode() == QGL::Points);
    QVERIFY(node.effect() == QGL::LitMaterial);
    QVERIFY(node.userEffect() == &userEffect);
    QVERIFY(node.hasEffect());
    QCOMPARE(node.materialIndex(), 0);
    QCOMPARE(node.backMaterialIndex(), 1);
    QVERIFY(node.pickNode() == &pick);
}

QTEST_APPLESS_MAIN(tst_QGLSceneNode)

#include "tst_qglscenenode.moc"
