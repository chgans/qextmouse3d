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
#include "qglsection_p.h"
#include "QGLDisplayList.h"

#include "qtest_helpers_p.h"

class tst_QGLSection : public QObject
{
    Q_OBJECT
public:
    tst_QGLSection() {}
    ~tst_QGLSection() {}

private slots:
    void create();
    void modify();
    void append();
    void updateTexCoord();
    void appendSmooth();
    void appendFaceted();
    void appendTexCoord();
    void appendColor();
    void accessors();
};

class QGLSectionTest : public QGLSection
{
public:
    QGLSectionTest(QGLDisplayList *list, QGL::Smoothing s = QGL::Smooth)
        : QGLSection(list, s) {}

    void appendSmooth(const QLogicalVertex &vertex)
    {
        QGLSection::appendSmooth(vertex);
    }

    void appendFaceted(const QLogicalVertex &vertex)
    {
        QGLSection::appendFaceted(vertex);
    }

    void append(const QLogicalVertex &vertex)
    {
        QGLSection::append(vertex);
    }
};

class TestQGLDisplayList : public QGLDisplayList
{
public:
    QGLSection *currentSection() { return QGLDisplayList::currentSection(); }
    QList<QGLSection*> sections() { return QGLDisplayList::sections(); }
};

void tst_QGLSection::create()
{
    // Test that a newly created object has the correct defaults.
    TestQGLDisplayList list;
    list.newSection();
    QGLSection *section = list.currentSection();
    QVERIFY(section->hasField(QGL::Position));
    QCOMPARE(section->fields(), QGeometryData::fieldMask(QGL::Position));
    QCOMPARE(section->smoothing(), QGL::Smooth);
    QCOMPARE(section->count(), 0);
    QCOMPARE(section->displayList(), &list);
    QCOMPARE(section->boundingBox(), QBox3D());
}

void tst_QGLSection::modify()
{
    TestQGLDisplayList list;
    list.newSection();
    QGLSection *section = list.currentSection();

    QVector3D va(-1.0f, -1.0f, 0.0f);
    QVector3D vb(1.0f, -1.0f, 0.0f);
    QVector3D vc(1.0f, 1.0f, 0.0f);
    QVector3D n(0.0f, 0.0f, 1.0f);
    QGLPrimitive p;
    p.appendVertex(va);
    p.appendVertex(vb);
    p.appendVertex(vc);
    p.setCommonNormal(n);
    list.addTriangle(p);
    QCOMPARE(section->count(), 3);
    list.newSection();
    QGLSection *section2 = list.currentSection();
    list.addTriangle(p);
    QCOMPARE(section2->count(), 3);
}

void tst_QGLSection::append()
{
    TestQGLDisplayList list;
    list.newSection();
    QGLSection *section = list.currentSection();

    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(1.0f, 0.0f, 0.0f);
    QLogicalVertex vx(testVertex, testNormal);
    section->append(vx);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->indices().count(), 1);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->texCoords().count(), 0);
    QCOMPARE(section->colors().count(), 0);
}

void tst_QGLSection::updateTexCoord()
{
    TestQGLDisplayList list;
    QGLSectionTest *section = new QGLSectionTest(&list);

    QCOMPARE(section->hasData(QLogicalVertex::Texture), false);

    // if arg is an InvalidTexCoord nothing happens, returns -1 without even
    // accessing the texture data array, or indexing with the position
    int result = section->updateTexCoord(1234, QLogicalVertex::InvalidTexCoord);
    QCOMPARE(section->hasData(QLogicalVertex::Texture), false);
    QCOMPARE(result, -1);

    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(1.0f, 0.0f, 0.0f);
    QVector2D testTexCoord(0.1f, 0.1f);
    QLogicalVertex vx(testVertex, testNormal);

    // if the previous texture coordinate was invalid, effect is same as
    // setTexCoord() - just sets the value, returns that index.
    section->append(vx);
    section->enableTypes(QLogicalVertex::Texture);
    QCOMPARE(section->texCoords().at(0), QLogicalVertex::InvalidTexCoord);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->vertices().count(), 1);

    result = section->updateTexCoord(0, testTexCoord);
    QCOMPARE(result, 0);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->vertices().count(), 1);

    // if arg is already in that position (must access the texture data array
    // to confirm this) then return -1 without doing anything else
    result = section->updateTexCoord(0, testTexCoord);
    QCOMPARE(result, -1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->vertices().count(), 1);

    // if the previous texture coordinate is neither invalide, nor equal to
    // the new incoming value, then duplicate the vertex to carry the new
    // texture data - create a "seam".
    QVector2D testTexCoord2(0.9f, 0.9f);
    result = section->updateTexCoord(0, testTexCoord2);
    QCOMPARE(result, 1);
    QCOMPARE(section->texCoords().at(1), testTexCoord2);
    QCOMPARE(section->texCoords().count(), 2);
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
}

