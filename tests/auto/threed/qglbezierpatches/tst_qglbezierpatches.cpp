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
#include "qglbezierpatches.h"
#include "qglbuilder.h"
#include "qglteapot.h"

class tst_QGLBezierPatches : public QObject
{
    Q_OBJECT
public:
    tst_QGLBezierPatches() {}
    ~tst_QGLBezierPatches() {}

private slots:
    void defaultValue();
    void modify();
    void teapot();
    void build();
};

void tst_QGLBezierPatches::defaultValue()
{
    QGLBezierPatches patches;
    QVERIFY(patches.positions().isEmpty());
    QVERIFY(patches.textureCoords().isEmpty());
    QCOMPARE(patches.subdivisionDepth(), 4);
    QVERIFY(!patches.compactSubdivision());
}

void tst_QGLBezierPatches::modify()
{
    QVector3DArray positions;
    for (int index = 0; index < 32; ++index)
        positions.append(index, index + 1, index + 2);

    QVector2DArray texCoords;
    texCoords.append(0.0f, 0.5f);
    texCoords.append(0.5f, 1.0f);
    texCoords.append(0.5f, 0.0f);
    texCoords.append(1.0f, 0.5f);

    QGLBezierPatches patches;
    patches.setPositions(positions);
    patches.setTextureCoords(texCoords);
    patches.setSubdivisionDepth(23);
    patches.setCompactSubdivision(true);

    QCOMPARE(patches.positions().size(), positions.size());
    QVERIFY(patches.positions() == positions);
    QCOMPARE(patches.textureCoords().size(), texCoords.size());
    QVERIFY(patches.textureCoords() == texCoords);
    QCOMPARE(patches.subdivisionDepth(), 23);
    QVERIFY(patches.compactSubdivision());

    QGLBezierPatches patches2(patches);
    QCOMPARE(patches2.positions().size(), positions.size());
    QVERIFY(patches2.positions() == positions);
    QCOMPARE(patches2.textureCoords().size(), texCoords.size());
    QVERIFY(patches2.textureCoords() == texCoords);
    QCOMPARE(patches2.subdivisionDepth(), 23);
    QVERIFY(patches2.compactSubdivision());

    patches2.setPositions(QVector3DArray());
    patches2.setTextureCoords(QVector2DArray());
    patches2.setSubdivisionDepth(18);
    patches2.setCompactSubdivision(false);

    QCOMPARE(patches.positions().size(), positions.size());
    QVERIFY(patches.positions() == positions);
    QCOMPARE(patches.textureCoords().size(), texCoords.size());
    QVERIFY(patches.textureCoords() == texCoords);
    QCOMPARE(patches.subdivisionDepth(), 23);
    QVERIFY(patches.compactSubdivision());

    QVERIFY(patches2.positions().isEmpty());
    QVERIFY(patches2.textureCoords().isEmpty());
    QCOMPARE(patches2.subdivisionDepth(), 18);
    QVERIFY(!patches2.compactSubdivision());

    QGLBezierPatches patches3;
    patches3 = patches;

    QCOMPARE(patches3.positions().size(), positions.size());
    QVERIFY(patches3.positions() == positions);
    QCOMPARE(patches3.textureCoords().size(), texCoords.size());
    QVERIFY(patches3.textureCoords() == texCoords);
    QCOMPARE(patches3.subdivisionDepth(), 23);
    QVERIFY(patches3.compactSubdivision());

    patches3.setPositions(QVector3DArray());
    patches3.setTextureCoords(QVector2DArray());
    patches3.setSubdivisionDepth(18);
    patches3.setCompactSubdivision(false);

    QCOMPARE(patches.positions().size(), positions.size());
    QVERIFY(patches.positions() == positions);
    QCOMPARE(patches.textureCoords().size(), texCoords.size());
    QVERIFY(patches.textureCoords() == texCoords);
    QCOMPARE(patches.subdivisionDepth(), 23);
    QVERIFY(patches.compactSubdivision());

    QVERIFY(patches3.positions().isEmpty());
    QVERIFY(patches3.textureCoords().isEmpty());
    QCOMPARE(patches3.subdivisionDepth(), 18);
    QVERIFY(!patches3.compactSubdivision());

    patches = QGLBezierPatches();
    QVERIFY(patches.positions().isEmpty());
    QVERIFY(patches.textureCoords().isEmpty());
    QCOMPARE(patches.subdivisionDepth(), 4);
    QVERIFY(!patches.compactSubdivision());
}

void tst_QGLBezierPatches::teapot()
{
    // Verify that patch data is added by QGLTeapot's constructor,
    // but not what that patch data is.
    QGLTeapot teapot;
    QCOMPARE(teapot.positions().size(), 32 * 16);
    QCOMPARE(teapot.textureCoords().size(), 0);
    QCOMPARE(teapot.subdivisionDepth(), 4);
    QVERIFY(!teapot.compactSubdivision());
}

void tst_QGLBezierPatches::build()
{
    // Call the build routines with different options.  It isn't really
    // easy to determine if the correct triangles were generated.  Leave
    // that up to visual acceptance tests.

    QGLBuilder builder;

    QGLTeapot teapot1;
    builder << teapot1;

    QGLTeapot teapot2;
    teapot2.setCompactSubdivision(true);
    builder << teapot2;

    QGLTeapot teapot3;
    QVector2DArray texCoords;
    for (int index = 0; index < 32; ++index) {
        texCoords.append(0.0f, 0.5f);
        texCoords.append(0.5f, 1.0f);
    }
    teapot3.setTextureCoords(texCoords);
    builder << teapot3;

    delete builder.finalizedSceneNode();
}

QTEST_APPLESS_MAIN(tst_QGLBezierPatches)

#include "tst_qglbezierpatches.moc"
