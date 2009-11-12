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
#include "qglvertexdescription.h"

class tst_QGLVertexDescription : public QObject
{
    Q_OBJECT
public:
    tst_QGLVertexDescription() {}
    ~tst_QGLVertexDescription() {}

private slots:
    void create();
    void addField();
};

void tst_QGLVertexDescription::create()
{
    QGLVertexDescription desc;
    QCOMPARE(desc.stride(), 0);
    QCOMPARE(desc.fieldCount(), 0);

    desc.addField(QGL::Normal, 3);
    QCOMPARE(desc.stride(), 3);
    QCOMPARE(desc.fieldCount(), 1);
    QVERIFY(desc.fieldAttribute(0) == QGL::Normal);
    QCOMPARE(desc.fieldSize(0), 3);
    QCOMPARE(desc.fieldOffset(0), 0);
    QCOMPARE(desc.indexOf(QGL::Normal), 0);
    QCOMPARE(desc.indexOf(QGL::Position), -1);

    // Out of range field indexes.
    QVERIFY(desc.fieldAttribute(1) == QGL::Position);
    QCOMPARE(desc.fieldSize(1), 0);
    QCOMPARE(desc.fieldOffset(1), 0);
    QVERIFY(desc.fieldAttribute(-1) == QGL::Position);
    QCOMPARE(desc.fieldSize(-1), 0);
    QCOMPARE(desc.fieldOffset(-1), 0);
}

void tst_QGLVertexDescription::addField()
{
    QGLVertexDescription desc1;
    desc1.addField(QGL::Normal, 3);
    QCOMPARE(desc1.stride(), 3);
    QCOMPARE(desc1.fieldCount(), 1);
    QVERIFY(desc1.fieldAttribute(0) == QGL::Normal);
    QVERIFY(desc1.fieldSize(0) == 3);
    QVERIFY(desc1.fieldOffset(0) == 0);

    QGLVertexDescription desc2;
    desc2.addField(QGL::Normal, 3);
    desc2.addField(QGL::TextureCoord0, 2);
    QCOMPARE(desc2.stride(), 5);
    QCOMPARE(desc2.fieldCount(), 2);
    QVERIFY(desc2.fieldAttribute(0) == QGL::Normal);
    QVERIFY(desc2.fieldSize(0) == 3);
    QVERIFY(desc2.fieldOffset(0) == 0);
    QVERIFY(desc2.fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(desc2.fieldSize(1) == 2);
    QVERIFY(desc2.fieldOffset(1) == 3);

    QGLVertexDescription desc3;
    desc3.addField(QGL::Normal, 3);
    desc3.addField(QGL::TextureCoord0, 2);
    desc3.addField(QGL::Position, 1);
    QCOMPARE(desc3.stride(), 6);
    QCOMPARE(desc3.fieldCount(), 3);
    QVERIFY(desc3.fieldAttribute(0) == QGL::Normal);
    QVERIFY(desc3.fieldSize(0) == 3);
    QVERIFY(desc3.fieldOffset(0) == 0);
    QVERIFY(desc3.fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(desc3.fieldSize(1) == 2);
    QVERIFY(desc3.fieldOffset(1) == 3);
    QVERIFY(desc3.fieldAttribute(2) == QGL::Position);
    QVERIFY(desc3.fieldSize(2) == 1);
    QVERIFY(desc3.fieldOffset(2) == 5);

    QGLVertexDescription desc4;
    desc4.addField(QGL::Normal, 3);
    desc4.addField(QGL::TextureCoord0, 2);
    desc4.addField(QGL::Position, 1);
    desc4.addField(QGL::Color, 4);
    QCOMPARE(desc4.stride(), 10);
    QCOMPARE(desc4.fieldCount(), 4);
    QVERIFY(desc4.fieldAttribute(0) == QGL::Normal);
    QVERIFY(desc4.fieldSize(0) == 3);
    QVERIFY(desc4.fieldOffset(0) == 0);
    QVERIFY(desc4.fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(desc4.fieldSize(1) == 2);
    QVERIFY(desc4.fieldOffset(1) == 3);
    QVERIFY(desc4.fieldAttribute(2) == QGL::Position);
    QVERIFY(desc4.fieldSize(2) == 1);
    QVERIFY(desc4.fieldOffset(2) == 5);
    QVERIFY(desc4.fieldAttribute(3) == QGL::Color);
    QVERIFY(desc4.fieldSize(3) == 4);
    QVERIFY(desc4.fieldOffset(3) == 6);

    desc1.addField(QGL::TextureCoord0, 2);
    QCOMPARE(desc1.stride(), 5);
    QCOMPARE(desc1.fieldCount(), 2);
    QVERIFY(desc1.fieldAttribute(0) == QGL::Normal);
    QVERIFY(desc1.fieldSize(0) == 3);
    QVERIFY(desc1.fieldOffset(0) == 0);
    QVERIFY(desc1.fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(desc1.fieldSize(1) == 2);
    QVERIFY(desc1.fieldOffset(1) == 3);

    desc1.addField(QGL::Position, 1);
    QCOMPARE(desc1.stride(), 6);
    QCOMPARE(desc1.fieldCount(), 3);
    QVERIFY(desc1.fieldAttribute(0) == QGL::Normal);
    QVERIFY(desc1.fieldSize(0) == 3);
    QVERIFY(desc1.fieldOffset(0) == 0);
    QVERIFY(desc1.fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(desc1.fieldSize(1) == 2);
    QVERIFY(desc1.fieldOffset(1) == 3);
    QVERIFY(desc1.fieldAttribute(2) == QGL::Position);
    QVERIFY(desc1.fieldSize(2) == 1);
    QVERIFY(desc1.fieldOffset(2) == 5);

    desc1.addField(QGL::Color, 4);
    QCOMPARE(desc1.stride(), 10);
    QCOMPARE(desc1.fieldCount(), 4);
    QVERIFY(desc1.fieldAttribute(0) == QGL::Normal);
    QVERIFY(desc1.fieldSize(0) == 3);
    QVERIFY(desc1.fieldOffset(0) == 0);
    QVERIFY(desc1.fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(desc1.fieldSize(1) == 2);
    QVERIFY(desc1.fieldOffset(1) == 3);
    QVERIFY(desc1.fieldAttribute(2) == QGL::Position);
    QVERIFY(desc1.fieldSize(2) == 1);
    QVERIFY(desc1.fieldOffset(2) == 5);
    QVERIFY(desc1.fieldAttribute(3) == QGL::Color);
    QVERIFY(desc1.fieldSize(3) == 4);
    QVERIFY(desc1.fieldOffset(3) == 6);

    QCOMPARE(desc1.indexOf(QGL::Normal), 0);
    QCOMPARE(desc1.indexOf(QGL::TextureCoord0), 1);
    QCOMPARE(desc1.indexOf(QGL::Position), 2);
    QCOMPARE(desc1.indexOf(QGL::Color), 3);
    QCOMPARE(desc1.indexOf(QGL::CustomVertex0), -1);
}

QTEST_APPLESS_MAIN(tst_QGLVertexDescription)

#include "tst_qglvertexdescription.moc"