void tst_QGLSection::appendSmooth()
{
    TestQGLDisplayList list;
    QGLSectionTest *section = new QGLSectionTest(&list);

    // append a vertex - check it appears in the data along with its normal
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->indices().count(), 1);
    QCOMPARE(section->indices().at(0), 0);

    // append a vertex equal to one already appended - check it was coalesced
    QVector3D testNormal2(0.0f, 0.0f, 1.0f);
    QVector3D result = testNormal + testNormal2; // QVector3D(0.0f, 0.0f, 7.543f);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0),testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), result);
    QCOMPARE(section->indices().count(), 2);
    QCOMPARE(section->indices().at(1), 0);

    // append a new  different vertex - check it is not coalesced
    QVector3D testVertex2(-1.234f, -2.345f, -3.456f);
    QVector3D testNormal3(-4.321f, -5.432f, -6.543f);
    section->appendSmooth(QLogicalVertex(testVertex2, testNormal3));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex2);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal3);
    QCOMPARE(section->indices().count(), 3);
    QCOMPARE(section->indices().at(2), 1);

    // append a vertex equal to one already appended, but inside a new section - check its not coalesced
    section = new QGLSectionTest(&list);
    section->appendSmooth(QLogicalVertex(testVertex2, testNormal3));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().last(), testVertex2);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().last(), testNormal3);
    QCOMPARE(section->indices().last(), 0);
    QCOMPARE(section->indices().count(), 1);
}

void tst_QGLSection::appendFaceted()
{
    TestQGLDisplayList list;
    QGLSectionTest *section = new QGLSectionTest(&list);

    // append a vertex - check it appears in the data along with its normal
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    section->appendFaceted(QLogicalVertex(testVertex, testNormal));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->indices().count(), 1);
    QCOMPARE(section->indices().at(0), 0);

    // append a vertex equal to one already appended, but with different normal - check it was NOT coalesced
    QVector3D testNormal2(0.0f, 0.0f, 1.0f);
    section->appendFaceted(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal2);
    QCOMPARE(section->indices().count(), 2);
    QCOMPARE(section->indices().at(1), 1);

    // append a vertex equal to one already appended, but with same normal - check it WAS coalesced
    section->appendFaceted(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal2);
    QCOMPARE(section->indices().count(), 3);
    QCOMPARE(section->indices().at(2), 1);

    // append a vertex equal to one already appended, with same normal, BUT in a new section - check it was NOT coalesced
    section = new QGLSectionTest(&list);
    section->appendFaceted(QLogicalVertex(testVertex, testNormal2));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal2);
    QCOMPARE(section->indices().count(), 1);
    QCOMPARE(section->indices().at(0), 0);
}

void tst_QGLSection::appendTexCoord()
{
    TestQGLDisplayList list;
    QGLSectionTest *section = new QGLSectionTest(&list);

    // note that the tests above do the case of texCoord, InvalidTexCoord

    // append a texture - check it appears in the data at the correct index
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    QVector3D testNormal(4.321f, 5.432f, 6.543f);
    QVector2D testTexCoord(0.0f, 0.0f);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->indices().count(), 1);
    QCOMPARE(section->indices().at(0), 0);

    // append same texture - will coalesce and index the vert
    section->appendSmooth(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->indices().count(), 2);
    QCOMPARE(section->indices().at(1), 0);

    // new vertex created to carry the updated texture coord, even though
    // the normal and vertex are the same and thus the logical vert would
    // otherwise coalesced - the new vert is a copy (same vert and normal)
    QVector2D testTexCoord2(1.0, 1.0);
    section->appendSmooth(QLogicalVertex(testVertex, testNormal, testTexCoord2));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal);
    QCOMPARE(section->texCoords().count(), 2);
    QCOMPARE(section->texCoords().at(1), testTexCoord2);
    QCOMPARE(section->indices().count(), 3);
    QCOMPARE(section->indices().at(2), 1);

    section = new QGLSectionTest(&list);

    // append a faceted vertex with a tex coord check it appears in the data
    // in a new section now, so the same vert and normal wont be coalesced
    section->appendFaceted(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), testNormal);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);
    QCOMPARE(section->indices().count(), 1);
    QCOMPARE(section->indices().at(0), 0);

    // append a vertex & normal equal to one already appended, but with different tex coord
    // check it was NOT coalesced, dup vert created
    section->appendFaceted(QLogicalVertex(testVertex, testNormal, testTexCoord2));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal);
    QCOMPARE(section->texCoords().count(), 2);
    QCOMPARE(section->texCoords().at(1), testTexCoord2);
    QCOMPARE(section->indices().count(), 2);
    QCOMPARE(section->indices().at(1), 1);

    // append a vertex equal to first one appended above, with same normal, and
    // same texture - check it WAS coalesced to index 0
    section->appendFaceted(QLogicalVertex(testVertex, testNormal, testTexCoord));
    QCOMPARE(section->vertices().count(), 2);
    QCOMPARE(section->vertices().at(1), testVertex);
    QCOMPARE(section->normals().count(), 2);
    QCOMPARE(section->normals().at(1), testNormal);
    QCOMPARE(section->texCoords().count(), 2);
    QCOMPARE(section->texCoords().at(1), testTexCoord2);
    QCOMPARE(section->indices().count(), 3);
    QCOMPARE(section->indices().at(2), 0);

    QVERIFY(section->hasData(QLogicalVertex::Normal));
    QVERIFY(section->hasData(QLogicalVertex::Texture));
    QVERIFY(!section->hasData(QLogicalVertex::Color));
}

