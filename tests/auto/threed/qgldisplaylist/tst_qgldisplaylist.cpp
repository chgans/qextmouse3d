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
#include "qgldisplaylist.h"
#include "qglmaterialcollection.h"

class tst_QGLDisplayList : public QObject
{
    Q_OBJECT
public:
    tst_QGLDisplayList() {}
    ~tst_QGLDisplayList() {}

private slots:
    void testHelpers();
    void createDefault();
    void createParent();
    void createMaterials();
    void createParentAndMaterials();
    void append();
    void appendSmooth();
    void appendFaceted();
    void appendTexCoord();
    void appendColor();
    void accessors();
    void loadArrays();
    void finalize();
};

class QGLDisplayListTest : public QGLDisplayList
{
public:
    QGLDisplayListTest(QObject *parent = 0, QGLMaterialCollection *materials = 0)
        : QGLDisplayList(parent, materials) { ++ctorCalled; }
    ~QGLDisplayListTest() { ++dtorCalled; }

    void appendSmooth(const QVector3D &a, const QVector3D &n,
                              const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord)
    {
        QGLDisplayList::appendSmooth(a, n, t);
    }

    void appendFaceted(const QVector3D &a, const QVector3D &n,
                       const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord)
    {
        QGLDisplayList::appendFaceted(a, n, t);
    }

    void appendColor(const QVector3D &a, const QColor4b &c,
                     const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord)
    {
        QGLDisplayList::appendColor(a, c, t);
    }

    int updateTexCoord(int position, const QVector2D &t)
    {
        return QGLDisplayList::updateTexCoord(position, t);
    }

    void append(const QVector3D &a,
                const QVector2D &t = QGLTextureSpecifier::InvalidTexCoord)
    {
        QGLDisplayList::append(a, t);
    }

    void finalize()
    {
        QGLDisplayList::finalize();
    }

    void loadArrays(QGLPainter *painter)
    {
        QGLDisplayList::loadArrays(painter);
    }

    void addSection(QGLSection *section)
    {
        QGLDisplayList::addSection(section);
    }

    void draw(QGLPainter *painter, int start, int count)
    {
        QGLDisplayList::draw(painter, start, count);
    }

    QGLDisplayList::VectorArray vertexArray() const
    {
        return QGLDisplayList::vertexArray();
    }

    QGLDisplayList::VectorArray normalArray() const
    {
        return QGLDisplayList::normalArray();
    }

    QGLDisplayList::TexCoordArray texCoordArray() const
    {
        return QGLDisplayList::texCoordArray();
    }

    QGLDisplayList::ColorArray colorArray() const
    {
        return QGLDisplayList::colorArray();
    }

    QGLDisplayList::IndexArray indexArray() const
    {
        return QGLDisplayList::indexArray();
    }

    static int ctorCalled;
    static int dtorCalled;
    static void initCounts()
    {
        ctorCalled = 0;
        dtorCalled = 0;
    }
};

int QGLDisplayListTest::dtorCalled = 0;
int QGLDisplayListTest::ctorCalled = 0;

class QGLMaterialCollectionTest : public QGLMaterialCollection
{
public:
    QGLMaterialCollectionTest(QObject *parent = 0)
       : QGLMaterialCollection(parent)
    {
        ++ctorCalled;
    }
    virtual ~QGLMaterialCollectionTest()
    {
        ++dtorCalled;
    }
    static int ctorCalled;
    static int dtorCalled;
    static void initCounts()
    {
        ctorCalled = 0;
        dtorCalled = 0;
    }
};

class DeleteReceiver : public QObject
{
    Q_OBJECT
public:
    DeleteReceiver() : deleteReceived(false) {}
    bool deleteReceived;
public slots:
    void flagDelete() { deleteReceived = true; }
};

int QGLMaterialCollectionTest::dtorCalled = 0;
int QGLMaterialCollectionTest::ctorCalled = 0;

