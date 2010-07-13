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
#include <QtCore/qpointer.h>
#include "qglbuilder.h"
#include "qvector2darray.h"
#include "qvector3darray.h"
#include "qglsection_p.h"
#include "qglmaterialcollection.h"
#include "qglscenenode.h"
#include "qglabstracteffect.h"
#include "qtest_helpers_p.h"
#include "qgeometrydata.h"

class tst_QGLBuilder : public QObject
{
    Q_OBJECT
public:
    tst_QGLBuilder() {}
    ~tst_QGLBuilder() {}

private slots:
    void createDefault();
    void newSection();
    void newNode();
    void pushNode();
    void popNode();
    void geometryBuild();
    void addTriangle();
    void addQuads);
    void addTriangleFan();
    void addTriangulatedFace();
    void extrude();
    void finalize();
};

// Indices in a QGL::IndexArray are int on desktop, ushort on OpenGL/ES.
// This macro works around the discrepancy to avoid confusing QCOMPARE.
#define QCOMPARE_INDEX(x,y)     QCOMPARE(int(x), int(y))

class TestQGLBuilder : public QGLBuilder
{
public:
    QGLSection *currentSection() { return QGLBuilder::currentSection(); }
    QList<QGLSection*> sections() { return QGLBuilder::sections(); }
};

void tst_QGLBuilder::createDefault()
{
    // Test that a newly created list works with no defaults
    TestQGLBuilder displayList0;
    QCOMPARE(displayList0.currentSection(), (QGLSection*)0);
    QCOMPARE(displayList0.sections().size(), 0);
    QCOMPARE(displayList0.currentNode(), (QGLSceneNode*)0);
    QVERIFY(displayList0.geometry().isEmpty());
    QVERIFY(displayList0.palette() != 0);

    QPointer<QGLMaterialCollection> palette = new QGLMaterialCollection();
    QGLBuilder displayList1(0, palette);
    QCOMPARE(displayList1.palette(), palette.data());

    QObject *obj = new QObject();
    QPointer<QGLBuilder> displayList2 = new QGLBuilder(obj, palette);
    QCOMPARE(displayList2->palette(), palette.data());
    QCOMPARE(displayList2->parent(), obj);
    QCOMPARE(obj->children().at(0), displayList2.data());
    delete obj;
    QVERIFY(displayList2.isNull());  // got destroyed by being a child
    QVERIFY(!palette.isNull());  // palette did not get destroyed
}

void tst_QGLBuilder::newSection()
{
    TestQGLBuilder displayList;
    displayList.newSection(); // defaults to smooth
    QGLSection *s = displayList.currentSection();
    QCOMPARE(s, displayList.currentSection());
    QCOMPARE(displayList.sections().count(), 1);
    QVERIFY(displayList.sections().contains(s));
    QCOMPARE(s->smoothing(), QGL::Smooth);
    displayList.newSection(QGL::Faceted);
    QGLSection *s2 = displayList.currentSection();
    QCOMPARE(s2, displayList.currentSection());
    QCOMPARE(displayList.sections().count(), 2);
    QVERIFY(displayList.sections().contains(s2));
}

class TestEffect : public QGLAbstractEffect
{
    QList<QGL::VertexAttribute> requiredFields() const
    {
        return QList<QGL::VertexAttribute>();
    }
    void setActive(QGLPainter *, bool)
    {
    }
};

void tst_QGLBuilder::newNode()
{
    TestQGLBuilder displayList;
    displayList.newSection();  // calls new node
    QGLSceneNode *node = displayList.currentNode();

    // newly created node works and has all defaults
    QCOMPARE(node->parent(), &displayList);
    QCOMPARE(node->effect(), QGL::LitMaterial); // lit material is the default
    QCOMPARE(node->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node->materialIndex(), -1);
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 0);

    // add some settings and geometry to the new node
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    node->setUserEffect(eff);
    node->setMaterialIndex(5);
    QGLPrimitive p;
    p.appendVertex(QVector3D(), QVector3D(1.0f, 2.0f, 3.0f), QVector3D(4.0f, 5.0f, 6.0f));
    displayList.addTriangle(p);

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
    QCOMPARE(node2->materialIndex(), -1);

    displayList.addTriangle(p);
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

