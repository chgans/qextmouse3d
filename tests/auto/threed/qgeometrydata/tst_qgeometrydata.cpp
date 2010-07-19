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
#include "qgeometrydata.h"
#include "qtest_helpers_p.h"

#include <QtCore/qdebug.h>

class tst_QGeometryData : public QObject
{
    Q_OBJECT
public:
    tst_QGeometryData() {}
    ~tst_QGeometryData() {}

private slots:
    void createDefault();
    void appendVertex();
    void appendNormal();
    void appendVertexNormal();
    void copy();
    void interleaveWith();
};

void tst_QGeometryData::createDefault()
{
    QGeometryData data;
    QCOMPARE(data.count(), 0);
    QCOMPARE(data.attributes().count(), 0);
    QCOMPARE(data.fields(), quint32(0));
    QCOMPARE(data.vertices().count(), 0);
    data.normalizeNormals();
    QCOMPARE(data.boundingBox(), QBox3D());

    // copy constructor on initialization - null default
    QGeometryData other = data;
    QCOMPARE(other.count(), 0);
    QCOMPARE(other.attributes().count(), 0);
    QCOMPARE(other.fields(), quint32(0));

    // copy constructor on default
    QGeometryData copy(data);
    QCOMPARE(copy.count(), 0);
    QCOMPARE(copy.attributes().count(), 0);
    QCOMPARE(copy.fields(), quint32(0));
}

void tst_QGeometryData::appendVertex()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);
    {
        QGeometryData data;
        data.appendVertex(a);
        QCOMPARE(data.count(), 1);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 1);
        QCOMPARE(data.vertices().at(0), a);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b);
        QCOMPARE(data.count(), 2);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 2);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c);
        QCOMPARE(data.count(), 3);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 3);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        QCOMPARE(data.count(), 4);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 4);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
        QCOMPARE(data.vertices().at(3), d);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        data.appendVertex(a, b, c, d);
        data.appendVertex(a);
        QCOMPARE(data.count(), 9);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(data.vertices().count(), 9);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(5), b);
        QCOMPARE(data.vertices().at(8), a);
    }
}

void tst_QGeometryData::appendNormal()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);
    {
        QGeometryData data;
        data.appendNormal(a);
        QCOMPARE(data.count(), 1);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 1);
        QCOMPARE(data.normals().at(0), a);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b);
        QCOMPARE(data.count(), 2);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 2);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b, c);
        QCOMPARE(data.count(), 3);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 3);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
        QCOMPARE(data.normals().at(2), c);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b, c, d);
        QCOMPARE(data.count(), 4);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 4);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
        QCOMPARE(data.normals().at(2), c);
        QCOMPARE(data.normals().at(3), d);
    }
    {
        QGeometryData data;
        data.appendNormal(a, b, c, d);
        data.appendNormal(a, b, c, d);
        data.appendNormal(a);
        QCOMPARE(data.count(), 9);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.normals().count(), 9);
        QCOMPARE(data.normals().at(0), a);
        QCOMPARE(data.normals().at(1), b);
        QCOMPARE(data.normals().at(5), b);
        QCOMPARE(data.normals().at(8), a);
    }
}

void tst_QGeometryData::appendVertexNormal()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);
    QVector3D an(5.1, 5.2, 5.3);
    QVector3D bn(6.1, 6.2, 6.3);
    QVector3D cn(7.1, 7.2, 7.3);
    QVector3D dn(8.1, 8.2, 8.3);
    {
        QGeometryData data;
        data.appendVertex(a);
        data.appendNormal(an);
        QCOMPARE(data.count(), 1);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 1);
        QCOMPARE(data.vertices().at(0), a);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b);
        data.appendNormal(an, bn);
        QCOMPARE(data.count(), 2);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 2);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertex(1), b);
        QCOMPARE(data.normals().count(), 2);
        QCOMPARE(data.normal(0), an);
        QCOMPARE(data.normals().at(1), bn);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c);
        data.appendNormal(an, bn, cn);
        QCOMPARE(data.count(), 3);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 3);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
        QCOMPARE(data.normals().count(), 3);
        QCOMPARE(data.normal(0), an);
        QCOMPARE(data.normals().at(1), bn);
        QCOMPARE(data.normal(2), cn);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        data.appendNormal(an, bn, cn, dn);
        QCOMPARE(data.count(), 4);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 4);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(2), c);
        QCOMPARE(data.vertices().at(3), d);
        QCOMPARE(data.normals().count(), 4);
        QCOMPARE(data.normals().at(0), an);
        QCOMPARE(data.normals().at(1), bn);
        QCOMPARE(data.normals().at(2), cn);
        QCOMPARE(data.normals().at(3), dn);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        data.appendNormal(an, bn, cn, dn);
        data.appendVertex(a, b, c, d);
        data.appendNormal(an, bn, cn, dn);
        data.appendVertex(a);
        data.appendNormal(an);
        QCOMPARE(data.count(), 9);
        QCOMPARE(data.fields(), QGL::fieldMask(QGL::Position) | QGL::fieldMask(QGL::Normal));
        QCOMPARE(data.vertices().count(), 9);
        QCOMPARE(data.vertices().at(0), a);
        QCOMPARE(data.vertices().at(1), b);
        QCOMPARE(data.vertices().at(5), b);
        QCOMPARE(data.vertices().at(8), a);
        QCOMPARE(data.normals().count(), 9);
        QCOMPARE(data.normals().at(0), an);
        QCOMPARE(data.normals().at(1), bn);
        QCOMPARE(data.normals().at(5), bn);
        QCOMPARE(data.normals().at(8), an);
    }
}