void tst_QGLDisplayList::testHelpers()
{
    // test that the helper classes are doing the right thing
    QGLDisplayListTest::initCounts();
    QVERIFY(QGLDisplayListTest::dtorCalled == 0);
    QVERIFY(QGLDisplayListTest::ctorCalled == 0);
    {
        QGLDisplayListTest list0;
        QVERIFY(QGLDisplayListTest::ctorCalled == 1);
    }
    QVERIFY(QGLDisplayListTest::dtorCalled == 1);

    QGLMaterialCollectionTest::initCounts();
    QVERIFY(QGLMaterialCollectionTest::dtorCalled == 0);
    QVERIFY(QGLMaterialCollectionTest::ctorCalled == 0);
    {
        QGLMaterialCollectionTest collection0;
        QVERIFY(QGLMaterialCollectionTest::ctorCalled == 1);
    }
    QVERIFY(QGLMaterialCollectionTest::dtorCalled == 1);

    QObject object0;
    DeleteReceiver rec;
    QVERIFY(rec.deleteReceived == false);
    {
        QObject object1;
        QObject::connect(&object1, SIGNAL(destroyed()),
                         &rec, SLOT(flagDelete()));
    }
    QVERIFY(rec.deleteReceived);
}

void tst_QGLDisplayList::createDefault()
{
    // Test that a newly created list works with no defaults
    QGLDisplayListTest::initCounts();
    QGLMaterialCollectionTest::initCounts();
    DeleteReceiver rec0;
    {
        QGLDisplayListTest list0;
        QVERIFY(list0.parent() == NULL);
        QVERIFY(list0.materials() != NULL);
        QObject::connect(list0.materials(), SIGNAL(destroyed()),
                         &rec0, SLOT(flagDelete()));
        QVERIFY(QGLDisplayListTest::ctorCalled == 1);
    }
    // list1 got deleted
    QVERIFY(QGLDisplayListTest::dtorCalled == 1);
    QVERIFY(rec0.deleteReceived);
}

void tst_QGLDisplayList::createParent()
{
    // Test that a newly created list works with parent object
    QGLDisplayListTest::initCounts();
    QGLMaterialCollectionTest::initCounts();
    DeleteReceiver rec1;
    QObject *obj1 = new QObject();
    QGLDisplayListTest * list1 = new QGLDisplayListTest(obj1);
    QVERIFY(list1->parent() == obj1);
    QVERIFY(list1->materials() != NULL);
    QObject::connect(list1->materials(), SIGNAL(destroyed()),
                     &rec1, SLOT(flagDelete()));
    QVERIFY(QGLDisplayListTest::ctorCalled == 1);
    delete obj1;
    // list1 got deleted
    QVERIFY(QGLDisplayListTest::dtorCalled == 1);
    QVERIFY(rec1.deleteReceived);
}

void tst_QGLDisplayList::createMaterials()
{
    // Test that a newly created list works with materials but no parent
    QGLDisplayListTest::initCounts();
    QGLMaterialCollectionTest::initCounts();
    {
        QGLMaterialCollectionTest mats1;
        QVERIFY(QGLMaterialCollectionTest::ctorCalled == 1);
        {
            QGLDisplayListTest list1(0, &mats1);
            QVERIFY(list1.parent() == NULL);
            QVERIFY(list1.materials() == &mats1);
            QVERIFY(QGLDisplayListTest::ctorCalled == 1);
        }
        // list1 got deleted
        QVERIFY(QGLDisplayListTest::dtorCalled == 1);
        QVERIFY(QGLMaterialCollectionTest::dtorCalled == 0);
    }
    QVERIFY(QGLMaterialCollectionTest::dtorCalled == 1);
}

void tst_QGLDisplayList::createParentAndMaterials()
{
    // Test that a newly created list works with materials and parent
    QGLDisplayListTest::initCounts();
    QGLMaterialCollectionTest::initCounts();
    {
        QGLMaterialCollectionTest mats1;
        QVERIFY(QGLMaterialCollectionTest::ctorCalled == 1);
        QObject *obj1 = new QObject();
        QGLDisplayListTest *list1 = new QGLDisplayListTest(obj1, &mats1);
        QVERIFY(list1->parent() == obj1);
        QVERIFY(list1->materials() == &mats1);
        QVERIFY(QGLDisplayListTest::ctorCalled == 1);
        delete obj1;
        // list1 got deleted
        QVERIFY(QGLDisplayListTest::dtorCalled == 1);
        QVERIFY(QGLMaterialCollectionTest::dtorCalled == 0);
    }
    QVERIFY(QGLMaterialCollectionTest::dtorCalled == 1);
}

