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
#include "qglgeometry.h"

class tst_QGLGeometry : public QObject
{
    Q_OBJECT
public:
    tst_QGLGeometry() {}
    ~tst_QGLGeometry() {}

private slots:
    void create();
    void accessors();
};

void tst_QGLGeometry::create()
{
    QGLGeometry geom;
    QCOMPARE(geom.drawingMode(), QGL::NoDrawingMode);
    QVERIFY(geom.vertexArray().isEmpty());
    QVERIFY(geom.indexArray().isEmpty());
    QCOMPARE(geom.bufferThreshold(), 32);
    QVERIFY(geom.boundingBox().isNull());
}

void tst_QGLGeometry::accessors()
{
    QGLVertexArray array;
    QGLGeometry geom;
    geom.setVertexArray(array);
    QCOMPARE(geom.vertexArray(), array);
    array << QVector3D(1.0, 2.0, 3.0);
    QVERIFY(geom.vertexArray() != array);
    geom.setVertexArray(array);
    QCOMPARE(geom.vertexArray(), array);
    QGLIndexArray indexes;
    indexes.append(1, 2, 3, 4);
    geom.setIndexArray(indexes);
    QCOMPARE(geom.indexArray(), indexes);
    QBox3D box(QVector3D(), QVector3D(3.0, 4.0, 5.0));
    geom.setBoundingBox(box);
    QCOMPARE(geom.boundingBox(), box);
    geom.setBufferThreshold(0);
    QCOMPARE(geom.bufferThreshold(), 0);
    geom.setDrawingMode(QGL::Lines);
}

QTEST_APPLESS_MAIN(tst_QGLGeometry)

#include "tst_qglgeometry.moc"