QVector3D avec(99.1, 99.2, 99.3);

static QGeometryData aFunc(const QGeometryData& g)   // not a copy but a ref
{
    QGeometryData d = g;  // copy constructor
    d.appendVertex(avec);
    return d;   // assingment operator
}

void tst_QGeometryData::copy()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);
    {
        QGeometryData data;
        QCOMPARE(data.count(), 0);
        QCOMPARE(data.fields(), (quint32)0);
        QGeometryData other;
        other.appendVertex(QVector3D());
        QCOMPARE(other.count(), 1);
        QCOMPARE(other.fields(), QGL::fieldMask(QGL::Position));
        other = aFunc(data);  // assignment op - throw away previous d
        QCOMPARE(other.count(), 1);
        QCOMPARE(other.fields(), QGL::fieldMask(QGL::Position));
        QVector3D res = other.vertices().at(0);
        QCOMPARE(res, avec);
    }
    {
        QGeometryData data;
        data.appendVertex(a, b, c, d);
        QGeometryData other;
        QCOMPARE(other.count(), 0);
        QCOMPARE(other.fields(), (quint32)0);
        other = aFunc(data);  // assignment operator
        other.appendVertex(a);
        QCOMPARE(other.count(), 6);
        QCOMPARE(other.fields(), QGL::fieldMask(QGL::Position));
        QCOMPARE(other.vertices().count(), 6);
        QCOMPARE(other.vertices().at(0), a);
        QCOMPARE(other.vertices().at(1), b);
        QCOMPARE(other.vertices().at(4), avec);
        QCOMPARE(other.vertices().at(5), a);
    }
}

void tst_QGeometryData::interleaveWith()
{
    QVector3D a(1.1, 1.2, 1.3);
    QVector3D b(2.1, 2.2, 2.3);
    QVector3D c(3.1, 3.2, 3.3);
    QVector3D d(4.1, 4.2, 4.3);
    QVector3D vx(0.7071, 0.7071, 0.0);
    QVector2D at(0.11, 0.12);
    QVector2D bt(0.21, 0.22);
    QVector2D ct(0.31, 0.32);
    QVector2D dt(0.41, 0.42);
    QVector2D tx(1.0, 1.0);

    QGeometryData data;
    data.appendVertex(a, b, c, d);
    data.appendTexCoord(at, bt, ct, dt);
    QGeometryData dat2;

    // count is the smaller of the two - nothing in this null case
    // also make sure the argument doesnt somehow change - its a const
    // so it shouldn't...
    dat2.interleaveWith(data);
    QCOMPARE(data.count(), 4);
    QCOMPARE(data.vertex(0), a);
    QCOMPARE(dat2.count(), 0);
    QCOMPARE(dat2.count(QGL::Position), 0);
    QCOMPARE(dat2.fields(), quint32(0));

    // dat2 is smaller and has less fields
    dat2.appendVertex(a + vx, b + vx);
    dat2.interleaveWith(data);
    QCOMPARE(data.count(), 4);
    QCOMPARE(data.vertex(0), a);
    QCOMPARE(dat2.count(), 4);
    QCOMPARE(dat2.count(QGL::Position), 4);
    QCOMPARE(dat2.count(QGL::TextureCoord0), 0);
    QCOMPARE(dat2.fields(), QGL::fieldMask(QGL::Position));
    QCOMPARE(dat2.vertex(0), a + vx);
    QCOMPARE(dat2.vertex(1), a);
    QCOMPARE(dat2.vertex(2), b + vx);
    QCOMPARE(dat2.vertex(3), b);

    // full zip with both sides have 4 verts & textures
    dat2.clear();
    for (int i = 0; i < data.count(); ++i)
    {
        dat2.appendVertex(data.vertex(i) + vx);
        dat2.appendTexCoord(data.texCoord(i) + tx);
    }
    dat2.interleaveWith(data);
    QCOMPARE(dat2.count(), 8);
    QCOMPARE(dat2.count(QGL::Position), 8);
    QCOMPARE(dat2.count(QGL::TextureCoord0), 8);
    QCOMPARE(dat2.fields(), QGL::fieldMask(QGL::Position) |
             QGL::fieldMask(QGL::TextureCoord0));
    QCOMPARE(dat2.vertex(0), a + vx);
    QCOMPARE(dat2.vertex(1), a);
    QCOMPARE(dat2.vertex(4), c + vx);
    QCOMPARE(dat2.vertex(7), d);
    QCOMPARE(dat2.texCoord(0), at + tx);
    QCOMPARE(dat2.texCoord(3), bt);
    QCOMPARE(dat2.texCoord(7), dt);
}

QTEST_APPLESS_MAIN(tst_QGeometryData)

#include "tst_qgeometrydata.moc"