void tst_QGLDisplayList::append()
{
    QGLDisplayListTest list;
    list.newSection();

    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    list.append(testVertex);
    QVERIFY(list.vertexArray().count() == 1);
    QVERIFY(list.normalArray().count() == 0);
}

void tst_QGLDisplayList::appendSmooth()
{
    QGLDisplayListTest list;
    list.newSection();

    // append a vertex - check it appears in the data along with its normal
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    list.appendSmooth(testVertex, testNormal);
    QVERIFY(list.vertexArray().count() == 1);
    QVERIFY(list.vertexArray().at(0) == testVertex);
    QVERIFY(list.normalArray().count() == 1);
    QVERIFY(list.normalArray().at(0) == testNormal);
    QVERIFY(list.indexArray().count() == 1);
    QVERIFY(list.indexArray().at(0) == 0);

    // append a vertex equal to one already appended - check it was coalesced
    QVector3D testNormal2(0.0f, 0.0f, 1.0f);
    QVector3D result = testNormal + testNormal2; // QVector3D(0.0f, 0.0f, 7.543f);
    list.appendSmooth(testVertex, testNormal2);
    QVERIFY(list.vertexArray().count() == 1);
    QVERIFY(list.vertexArray().at(0) == testVertex);
    QVERIFY(list.normalArray().count() == 1);
    QVERIFY(list.normalArray().at(0) == result);
    QVERIFY(list.indexArray().count() == 2);
    QVERIFY(list.indexArray().at(1) == 0);

    // append a new  different vertex - check it is not coalesced
    QVector3D testVertex2(-1.234f, -2.345f, -3.456f);
    QVector3D testNormal3(-4.321f, -5.432f, -6.543f);
    list.appendSmooth(testVertex2, testNormal3);
    QVERIFY(list.vertexArray().count() == 2);
    QVERIFY(list.vertexArray().at(1) == testVertex2);
    QVERIFY(list.normalArray().count() == 2);
    QVERIFY(list.normalArray().at(1) == testNormal3);
    QVERIFY(list.indexArray().count() == 3);
    QVERIFY(list.indexArray().at(2) == 1);

    // append a vertex equal to one already appended, but inside a new patch - check its not coalesced
    list.newSection();
    list.appendSmooth(testVertex2, testNormal3);
    QVERIFY(list.vertexArray().count() == 3);
    QVERIFY(list.vertexArray().at(2) == testVertex2);
    QVERIFY(list.normalArray().count() == 3);
    QVERIFY(list.normalArray().last() == testNormal3);
    QVERIFY(list.indexArray().at(3) == 2);
    QVERIFY(list.indexArray().count() == 4);
}

void tst_QGLDisplayList::appendFaceted()
{
    QGLDisplayListTest list;
    list.newSection(QGL::Faceted);

    // append a vertex - check it appears in the data along with its normal
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    list.appendFaceted(testVertex, testNormal);
    QVERIFY(list.vertexArray().count() == 1);
    QVERIFY(list.vertexArray().at(0) == testVertex);
    QVERIFY(list.normalArray().count() == 1);
    QVERIFY(list.normalArray().at(0) == testNormal);
    QVERIFY(list.indexArray().count() == 1);
    QVERIFY(list.indexArray().at(0) == 0);

    // append a vertex equal to one already appended, but with different normal - check it was NOT coalesced
    QVector3D testNormal2(0.0f, 0.0f, 1.0f);
    list.appendFaceted(testVertex, testNormal2);
    QVERIFY(list.vertexArray().count() == 2);
    QVERIFY(list.vertexArray().at(1) == testVertex);
    QVERIFY(list.normalArray().count() == 2);
    QVERIFY(list.normalArray().at(1) == testNormal2);
    QVERIFY(list.indexArray().count() == 2);
    QVERIFY(list.indexArray().at(1) == 1);

    // append a vertex equal to one already appended, but with same normal - check it WAS coalesced
    list.appendFaceted(testVertex, testNormal2);
    QVERIFY(list.vertexArray().count() == 2);
    QVERIFY(list.vertexArray().at(1) == testVertex);
    QVERIFY(list.normalArray().count() == 2);
    QVERIFY(list.normalArray().at(1) == testNormal2);
    QVERIFY(list.indexArray().count() == 3);
    QVERIFY(list.indexArray().at(2) == 1);

    // append a vertex equal to one already appended, with same normal, BUT in a new patch - check it was NOT coalesced
    list.newSection();
    list.appendFaceted(testVertex, testNormal2);
    QVERIFY(list.vertexArray().count() == 3);
    QVERIFY(list.vertexArray().at(2) == testVertex);
    QVERIFY(list.normalArray().count() == 3);
    QVERIFY(list.normalArray().at(2) == testNormal2);
    QVERIFY(list.indexArray().count() == 4);
    QVERIFY(list.indexArray().at(3) == 2);
}