void tst_QGLBuilder::pushNode()
{
    TestQGLBuilder displayList;
    displayList.newSection();
    QGLSceneNode *node = displayList.newNode();
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    node->setUserEffect(eff);
    node->setMaterialIndex(5);
    QGLPrimitive p;
    p.appendVertex(QVector3D(), QVector3D(1.0f, 2.0f, 3.0f), QVector3D(4.0f, 5.0f, 6.0f));
    displayList.addTriangle(p);

    QGLSceneNode *node2 = displayList.pushNode();
    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 3);
    QCOMPARE(displayList.currentNode(), node2);
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 0);
    QCOMPARE(node2->parent(), node);
    QCOMPARE(node2->effect(), QGL::LitMaterial); // lit material is the default
    QCOMPARE(node2->userEffect(), (QGLAbstractEffect *)0);
    QCOMPARE(node2->materialIndex(), -1);
}

void tst_QGLBuilder::popNode()
{
    TestQGLBuilder displayList;
    displayList.newSection();
    QGLSceneNode *node = displayList.newNode();
    node->setEffect(QGL::LitDecalTexture2D);
    QGLAbstractEffect *eff = new TestEffect;
    QMatrix4x4 mat;
    mat.rotate(45.0f, 0.0f, 1.0f, 0.0f);
    node->setLocalTransform(mat);
    node->setUserEffect(eff);
    node->setMaterialIndex(5);
    QGLPrimitive p;
    p.appendVertex(QVector3D(), QVector3D(1.0f, 2.0f, 3.0f), QVector3D(4.0f, 5.0f, 6.0f));
    displayList.addTriangle(p);

    QGLSceneNode *node2 = displayList.pushNode();

    displayList.addTriangle(p);

    QGLSceneNode *node3 = displayList.popNode();
    QCOMPARE(node2->start(), 3);
    QCOMPARE(node2->count(), 3);

    QCOMPARE(node3->effect(), QGL::LitDecalTexture2D);
    QCOMPARE(node3->userEffect(), eff);
    QCOMPARE(node3->materialIndex(), 5);
    QCOMPARE(node3->localTransform(), mat);
    QCOMPARE(node3->start(), 6);
    QCOMPARE(node3->count(), 0);
}

