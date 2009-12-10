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
#include <QtCore/qpointer.h>
#include "qgldisplaylist.h"
#include "qglmaterialcollection.h"
#include "qglscenenode.h"
#include "qglabstracteffect.h"

class tst_QGLDisplayList : public QObject
{
    Q_OBJECT
public:
    tst_QGLDisplayList() {}
    ~tst_QGLDisplayList() {}

private slots:
    void createDefault();
    void newSection();
    void currentNode();
    void newNode();
    void newNodeEmptyPrev();
    void pushNode();
    void popNode();
    void addTriangle();
    void addQuad();
    void addTriangleFan();
    void addTriangulatedFace();
    void extrude();
    void finalize();
};

void tst_QGLDisplayList::createDefault()
{
    // Test that a newly created list works with no defaults
    QGLDisplayList displayList0;
    QCOMPARE(displayList0.currentSection(), (QGLSection*)0);
    QCOMPARE(displayList0.sections().size(), 0);
    QCOMPARE(displayList0.currentNode(), (QGLSceneNode*)0);
    QVERIFY(displayList0.geometry() != (QGLGeometry*)0);
    QVERIFY(displayList0.geometry()->palette() != 0);

    QPointer<QGLMaterialCollection> palette = new QGLMaterialCollection();
    QGLDisplayList displayList1(0, palette);
    QCOMPARE(displayList1.geometry()->palette(), palette.data());

    QObject *obj = new QObject();
    QPointer<QGLDisplayList> displayList2 = new QGLDisplayList(obj, palette);
    QCOMPARE(displayList2->geometry()->palette(), palette.data());
    QCOMPARE(displayList2->parent(), obj);
    QCOMPARE(obj->children().at(0), displayList2.data());
    delete obj;
    QVERIFY(displayList2.isNull());  // got destroyed by being a child
    QVERIFY(!palette.isNull());  // palette did not get destroyed
}

void tst_QGLDisplayList::newSection()
{
    QGLDisplayList displayList;
    QGLSection *s = displayList.newSection(); // defaults to smooth
    QCOMPARE(s, displayList.currentSection());
    QCOMPARE(displayList.sections().count(), 1);
    QVERIFY(displayList.sections().contains(s));
    QCOMPARE(s->smoothing(), QGL::Smooth);
    QGLSection *s2 = displayList.newSection(QGL::Faceted);
    QVERIFY(s->isFinalized());
    QCOMPARE(s2, displayList.currentSection());
    QCOMPARE(displayList.sections().count(), 2);
    QVERIFY(displayList.sections().contains(s2));
}

void tst_QGLDisplayList::currentNode()
{
}

class TestEffect : public QGLAbstractEffect
{
    QList<QGL::VertexAttribute> requiredFields() const
    {
        return QList<QGL::VertexAttribute>();
    }
    void setActive(bool)
    {
    }
};

void tst_QGLDisplayList::newNode()
{
    QGLDisplayList displayList;
    QGLSection *sec = displayList.newSection();  // calls new node
    QGLSceneNode *node = displayList.currentNode();

    // newly created node works and has all defaults
    QCOMPARE(node->parent(), &displayList);
    QCOMPARE(node->effect(), QGL::LitMaterial); // lit material is the default
    QCOMPARE(node->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node->material(), -1);
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 0);

    // add some settings and geometry to the new node
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    node->setUserEffect(eff);
    node->setMaterial(5);
    sec->append(QLogicalVertex(QVector3D()));
    sec->append(QLogicalVertex(QVector3D(1.0f, 2.0f, 3.0f)));
    sec->append(QLogicalVertex(QVector3D(4.0f, 5.0f, 6.0f)));

    // now create a new node
    QGLSceneNode *node2 = displayList.newNode();

    // the previous node got cleaned up properly, with its count updated
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 3);

    // new node counts off from where the previous one finished and has same defaults
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 0);
    QCOMPARE(node2->parent(), &displayList);
    QCOMPARE(node2->effect(), QGL::LitDecalTexture2D); // cloned from previous
    QCOMPARE(node2->userEffect(), eff);
    QCOMPARE(node2->material(), 5);

    sec->append(QLogicalVertex(QVector3D()));
    sec->append(QLogicalVertex(QVector3D(1.0f, 2.0f, 3.0f)));
    sec->append(QLogicalVertex(QVector3D(4.0f, 5.0f, 6.0f)));
    displayList.newNode();

    // confirm that 2nd and last node in chain was also finished properly
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 3);