void tst_QGLDisplayList::appendTexCoord()
{
    QGLDisplayListTest list;
    list.newSection();

    // note that the tests above do the case of texCoord == InvalidTexCoord

    // append a texture - check it appears in the data at the correct index
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    QVector2D testTexCoord(0.0f, 0.0f);
    list.appendSmooth(testVertex, testNormal, testTexCoord);
    QVERIFY(list.vertexArray().count() == 1);
    QVERIFY(list.vertexArray().at(0) == testVertex);
    QVERIFY(list.normalArray().count() == 1);
    QVERIFY(list.normalArray().at(0) == testNormal);
    QVERIFY(list.texCoordArray().count() == 1);
    QVERIFY(list.texCoordArray().at(0) == testTexCoord);
    QVERIFY(list.indexArray().count() == 1);
    QVERIFY(list.indexArray().at(0) == 0);

    // append same texture - will coalesce and index the vert
    list.appendSmooth(testVertex, testNormal, testTexCoord);
    QVERIFY(list.vertexArray().count() == 1);
    QVERIFY(list.vertexArray().at(0) == testVertex);
    QVERIFY(list.normalArray().count() == 1);
    QVERIFY(list.normalArray().at(0) == testNormal);
    QVERIFY(list.texCoordArray().count() == 1);
    QVERIFY(list.texCoordArray().at(0) == testTexCoord);
    QVERIFY(list.indexArray().count() == 2);
    QVERIFY(list.indexArray().at(1) == 0);

    // new vertex created to carry the updated texture coord, even though
    // the normal and vertex are the same and thus the logical vert would
    // otherwise coalesced - the new vert is a copy (same vert and normal)
    QVector2D testTexCoord2(1.0, 1.0);
    list.appendSmooth(testVertex, testNormal, testTexCoord2);
    QVERIFY(list.vertexArray().count() == 2);
    QVERIFY(list.vertexArray().at(1) == testVertex);
    QVERIFY(list.normalArray().count() == 2);
    QVERIFY(list.normalArray().at(1) == testNormal);
    QVERIFY(list.texCoordArray().count() == 2);
    QVERIFY(list.texCoordArray().at(1) == testTexCoord2);
    QVERIFY(list.indexArray().count() == 3);
    QVERIFY(list.indexArray().at(2) == 1);

    list.newSection(QGL::Faceted);

    // append a faceted vertex with a tex coord check it appears in the data
    // in a new section now, so the same vert and normal wont be coalesced
    list.appendFaceted(testVertex, testNormal, testTexCoord);
    QVERIFY(list.vertexArray().count() == 3);
    QVERIFY(list.vertexArray().at(2) == testVertex);
    QVERIFY(list.normalArray().count() == 3);
    QVERIFY(list.normalArray().at(2) == testNormal);
    QVERIFY(list.texCoordArray().count() == 3);
    QVERIFY(list.texCoordArray().at(2) == testTexCoord);
    QVERIFY(list.indexArray().count() == 4);
    QVERIFY(list.indexArray().at(3) == 2);

    // append a vertex & normal equal to one already appended, but with different tex coord
    // check it was NOT coalesced, dup vert created
    list.appendFaceted(testVertex, testNormal, testTexCoord2);
    QVERIFY(list.vertexArray().count() == 4);
    QVERIFY(list.vertexArray().at(3) == testVertex);
    QVERIFY(list.normalArray().count() == 4);
    QVERIFY(list.normalArray().at(3) == testNormal);
    QVERIFY(list.texCoordArray().count() == 4);
    QVERIFY(list.texCoordArray().at(3) == testTexCoord2);
    QVERIFY(list.indexArray().count() == 5);
    QVERIFY(list.indexArray().at(4) == 3);

    // append a vertex equal to one already appended, but with same normal, and
    // same texture - check it WAS coalesced
    list.appendFaceted(testVertex, testNormal, testTexCoord);
    QVERIFY(list.vertexArray().count() == 4);
    QVERIFY(list.vertexArray().at(3) == testVertex);
    QVERIFY(list.normalArray().count() == 4);
    QVERIFY(list.normalArray().at(3) == testNormal);
    QVERIFY(list.texCoordArray().count() == 4);
    QVERIFY(list.texCoordArray().at(2) == testTexCoord);
    QVERIFY(list.texCoordArray().at(3) == testTexCoord2);
    QVERIFY(list.indexArray().count() == 6);
    QVERIFY(list.indexArray().at(5) == 2);

    QVERIFY(list.hasNormals());
    QVERIFY(list.hasTexCoords());
    QVERIFY(!list.hasColors());
}