void tst_QGLBuilder::geometryBuild()
{
    // here we really just test that the right values get added
    // to the underlying QGLPrimitive - the function of the actual
    // building that takes place when end() is called is tested by
    // the addTriangle() and other tests below
    TestQGLBuilder displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();

    QCOMPARE(sec->count(), 0);  // empty to start off with
    QCOMPARE(node->count(), 0);

    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D e(1.0f, 2.0f, 0.0f);
    QVector3D f(-1.0f, 2.0f, 0.0f);
    QVector3D origin;
    QVector3D norm(0.0f, 0.0f, 1.0f);

    displayList.begin(QGL::TRIANGLE);
    QGLPrimitive *p = displayList.currentPrimitive();
    QVERIFY(p != (QGLPrimitive*)0);
    displayList.addVertex(a);
    displayList.addVertex(b.x(), b.y());
    displayList.addVertex(c.x(), c.y(), c.z());
    QCOMPARE(p->count(), 3);
    QCOMPARE(p->vertex(0), a);
    QCOMPARE(p->vertex(1), b);
    QCOMPARE(p->vertex(2), c);
    displayList.end();
    QCOMPARE(displayList.currentPrimitive(), (QGLPrimitive*)0);
    QCOMPARE(sec->count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(node->count(), 3);

    displayList.newSection();
    displayList.begin(QGL::TRIANGLE_STRIP);
    p = displayList.currentPrimitive();
    QVERIFY(p != (QGLPrimitive*)0);  // should get a whole new primitive here
    sec = displayList.currentSection();
    node = displayList.currentNode();
    QCOMPARE(sec->count(), 0);  // empty to start off with
    QCOMPARE(node->count(), 0);
    QVector3DArray data;
    data << a << b << d << c << f << e;
    displayList.addVertexArray(data);
    QCOMPARE(p->count(), 6);
    QCOMPARE(p->vertex(0), a);
    QCOMPARE(p->vertex(1), b);
    QCOMPARE(p->vertex(2), d);
    QCOMPARE(p->vertex(3), c);
    QCOMPARE(p->vertex(4), f);
    QCOMPARE(p->vertex(5), e);

    // this will implicitly call displayList.end() -  test the end of TRIANGLE_STRIP
    displayList.begin(QGL::TRIANGLE_FAN);
    p = displayList.currentPrimitive();
    QVERIFY(p != (QGLPrimitive*)0);  // should get a whole new primitive here
    // but the section will contain the data from posting the last primitive
    QCOMPARE(sec->count(), 6);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), d);
    QCOMPARE(sec->vertex(3), c);
    QCOMPARE(sec->vertex(4), f);
    QCOMPARE(sec->vertex(5), e);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->normal(5).normalized(), norm);
    QCOMPARE(node->count(), 12); // TRIANGLE_STRIP will here draw 4 triangles = 12 indices

    // now go on and test TRIANGLE_FAN
    displayList.newSection();
    sec = displayList.currentSection();
    node = displayList.currentNode();
    displayList.addVertex(a);
    displayList.addNormal(norm);
    displayList.addVertex(b);
    displayList.addNormal(norm);
    displayList.addVertex(c);
    displayList.addNormal(norm);
    displayList.addVertex(d);
    displayList.addNormal(norm);
    displayList.addVertex(e);
    displayList.addNormal(norm);
    QCOMPARE(p->count(), 5);
    QCOMPARE(p->vertex(0), a);
    QCOMPARE(p->vertex(4), e);
    QCOMPARE(p->normal(0), norm);
    QCOMPARE(p->normal(4), norm);
    displayList.end();
    QCOMPARE(sec->count(), 5);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->vertex(4), e);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(4).normalized(), norm);
    QCOMPARE(node->count(), 9); // TRIANGLE_FAN will here draw 3 triangles = 9 indices

    displayList.begin(QGL::TRIANGULATED_FACE);
    displayList.newSection();
    sec = displayList.currentSection();
    node = displayList.currentNode();
    displayList.addVertex(a);
    displayList.addColor(Qt::red);
    displayList.addVertex(b);
    displayList.addColor(QColor4ub(Qt::green));
    displayList.addVertex(c);
    displayList.addVertex(d);
    displayList.addVertex(e);
    QArray<QColor4ub> cdata;
    cdata.append(Qt::blue, Qt::yellow, Qt::black);
    displayList.addColorArray(cdata);
    QCOMPARE(p->count(), 5);
    QCOMPARE(p->vertex(0), a);
    QCOMPARE(p->vertex(4), e);
    QCOMPARE(p->color(0), QColor4ub(Qt::red));
    QCOMPARE(p->color(1), QColor4ub(Qt::green));
    QCOMPARE(p->color(2), QColor4ub(Qt::blue));
    QCOMPARE(p->color(3), QColor4ub(Qt::yellow));
    QCOMPARE(p->color(4), QColor4ub(Qt::black));
    displayList.setFlags(QGL::USE_VERTEX_0_AS_CTR);
    QVERIFY(p->flags() == QGL::USE_VERTEX_0_AS_CTR);
    displayList.end();
    QCOMPARE(sec->count(), 5);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(4), e);
    QCOMPARE(p->color(0), QColor4ub(Qt::red));
    QCOMPARE(p->color(1), QColor4ub(Qt::green));
    QCOMPARE(p->color(2), QColor4ub(Qt::blue));
    QCOMPARE(p->color(3), QColor4ub(Qt::yellow));
    QCOMPARE(p->color(4), QColor4ub(Qt::black));
    QCOMPARE(node->count(), 12); // TRIANGLE_FAN will here draw 4 triangles = 12 indices
}