#ifndef QT_NO_MEMBER_TEMPLATES
    QList<QGLSceneNode*> nodes = displayList.findChildren<QGLSceneNode*>();
    QVERIFY(nodes.contains(node));
    QVERIFY(nodes.contains(node2));
#endif
}

void tst_QGLDisplayList::newNodeEmptyPrev()
{
    QGLDisplayList displayList;
    QPointer<QGLSceneNode> node = displayList.newNode();

    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    node->setUserEffect(eff);
    node->setMaterial(5);

    // causes any empty nodes to be deleted whilst creating new
    QGLSceneNode *node2 = displayList.newNode();

    // has been deleted because its empty
    QVERIFY(node.isNull());

#ifndef QT_NO_MEMBER_TEMPLATES
    QList<QGLSceneNode*> nodes = displayList.findChildren<QGLSceneNode*>();
    QVERIFY(!nodes.contains(node.data()));
    QVERIFY(nodes.contains(node2));
#endif

    // even tho node was deleted, node2 still inherited its values
    QCOMPARE(node2->effect(), QGL::LitDecalTexture2D); // cloned from previous
    QCOMPARE(node2->userEffect(), eff);
    QCOMPARE(node2->material(), 5);
}

void tst_QGLDisplayList::pushNode()
{
    QGLDisplayList displayList;
    QGLSection *sec = displayList.newSection();
    QGLSceneNode *node = displayList.newNode();
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    node->setUserEffect(eff);
    node->setMaterial(5);
    sec->append(QLogicalVertex(QVector3D()));
    sec->append(QLogicalVertex(QVector3D(1.0f, 2.0f, 3.0f)));
    sec->append(QLogicalVertex(QVector3D(4.0f, 5.0f, 6.0f)));

    QGLSceneNode *node2 = displayList.pushNode();
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 3);
    QCOMPARE(displayList.currentNode(), node2);
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 0);
    QCOMPARE(node2->parent(), node);
    QCOMPARE(node2->effect(), QGL::LitMaterial); // lit material is the default
    QCOMPARE(node2->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node2->material(), -1);
}

void tst_QGLDisplayList::popNode()
{
    QGLDisplayList displayList;
    QGLSection *sec = displayList.newSection();
    QGLSceneNode *node = displayList.newNode();
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    QMatrix4x4 mat;
    mat.rotate(45.0f, 0.0f, 1.0f, 0.0f);
    node->setLocalTransform(mat);
    node->setUserEffect(eff);
    node->setMaterial(5);
    sec->append(QLogicalVertex(QVector3D()));
    sec->append(QLogicalVertex(QVector3D(1.0f, 2.0f, 3.0f)));
    sec->append(QLogicalVertex(QVector3D(4.0f, 5.0f, 6.0f)));

    QGLSceneNode *node2 = displayList.pushNode();

    sec->append(QLogicalVertex(QVector3D()));
    sec->append(QLogicalVertex(QVector3D(1.0f, 2.0f, 3.0f)));
    sec->append(QLogicalVertex(QVector3D(4.0f, 5.0f, 6.0f)));

    QGLSceneNode *node3 = displayList.popNode();
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 3);

    QCOMPARE(node3->effect(), QGL::LitDecalTexture2D);
    QCOMPARE(node3->userEffect(), eff);
    QCOMPARE(node3->material(), 5);
    QCOMPARE(node3->localTransform(), mat);
    QCOMPARE(node3->start(), 6);
    QCOMPARE(node3->count(), 0);
}

void tst_QGLDisplayList::addTriangle()
{
}

void tst_QGLDisplayList::addQuad()
{
}

void tst_QGLDisplayList::addTriangleFan()
{
}

void tst_QGLDisplayList::addTriangulatedFace()
{
}

void tst_QGLDisplayList::extrude()
{
}

void tst_QGLDisplayList::finalize()
{
}

QTEST_APPLESS_MAIN(tst_QGLDisplayList)

#include "tst_qgldisplaylist.moc"