void tst_QGLDisplayList::appendColor()
{
    QGLDisplayListTest list;
    list.newSection();

    QColor4b color(32, 64, 128, 255);
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    list.appendColor(testVertex, color);
    QVERIFY(list.vertexArray().count() == 1);
    QVERIFY(list.vertexArray().at(0) == testVertex);
    QVERIFY(list.colorArray().count() == 1);
    QVERIFY(list.colorArray().at(0) == color);

    QVERIFY(list.hasColors());
    QVERIFY(!list.hasTexCoords());
    QVERIFY(!list.hasNormals());

    list.newSection();

    QVector2D testTexCoord(0.0f, 0.0f);
    list.appendColor(testVertex, color, testTexCoord);
    QVERIFY(list.vertexArray().count() == 2);
    QVERIFY(list.vertexArray().at(1) == testVertex);
    QVERIFY(list.colorArray().count() == 2);
    QVERIFY(list.colorArray().at(1) == color);
    QVERIFY(list.texCoordArray().count() == 2);
    QVERIFY(list.texCoordArray().at(1) == testTexCoord);

    QVERIFY(list.hasColors());
    QVERIFY(list.hasTexCoords());
    QVERIFY(!list.hasNormals());
}

void tst_QGLDisplayList::accessors()
{
    QGLDisplayListTest list;
    list.newSection();

    QVector3D vertex;
    QVector3D vertex2(1.0f, 1.0f, 1.0f);
    QVector2D textureCoord;
    QVector2D textureCoord2(1.0f, 1.0f);
    QColor4b color;
    QColor4b color2(16, 32, 64, 128);
    QVector3D normal;
    QVector3D normal2(1.0f, 0.0f, 0.0f);

    QVERIFY(list.count() == 0);
    QVERIFY(!list.hasColors());
    QVERIFY(!list.hasNormals());
    QVERIFY(!list.hasTexCoords());
    QVERIFY(list.indexOf(vertex) == -1);

    list.append(vertex);
    QVERIFY(list.count() == 1);
    QVERIFY(list.indexOf(vertex) == 0);
    QVERIFY(list.indexOf(vertex2) == -1);

    list.setVertex(0, vertex2);
    QVERIFY(list.count() == 1);
    QVERIFY(list.indexOf(vertex2) == 0);
    QVERIFY(list.indexOf(vertex) == -1);

    list.setNormal(0, normal);
    QVERIFY(list.normalArray().count() == 1);
    QVERIFY(list.normalArray().at(0) == normal);
    QVERIFY(list.hasNormals());

    list.setTexCoord(0, textureCoord);
    QVERIFY(list.texCoordArray().count() == 1);
    QVERIFY(list.texCoordArray().at(0) == textureCoord);
    QVERIFY(list.hasTexCoords());

    list.setColor(0, color);
    QVERIFY(list.colorArray().count() == 1);
    QVERIFY(list.colorArray().at(0) == color);
    QVERIFY(list.hasColors());
}

void tst_QGLDisplayList::loadArrays()
{
}

void tst_QGLDisplayList::finalize()
{
}

QTEST_APPLESS_MAIN(tst_QGLDisplayList)

#include "tst_qgldisplaylist.moc"