void tst_QGLBuilder::addTriangle()
{
    TestQGLBuilder displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector2D ta(0.0, 0.0);
    QVector2D tb(1.0, 0.0);
    QVector2D tc(1.0, 1.0);
    QVector3D norm(0.0f, 0.0f, 1.0f);
    QGLPrimitive p;
    p.appendVertex(a, b, c);
    p.setCommonNormal(norm);

    displayList.addTriangle(p);
    QCOMPARE(sec->vertices().count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->normals().count(), 3);
    QCOMPARE(sec->indices().size(), 3);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector2D td(0.0f, 1.0f);
    QGLPrimitive q;
    q.appendVertex(a, c, d);
    q.setCommonNormal(norm);
    displayList.addTriangle(q);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    displayList.newSection();
    sec = displayList.currentSection();
    node = displayList.currentNode();
    QGLPrimitive r;
    r.appendVertex(a, b, c);
    r.appendTexCoord(ta, tb, tc);
    displayList.addTriangle(r);
    QCOMPARE(sec->vertices().count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->normals().count(), 3);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 3);
    QCOMPARE(sec->texCoords().at(0), ta);
    QCOMPARE(sec->texCoords().at(1), tb);
    QCOMPARE(sec->texCoords().at(2), tc);
    QCOMPARE(sec->indices().size(), 3);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE(sec->fields(),
             QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal) | QGL::fieldMask(QGL::TextureCoord0));

    QGLPrimitive s;
    s.appendVertex(a, b, d);
    s.appendTexCoord(ta, tb, td);
    displayList.addTriangle(s);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 4);
    QCOMPARE(sec->texCoords().at(0), QVector2D(0.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(1), QVector2D(1.0f, 0.0f));
    QCOMPARE(sec->texCoords().at(2), QVector2D(1.0f, 1.0f));
    QCOMPARE(sec->texCoords().at(3), QVector2D(0.0f, 1.0f));
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 1);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(),
             QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal) | QGL::fieldMask(QGL::TextureCoord0));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    // correctly handle not being a multiple of 3 - discard spare ones
    displayList.newSection();
    sec = displayList.currentSection();
    QGLPrimitive t;
    t.appendVertex(a, b, c, d);
    displayList.addTriangle(t);
    QCOMPARE(sec->count(), 3);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
}

void tst_QGLBuilder::addQuads)
{
    TestQGLBuilder displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D norm(0.0f, 0.0f, 1.0f);
    QGLPrimitive p;
    p.appendVertex(a, b, c, d);
    p.setCommonNormal(norm);

    displayList.addQuad(p);
    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);

    displayList.newSection();
    sec = displayList.currentSection();
    node = displayList.currentNode();
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    QVector2D td(0.0f, 1.0f);
    QGLPrimitive q;
    q.appendVertex(a, b, c, d);
    q.appendTexCoord(ta, tb, tc, td);
    displayList.addQuad(q);

    QCOMPARE(sec->vertices().count(), 4);
    QCOMPARE(sec->vertex(0), a);
    QCOMPARE(sec->vertex(1), b);
    QCOMPARE(sec->vertex(2), c);
    QCOMPARE(sec->vertex(3), d);
    QCOMPARE(sec->normals().count(), 4);
    QCOMPARE(sec->normal(0).normalized(), norm);
    QCOMPARE(sec->normal(1).normalized(), norm);
    QCOMPARE(sec->normal(2).normalized(), norm);
    QCOMPARE(sec->normal(3).normalized(), norm);
    QCOMPARE(sec->texCoords().count(), 4);
    QCOMPARE(sec->texCoords().at(0), ta);
    QCOMPARE(sec->texCoords().at(1), tb);
    QCOMPARE(sec->texCoords().at(2), tc);
    QCOMPARE(sec->texCoords().at(3), td);
    QCOMPARE(sec->indices().size(), 6);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE(sec->fields(),
             QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal) | QGL::fieldMask(QGL::TextureCoord0));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 6);
}

