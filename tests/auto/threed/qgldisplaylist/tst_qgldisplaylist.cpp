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
#include "qglsection_p.h"
#include "qglmaterialcollection.h"
#include "qglscenenode.h"
#include "qglabstracteffect.h"
#include "qtest_helpers_p.h"

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

class TestQGLDisplayList : public QGLDisplayList
{
public:
    QGLSection *currentSection() { return QGLDisplayList::currentSection(); }
    QList<QGLSection*> sections() { return QGLDisplayList::sections(); }
};

void tst_QGLDisplayList::createDefault()
{
    // Test that a newly created list works with no defaults
    TestQGLDisplayList displayList0;
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
    TestQGLDisplayList displayList;
    displayList.newSection(); // defaults to smooth
    QGLSection *s = displayList.currentSection();
    QCOMPARE(s, displayList.currentSection());
    QCOMPARE(displayList.sections().count(), 1);
    QVERIFY(displayList.sections().contains(s));
    QCOMPARE(s->smoothing(), QGL::Smooth);
    displayList.newSection(QGL::Faceted);
    QGLSection *s2 = displayList.currentSection();
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
    TestQGLDisplayList displayList;
    displayList.newSection();  // calls new node
    QGLSection *sec = displayList.currentSection();
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
    QCOMPARE(node2->effect(), QGL::LitMaterial); // lit material is the default
    QCOMPARE(node2->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node2->material(), -1);

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
    TestQGLDisplayList displayList;
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

    // even tho node was deleted, node2 still works and is default
    QCOMPARE(node2->effect(), QGL::LitMaterial); // cloned from previous
    QCOMPARE(node2->userEffect(), (QGLAbstractEffect*)0);
    QCOMPARE(node2->material(), -1);
}

