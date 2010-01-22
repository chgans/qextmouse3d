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
    void copy();
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
    QCOMPARE(data.commonNormal(), QVector3D());

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
    qDebug() << "about to leave createDefault";
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

QTEST_APPLESS_MAIN(tst_QGeometryData)

#include "tst_qgeometrydata.moc"