void tst_QGLBuilder::addTriangleFan()
{
    TestQGLBuilder displayList;
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
    QVector2DArray edges;

    // if edges has length < 2, exit without doing anything
    QGLPrimitive p;
    p.appendVertex(center);
    displayList.addTriangleFan(p);
    QCOMPARE(sec->vertices().count(), 0);
    QCOMPARE(sec->normals().count(), 0);
    QCOMPARE(sec->indices().size(), 0);

    p.appendVertex(a, b, c, d);

    displayList.addTriangleFan(p);
    sec->normalizeNormals();
    QCOMPARE(sec->vertices().count(), 9);
    QCOMPARE(sec->vertex(0), center);
    QCOMPARE(sec->vertex(1), a);
    QCOMPARE(sec->vertex(2), b);
    QCOMPARE(sec->vertex(3), center);
    QCOMPARE(sec->vertex(4), b);
    QCOMPARE(sec->vertex(5), c);
    QCOMPARE(sec->vertex(6), center);
    QCOMPARE(sec->vertex(7), c);
    QCOMPARE(sec->vertex(8), d);
    QCOMPARE(sec->normals().count(), 9);
    QCOMPARE(sec->normal(0), n1);
    QCOMPARE(sec->normal(1), n1);
    QCOMPARE(sec->normal(2), n1);
    QCOMPARE(sec->normal(3), n2);
    QCOMPARE(sec->normal(4), n2);
    QCOMPARE(sec->normal(5), n2);
    QCOMPARE(sec->normal(6), n3);
    QCOMPARE(sec->normal(7), n3);
    QCOMPARE(sec->normal(8), n3);
    QCOMPARE(sec->indices().size(), 9);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 3);
    QCOMPARE_INDEX(sec->indices()[4], 4);
    QCOMPARE_INDEX(sec->indices()[5], 5);
    QCOMPARE_INDEX(sec->indices()[6], 6);
    QCOMPARE_INDEX(sec->indices()[7], 7);
    QCOMPARE_INDEX(sec->indices()[8], 8);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 9);
}

void tst_QGLBuilder::addTriangulatedFace()
{
    TestQGLBuilder displayList;
    displayList.newSection(QGL::Faceted);
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QVector3D center(0.0f, 0.0f, 0.0f);
    QVector2DArray edges;

    // if edges has length < 2, exit without doing anything
    QGLPrimitive p;
    displayList.addTriangleFan(p);
    QCOMPARE(sec->vertices().count(), 0);
    QCOMPARE(sec->normals().count(), 0);
    QCOMPARE(sec->indices().size(), 0);

    p.appendVertex(center);
    p.appendVertex(a, b, c, d);
    p.setFlags(QGL::USE_VERTEX_0_AS_CTR);

    displayList.addTriangulatedFace(p);
    sec->normalizeNormals();
    QCOMPARE(sec->vertices().count(), 5);
    QCOMPARE(sec->vertex(0), center);
    QCOMPARE(sec->vertex(1), a);
    QCOMPARE(sec->vertex(2), b);
    QCOMPARE(sec->vertex(3), c);
    QCOMPARE(sec->vertex(4), d);
    QCOMPARE(sec->normals().count(), 5);
    QCOMPARE(sec->normal(0), n);
    QCOMPARE(sec->normal(1), n);
    QCOMPARE(sec->normal(2), n);
    QCOMPARE(sec->normal(3), n);
    QCOMPARE(sec->normal(4), n);
    QCOMPARE(sec->indices().size(), 12);
    QCOMPARE_INDEX(sec->indices()[0], 0);
    QCOMPARE_INDEX(sec->indices()[1], 1);
    QCOMPARE_INDEX(sec->indices()[2], 2);
    QCOMPARE_INDEX(sec->indices()[3], 0);
    QCOMPARE_INDEX(sec->indices()[4], 2);
    QCOMPARE_INDEX(sec->indices()[5], 3);
    QCOMPARE_INDEX(sec->indices()[6], 0);
    QCOMPARE_INDEX(sec->indices()[7], 3);
    QCOMPARE_INDEX(sec->indices()[8], 4);
    QCOMPARE_INDEX(sec->indices()[9], 0);
    QCOMPARE_INDEX(sec->indices()[10], 4);
    QCOMPARE_INDEX(sec->indices()[11], 1);
    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 12);
}