void tst_QGLDisplayList::pushNode()
{
    TestQGLDisplayList displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
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
    TestQGLDisplayList displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
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
    TestQGLDisplayList displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D norm(0.0f, 0.0f, 1.0f);

    displayList.addTriangle(a, b, c);
    QCOMPARE(sec->vertices().count(), 3);
    QCOMPARE(sec->vertices().at(0), a);
    QCOMPARE(sec->vertices().at(1), b);
    QCOMPARE(sec->vertices().at(2), c);
    QCOMPARE(sec->normals().count(), 3);
    QCOMPARE(sec->indices().count(), 3);
    QCOMPARE(sec->indices().at(0), 0);
    QCOMPARE(sec->indices().at(1), 1);
    QCOMPARE(sec->indices().at(2), 2);
    QCOMPARE(sec->dataTypes(), QLogicalVertex::Vertex | QLogicalVertex::Normal);

    QVector3D d(-1.0f, 1.0f, 0.0f);
    displayList.addTriangle(a, c, d);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertices().at(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normals().at(0).normalized(), norm);
    QCOMPARE(sec->normals().at(1).normalized(), norm);
    QCOMPARE(sec->normals().at(2).normalized(), norm);
    QCOMPARE(sec->normals().at(3).normalized(), norm);
    QCOMPARE(sec->indices().count(), 6);
    QCOMPARE(sec->indices().at(3), 0);
    QCOMPARE(sec->indices().at(4), 2);
    QCOMPARE(sec->indices().at(5), 3);
    QCOMPARE(sec->dataTypes(), QLogicalVertex::Vertex | QLogicalVertex::Normal);

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    displayList.newSection();
    sec = displayList.currentSection();
    node = displayList.currentNode();
    QGLTextureModel model(0.0f, 0.0f, 1.0f, 1.0f);
    displayList.addTriangle(a, b, c, QVector3D(), model);
    QCOMPARE(sec->vertices().count(), 3);
    QCOMPARE(sec->vertices().at(0), a);
    QCOMPARE(sec->vertices().at(1), b);
    QCOMPARE(sec->vertices().at(2), c);
    QCOMPARE(sec->normals().count(), 3);
    QCOMPARE(sec->normals().at(0).normalized(), norm);
    QCOMPARE(sec->normals().at(1).normalized(), norm);
    QCOMPARE(sec->normals().at(2).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 3);
    QCOMPARE(sec->texCoords().at(0), QVector2D(0.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(1), QVector2D(1.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(2), QVector2D(1.0f, 1.0f));
    QCOMPARE(sec->indices().count(), 3);
    QCOMPARE(sec->indices().at(0), 0);
    QCOMPARE(sec->indices().at(1), 1);
    QCOMPARE(sec->indices().at(2), 2);
    QCOMPARE(sec->dataTypes(),
             QLogicalVertex::Vertex | QLogicalVertex::Normal | QLogicalVertex::Texture);

    displayList.addTriangle(a, c, d, QVector3D(), model, true);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertices().at(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normals().at(0).normalized(), norm);
    QCOMPARE(sec->normals().at(1).normalized(), norm);
    QCOMPARE(sec->normals().at(2).normalized(), norm);
    QCOMPARE(sec->normals().at(3).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 4);
    QCOMPARE(sec->texCoords().at(0), QVector2D(0.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(1), QVector2D(1.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(2), QVector2D(1.0f, 1.0f));
    QCOMPARE(sec->texCoords().at(3), QVector2D(0.0f, 1.0f));
    QCOMPARE(sec->indices().count(), 6);
    QCOMPARE(sec->indices().at(3), 0);
    QCOMPARE(sec->indices().at(4), 2);
    QCOMPARE(sec->indices().at(5), 3);
    QCOMPARE(sec->dataTypes(),
             QLogicalVertex::Vertex | QLogicalVertex::Normal | QLogicalVertex::Texture);

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);
}

void tst_QGLDisplayList::addQuad()
{
    TestQGLDisplayList displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D norm(0.0f, 0.0f, 1.0f);

    displayList.addQuad(a, b, c, d);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertices().at(0), a);
    QCOMPARE(sec->vertices().at(1), b);
    QCOMPARE(sec->vertices().at(2), c);
    QCOMPARE(sec->vertices().at(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normals().at(0).normalized(), norm);
    QCOMPARE(sec->normals().at(1).normalized(), norm);
    QCOMPARE(sec->normals().at(2).normalized(), norm);
    QCOMPARE(sec->normals().at(3).normalized(), norm);
    QCOMPARE(sec->indices().count(), 6);
    QCOMPARE(sec->indices().at(3), 0);
    QCOMPARE(sec->indices().at(0), 0);
    QCOMPARE(sec->indices().at(1), 1);
    QCOMPARE(sec->indices().at(2), 2);
    QCOMPARE(sec->indices().at(3), 0);
    QCOMPARE(sec->indices().at(4), 2);
    QCOMPARE(sec->indices().at(5), 3);
    QCOMPARE(sec->dataTypes(), QLogicalVertex::Vertex | QLogicalVertex::Normal);

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    displayList.newSection();
    sec = displayList.currentSection();
    node = displayList.currentNode();
    QGLTextureModel model(0.0f, 0.0f, 1.0f, 1.0f);
    displayList.addQuad(a, b, c, d, model);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertices().at(0), a);
    QCOMPARE(sec->vertices().at(1), b);
    QCOMPARE(sec->vertices().at(2), c);
    QCOMPARE(sec->vertices().at(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normals().at(0).normalized(), norm);
    QCOMPARE(sec->normals().at(1).normalized(), norm);
    QCOMPARE(sec->normals().at(2).normalized(), norm);
    QCOMPARE(sec->normals().at(3).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 4);
    QCOMPARE(sec->texCoords().at(0), QVector2D(0.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(1), QVector2D(1.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(2), QVector2D(1.0f, 1.0f));
    QCOMPARE(sec->texCoords().at(3), QVector2D(0.0f, 1.0f));
    QCOMPARE(sec->indices().count(), 6);
    QCOMPARE(sec->indices().at(3), 0);
    QCOMPARE(sec->indices().at(0), 0);
    QCOMPARE(sec->indices().at(1), 1);
    QCOMPARE(sec->indices().at(2), 2);
    QCOMPARE(sec->indices().at(3), 0);
    QCOMPARE(sec->indices().at(4), 2);
    QCOMPARE(sec->indices().at(5), 3);
    QCOMPARE(sec->dataTypes(),
             QLogicalVertex::Vertex | QLogicalVertex::Normal | QLogicalVertex::Texture);

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);
}

void tst_QGLDisplayList::addTriangleFan()
{
    TestQGLDisplayList displayList;
    displayList.newSection(QGL::Faceted);
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    qreal one_on_root2 = 1.0f / sqrt(2.0f);
    QVector3D n1(0.0f, -one_on_root2, one_on_root2);
    QVector3D n2(one_on_root2, 0.0f, one_on_root2);
    QVector3D n3(0.0f, one_on_root2, one_on_root2);
    QVector3D center(0.0f, 0.0f, 1.0f);
    QGL::VectorArray edges;
    edges << a << b << c << d;

    displayList.addTriangleFan(center, edges);
    sec->finalize();
    QCOMPARE(sec->vertices().count(), 9);
    QCOMPARE(sec->vertices().at(0), a);
    QCOMPARE(sec->vertices().at(1), b);
    QCOMPARE(sec->vertices().at(2), center);
    QCOMPARE(sec->vertices().at(3), b);
    QCOMPARE(sec->vertices().at(4), c);
    QCOMPARE(sec->vertices().at(5), center);
    QCOMPARE(sec->vertices().at(6), c);
    QCOMPARE(sec->vertices().at(7), d);
    QCOMPARE(sec->vertices().at(8), center);
    QCOMPARE(sec->normals().count(), 9);
    QCOMPARE(sec->normals().at(0), n1);
    QCOMPARE(sec->normals().at(1), n1);
    QCOMPARE(sec->normals().at(2), n1);
    QCOMPARE(sec->normals().at(3), n2);
    QCOMPARE(sec->normals().at(4), n2);
    QCOMPARE(sec->normals().at(5), n2);
    QCOMPARE(sec->normals().at(6), n3);
    QCOMPARE(sec->normals().at(7), n3);
    QCOMPARE(sec->normals().at(8), n3);
    QCOMPARE(sec->indices().count(), 9);
    QCOMPARE(sec->indices().at(0), 0);
    QCOMPARE(sec->indices().at(1), 1);
    QCOMPARE(sec->indices().at(2), 2);
    QCOMPARE(sec->indices().at(3), 3);
    QCOMPARE(sec->indices().at(4), 4);
    QCOMPARE(sec->indices().at(5), 5);
    QCOMPARE(sec->indices().at(6), 6);
    QCOMPARE(sec->indices().at(7), 7);
    QCOMPARE(sec->indices().at(8), 8);
    QCOMPARE(sec->dataTypes(), QLogicalVertex::Vertex | QLogicalVertex::Normal);

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 9);
}

void tst_QGLDisplayList::addTriangulatedFace()
{
    TestQGLDisplayList displayList;
    displayList.newSection(QGL::Faceted);
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QGL::VectorArray edges;
    edges << a << b << c << d;

    displayList.addTriangulatedFace(center, edges);
    sec->finalize();
    QCOMPARE(sec->vertices().count(), 5);
    QCOMPARE(sec->vertices().at(0), a);
    QCOMPARE(sec->vertices().at(1), b);
    QCOMPARE(sec->vertices().at(2), center);
    QCOMPARE(sec->vertices().at(3), c);
    QCOMPARE(sec->vertices().at(4), d);
    QCOMPARE(sec->normals().count(), 5);
    QCOMPARE(sec->normals().at(0), n);
    QCOMPARE(sec->normals().at(1), n);
    QCOMPARE(sec->normals().at(2), n);
    QCOMPARE(sec->normals().at(3), n);
    QCOMPARE(sec->normals().at(4), n);
    QCOMPARE(sec->indices().count(), 12);
    QCOMPARE(sec->indices().at(0), 0);
    QCOMPARE(sec->indices().at(1), 1);
    QCOMPARE(sec->indices().at(2), 2);
    QCOMPARE(sec->indices().at(3), 1);
    QCOMPARE(sec->indices().at(4), 3);
    QCOMPARE(sec->indices().at(5), 2);
    QCOMPARE(sec->indices().at(6), 3);
    QCOMPARE(sec->indices().at(7), 4);
    QCOMPARE(sec->indices().at(8), 2);
    QCOMPARE(sec->indices().at(9), 4);
    QCOMPARE(sec->indices().at(10), 0);
    QCOMPARE(sec->indices().at(11), 2);
    QCOMPARE(sec->dataTypes(), QLogicalVertex::Vertex | QLogicalVertex::Normal);

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 12);
}

void tst_QGLDisplayList::extrude()
{
    TestQGLDisplayList displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QGL::VectorArray edges;
    edges << a << b << c << d;

    qreal one_on_root2 = 1.0f / sqrt(2.0f);
    QVector3D n1(-one_on_root2, -one_on_root2, 0.0f);
    QVector3D n2(one_on_root2, -one_on_root2, 0.0f);
    QVector3D n3(one_on_root2, one_on_root2, 0.0f);
    QVector3D n4(-one_on_root2, one_on_root2, 0.0f);

    displayList.extrude(edges, -n);
    sec->finalize();

    QCOMPARE(sec->vertices().count(), edges.count() * 2);
    QGL::VectorArray vrts = sec->vertices();
    QGL::VectorArray nrms = sec->normals();
    QGL::IndexArray inxs = sec->indices();

    QCOMPARE(vrts.count(), 8);
    QCOMPARE(vrts.at(0), edges.at(0) - n);
    QCOMPARE(vrts.at(1), edges.at(1) - n);
    QCOMPARE(vrts.at(2), edges.at(1));
    QCOMPARE(vrts.at(3), edges.at(0));
    QCOMPARE(vrts.at(4), edges.at(2) - n);
    QCOMPARE(vrts.at(5), edges.at(2));
    QCOMPARE(vrts.at(6), edges.at(3) - n);
    QCOMPARE(vrts.at(7), edges.at(3));

    QCOMPARE(nrms.count(), 8);
    QCOMPARE(nrms.at(0).normalized(), n1);
    QCOMPARE(nrms.at(1).normalized(), n2);
    QCOMPARE(nrms.at(2).normalized(), n2);
    QCOMPARE(nrms.at(3).normalized(), n1);
    QCOMPARE(nrms.at(4).normalized(), n3);
    QCOMPARE(nrms.at(5).normalized(), n3);
    QCOMPARE(nrms.at(6).normalized(), n4);
    QCOMPARE(nrms.at(7).normalized(), n4);

    static int checkIx[] = {
        0, 1, 2, 0, 2, 3,
        1, 4, 5, 1, 5, 2,
        4, 6, 7, 4, 7, 5,
        6, 0, 3, 6, 3, 7
    };
    QCOMPARE(sec->indices().count(), 24);

    QCOMPARE(inxs.at(0), checkIx[0]);
    QCOMPARE(inxs.at(1), checkIx[1]);
    QCOMPARE(inxs.at(2), checkIx[2]);
    QCOMPARE(inxs.at(3), checkIx[3]);
    QCOMPARE(inxs.at(4), checkIx[4]);
    QCOMPARE(inxs.at(5), checkIx[5]);
    QCOMPARE(inxs.at(6), checkIx[6]);
    QCOMPARE(inxs.at(7), checkIx[7]);
    QCOMPARE(inxs.at(8), checkIx[8]);
    QCOMPARE(inxs.at(9), checkIx[9]);
    QCOMPARE(inxs.at(10), checkIx[10]);
    QCOMPARE(inxs.at(11), checkIx[11]);
    QCOMPARE(inxs.at(12), checkIx[12]);
    QCOMPARE(inxs.at(13), checkIx[13]);
    QCOMPARE(inxs.at(14), checkIx[14]);
    QCOMPARE(inxs.at(15), checkIx[15]);
    QCOMPARE(inxs.at(16), checkIx[16]);
    QCOMPARE(inxs.at(17), checkIx[17]);
    QCOMPARE(inxs.at(18), checkIx[18]);
    QCOMPARE(inxs.at(19), checkIx[19]);
    QCOMPARE(inxs.at(20), checkIx[20]);
    QCOMPARE(inxs.at(21), checkIx[21]);
    QCOMPARE(inxs.at(22), checkIx[22]);
    QCOMPARE(inxs.at(23), checkIx[23]);

    QCOMPARE(sec->dataTypes(), QLogicalVertex::Vertex | QLogicalVertex::Normal);

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 24);
}

void tst_QGLDisplayList::finalize()
{
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);

    // backwards wound triangle - on back face
    QVector3D e(1.0f, -1.0f, 1.0f);
    QVector3D f(-1.0f, -1.0f, 1.0f);
    QVector3D g(-1.0f, 1.0f, 1.0f);
    // normal points back
    QVector3D n10(0.0f, 0.0f, -1.0f);

    QGL::VectorArray edges;
    edges << a << b << c << d;

    qreal one_on_root2 = 1.0f / sqrt(2.0f);
    QVector3D n0(0.0f, 0.0f, 1.0f);
    QVector3D n1(-one_on_root2, -one_on_root2, 0.0f);
    QVector3D n2(one_on_root2, -one_on_root2, 0.0f);
    QVector3D n3(one_on_root2, one_on_root2, 0.0f);
    QVector3D n4(-one_on_root2, one_on_root2, 0.0f);

    TestQGLDisplayList displayList;
    displayList.newSection();
    QGLSceneNode *node = displayList.currentNode();

    displayList.addTriangulatedFace(center, edges);

    displayList.newSection();
    QGLSceneNode *node2 = displayList.currentNode();

    displayList.extrude(edges, -n);

    displayList.newSection();
    QGLSceneNode *node3 = displayList.currentNode();

    QGLTextureModel model(0.0f, 0.0f, 1.0f, 1.0f);
    // reverse winding, backwards normal == n10
    displayList.addTriangle(e, f, g, QVector3D(), model);

    displayList.finalize();
    QCOMPARE(displayList.sections().count(), 0);

    QGLGeometry *geom = displayList.geometry();

    QGLVertexArray verts = geom->vertexArray();
    QGLIndexArray ids = geom->indexArray();

    QCOMPARE(verts.vertexCount(), 13);
    QCOMPARE(ids.size(), 36);

    // triangulated face
    int tf = ids[node->start()]; // beginning of triangulated face
    QCOMPARE(node->count(), 12);
    QCOMPARE(verts.vector3DAt(tf, QGL::Position), a);
    QCOMPARE(verts.vector3DAt(tf + 2, QGL::Position), center);
    QCOMPARE(verts.vector3DAt(tf, QGL::Normal), n0);
    QCOMPARE(verts.vector3DAt(tf + 2, QGL::Normal), n0);

    int ext = ids[node2->start()]; // beginning of extrude
    int last = ids[node2->start() + (node2->count() - 1)];
    QCOMPARE(node2->count(), 24);
    QCOMPARE(verts.vector3DAt(ext, QGL::Position), a - n);
    QCOMPARE(verts.vector3DAt(ext, QGL::Normal), n1);
    QCOMPARE(verts.vector3DAt(last, QGL::Position), d);
    QCOMPARE(verts.vector3DAt(last, QGL::Normal), n4);

#ifndef QT_NO_MEMBER_TEMPLATES
    QList<QGLSceneNode*> nodes = displayList.findChildren<QGLSceneNode*>();
    QCOMPARE(nodes.count(), 3);
    QVERIFY(nodes.contains(node));
    QVERIFY(nodes.contains(node2));
    QVERIFY(nodes.contains(node3));
#endif

    // the first two nodes and the list itself all reference the same
    // geometry object, since they all have the same types: just vertices
    // and normals.  the last node has geometry with textures, so its in
    // a different geometry object.
    QCOMPARE(node->geometry(), geom);
    QCOMPARE(node2->geometry(), geom);
    QVERIFY(node3->geometry() != geom);

    geom = node3->geometry();
    QGLVertexArray verts2 = geom->vertexArray();
    QGLIndexArray ids2 = geom->indexArray();

    int tri = ids2[node->start()];
    QGLVertexDescription desc = verts2.fields();
    QCOMPARE(verts2.vector3DAt(tri, desc.indexOf(QGL::Position)), e);
    QCOMPARE(verts2.vector3DAt(tri, desc.indexOf(QGL::Normal)), n10);
    QCOMPARE(verts2.vector2DAt(tri, desc.indexOf(QGL::TextureCoord0)), model.bottomLeft());
}

QTEST_APPLESS_MAIN(tst_QGLDisplayList)

#include "tst_qgldisplaylist.moc"