void tst_QGLSection::appendColor()
{
    TestQGLDisplayList list;
    QGLSectionTest *section = new QGLSectionTest(&list);

    QColor4b color(32, 64, 128, 255);
    QVector3D testVertex(1.234f, 2.345f, 3.456f);
    section->append(QLogicalVertex(testVertex, color));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->colors().count(), 1);
    QCOMPARE(section->colors().at(0), color);

    QVERIFY(section->hasData(QLogicalVertex::Color));
    QVERIFY(!section->hasData(QLogicalVertex::Texture));
    QVERIFY(!section->hasData(QLogicalVertex::Normal));

    section = new QGLSectionTest(&list);

    QVector2D testTexCoord(0.0f, 0.0f);
    section->append(QLogicalVertex(testVertex, color, QVector3D(), testTexCoord));
    QCOMPARE(section->vertices().count(), 1);
    QCOMPARE(section->vertices().at(0), testVertex);
    QCOMPARE(section->colors().count(), 1);
    QCOMPARE(section->colors().at(0), color);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), testTexCoord);

    QVERIFY(section->hasData(QLogicalVertex::Color));
    QVERIFY(section->hasData(QLogicalVertex::Texture));
    QVERIFY(!section->hasData(QLogicalVertex::Normal));
}

void tst_QGLSection::accessors()
{
    TestQGLDisplayList list;
    QGLSectionTest *section = new QGLSectionTest(&list);

    QVector3D vertex;
    QVector3D vertex2(1.0f, 1.0f, 1.0f);
    QVector2D textureCoord;
    QVector2D textureCoord2(1.0f, 1.0f);
    QColor4b color;
    QColor4b color2(16, 32, 64, 128);
    QVector3D normal(0.0f, 1.0f, 0.0f);
    QVector3D normal2(1.0f, 0.0f, 0.0f);
    QLogicalVertex lv(vertex, color, normal, textureCoord);
    QLogicalVertex lv2(vertex2, color2, normal2, textureCoord2);

    QCOMPARE(section->count(), 0);
    QVERIFY(!section->hasData(QLogicalVertex::Color));
    QVERIFY(!section->hasData(QLogicalVertex::Texture));
    QVERIFY(!section->hasData(QLogicalVertex::Normal));

    section->append(lv);
    QCOMPARE(section->count(), 1);

    section->setVertex(0, vertex2);
    QCOMPARE(section->count(), 1);

    section->setNormal(0, normal2);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), normal2);
    QVERIFY(section->hasData(QLogicalVertex::Normal));

    section->setTexCoord(0, textureCoord2);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), textureCoord2);
    QVERIFY(section->hasData(QLogicalVertex::Texture));

    section->setColor(0, color2);
    QCOMPARE(section->colors().count(), 1);
    QCOMPARE(section->colors().at(0), color2);
    QVERIFY(section->hasData(QLogicalVertex::Color));

    section->setNormal(0, normal);
    QCOMPARE(section->normals().count(), 1);
    QCOMPARE(section->normals().at(0), normal);
    QVERIFY(section->hasData(QLogicalVertex::Normal));

    section->setTexCoord(0, textureCoord);
    QCOMPARE(section->texCoords().count(), 1);
    QCOMPARE(section->texCoords().at(0), textureCoord);
    QVERIFY(section->hasData(QLogicalVertex::Texture));

    section->setColor(0, color);
    QCOMPARE(section->colors().count(), 1);
    QCOMPARE(section->colors().at(0), color);
    QVERIFY(section->hasData(QLogicalVertex::Color));
}

QTEST_APPLESS_MAIN(tst_QGLSection)

#include "tst_qglsection.moc"