void tst_QGLBuilder::extrude()
{
    TestQGLBuilder displayList;
    displayList.newSection();
    QGLSection *sec = displayList.currentSection();
    QGLSceneNode *node = displayList.currentNode();
    QVector3D a(-1.0f, -1.0f, 0.0f);
    QVector3D b(1.0f, -1.0f, 0.0f);
    QVector3D c(1.0f, 1.0f, 0.0f);
    QVector3D d(-1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QVector3DArray edges;

    // if edges has length < 2, exit without doing anything
    QGLPrimitive p;
    QGLPrimitive q;
    displayList.addQuadsZipped(p, q);
    QCOMPARE(sec->vertices().count(), 0);
    QCOMPARE(sec->normals().count(), 0);
    QCOMPARE(sec->indices().size(), 0);

    p.appendVertex(a, b, c, d);
    edges = p.vertices();
    p.appendVertex(a);
    q = p.translated(-n);

    qreal one_on_root2 = 1.0f / sqrt(2.0f);
    QVector3D n1(-one_on_root2, -one_on_root2, 0.0f);
    QVector3D n2(one_on_root2, -one_on_root2, 0.0f);
    QVector3D n3(one_on_root2, one_on_root2, 0.0f);
    QVector3D n4(-one_on_root2, one_on_root2, 0.0f);

    displayList.addQuadsZipped(p, q);
    //sec->normalizeNormals();

    QCOMPARE(sec->count(), 8);
    QVector3DArray vrts = sec->vertices();
    QVector3DArray nrms = sec->normals();
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
    QCOMPARE(sec->indices().size(), 24);

    QCOMPARE_INDEX(inxs[0], checkIx[0]);
    QCOMPARE_INDEX(inxs[1], checkIx[1]);
    QCOMPARE_INDEX(inxs[2], checkIx[2]);
    QCOMPARE_INDEX(inxs[3], checkIx[3]);
    QCOMPARE_INDEX(inxs[4], checkIx[4]);
    QCOMPARE_INDEX(inxs[5], checkIx[5]);
    QCOMPARE_INDEX(inxs[6], checkIx[6]);
    QCOMPARE_INDEX(inxs[7], checkIx[7]);
    QCOMPARE_INDEX(inxs[8], checkIx[8]);
    QCOMPARE_INDEX(inxs[9], checkIx[9]);
    QCOMPARE_INDEX(inxs[10], checkIx[10]);
    QCOMPARE_INDEX(inxs[11], checkIx[11]);
    QCOMPARE_INDEX(inxs[12], checkIx[12]);
    QCOMPARE_INDEX(inxs[13], checkIx[13]);
    QCOMPARE_INDEX(inxs[14], checkIx[14]);
    QCOMPARE_INDEX(inxs[15], checkIx[15]);
    QCOMPARE_INDEX(inxs[16], checkIx[16]);
    QCOMPARE_INDEX(inxs[17], checkIx[17]);
    QCOMPARE_INDEX(inxs[18], checkIx[18]);
    QCOMPARE_INDEX(inxs[19], checkIx[19]);
    QCOMPARE_INDEX(inxs[20], checkIx[20]);
    QCOMPARE_INDEX(inxs[21], checkIx[21]);
    QCOMPARE_INDEX(inxs[22], checkIx[22]);
    QCOMPARE_INDEX(inxs[23], checkIx[23]);

    QCOMPARE(sec->fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));

    QCOMPARE(node->start(), 0);
    QCOMPARE(node->count(), 24);
}

void tst_QGLBuilder::finalize()
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

    QGLPrimitive p;
    p.appendVertex(center);
    p.setFlags(QGL::USE_VERTEX_0_AS_CTR);
    p.appendVertex(a, b, c, d);

    qreal one_on_root2 = 1.0f / sqrt(2.0f);
    QVector3D n0(0.0f, 0.0f, 1.0f);
    QVector3D n1(-one_on_root2, -one_on_root2, 0.0f);
    QVector3D n2(one_on_root2, -one_on_root2, 0.0f);
    QVector3D n3(one_on_root2, one_on_root2, 0.0f);
    QVector3D n4(-one_on_root2, one_on_root2, 0.0f);

    TestQGLBuilder displayList;
    displayList.newSection();
    QGLSceneNode *node = displayList.currentNode();

    displayList.addTriangulatedFace(p);

    displayList.newSection();
    QGLSceneNode *node2 = displayList.currentNode();

    QGLPrimitive s;
    s.appendVertex(a, b, c, d);
    s.appendVertex(a);
    displayList.addQuadsZipped(s, s.translated(-n));

    QPointer<QGLSceneNode> nodeEmpty0 = displayList.newNode();

    displayList.newSection();
    QGLSceneNode *node3 = displayList.currentNode();

    QGLPrimitive q;
    QVector2D ta(0.0f, 0.0f);
    QVector2D tb(1.0f, 0.0f);
    QVector2D tc(1.0f, 1.0f);
    q.appendVertex(e, f, g);
    q.appendTexCoord(ta, tb, tc);
    // reverse winding, backwards normal == n10
    displayList.addTriangle(q);

    QPointer<QGLSceneNode> nodeEmpty1 = displayList.pushNode();

    displayList.finalize();

    QCOMPARE(nodeEmpty0.data(), (QGLSceneNode*)0);
    QCOMPARE(nodeEmpty1.data(), (QGLSceneNode*)0);

    QCOMPARE(displayList.sections().count(), 0);

    QGeometryData geom = node->geometry();
    QGL::IndexArray ids = geom.indices();

    QCOMPARE(geom.count(QGL::Position), 13);
    QCOMPARE(ids.size(), 36);

    // triangulated face
    int tf = ids[node->start()]; // beginning of triangulated face
    QCOMPARE(node->count(), 12);
    QCOMPARE(geom.vertexAt(tf), center);
    QCOMPARE(geom.vertexAt(tf + 2), b);
    QCOMPARE(geom.normalAt(tf), n0);
    QCOMPARE(geom.normalAt(tf + 2), n0);

    geom = node2->geometry();
    ids = geom.indices();

    int ext = ids[node2->start()]; // beginning of extrude
    int last = ids[node2->start() + (node2->count() - 1)];
    QCOMPARE(node2->count(), 24);
    QCOMPARE(geom.vertexAt(ext), a - n);
    QCOMPARE(geom.normalAt(ext), n1);
    QCOMPARE(geom.vertexAt(last), d);
    QCOMPARE(geom.normalAt(last), n4);

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
    QVERIFY(node->geometry() == geom);
    QVERIFY(node2->geometry() == geom);
    QVERIFY(!(node3->geometry() == geom));

    geom = node3->geometry();
    QGL::IndexArray ids2 = geom.indices();

    int tri = ids2[node->start()];
    QCOMPARE(geom.vertexAt(tri), e);
    QCOMPARE(geom.normalAt(tri), n10);
    QCOMPARE(geom.texCoordAt(tri), ta);
}

QTEST_APPLESS_MAIN(tst_QGLBuilder)

#include "tst_QGLBuilder.moc"
