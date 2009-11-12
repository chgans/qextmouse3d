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
#include "qglvertexarray.h"

class tst_QGLVertexArray : public QObject
{
    Q_OBJECT
public:
    tst_QGLVertexArray() {}
    ~tst_QGLVertexArray() {}

private slots:
    void create();
    void addField();
    void copy();
    void append1D();
    void append2D();
    void append3D();
    void append4D();
    void appendLots();
    void setFloatAt();
    void setVector2DAt();
    void setVector3DAt();
    void setVector4DAt();
    void extractAndInterleave();
    void operatorEquals();
};

void tst_QGLVertexArray::create()
{
    QGLVertexArray array;
    QCOMPARE(array.stride(), 0);
    QCOMPARE(array.vertexCount(), 0);
    QCOMPARE(array.componentCount(), 0);
    QCOMPARE(array.fieldCount(), 0);
    QVERIFY(array.isEmpty());
    QVERIFY(array.data() == 0);
    QVERIFY(array.constData() == 0);

    array.reserve(100);
    QCOMPARE(array.stride(), 0);
    QCOMPARE(array.vertexCount(), 0);
    QCOMPARE(array.componentCount(), 0);
    QCOMPARE(array.fieldCount(), 0);
    QVERIFY(array.isEmpty());
    QVERIFY(array.data() == 0);
    QVERIFY(array.constData() == 0);

    array.append(1.0f, 2.0f, 3.0f);
    QCOMPARE(array.stride(), 0);
    QCOMPARE(array.vertexCount(), 3);
    QCOMPARE(array.componentCount(), 3);
    QCOMPARE(array.fieldCount(), 0);
    QVERIFY(!array.isEmpty());

    array.addField(QGL::Normal, 3);
    QCOMPARE(array.stride(), 3);
    QCOMPARE(array.vertexCount(), 1);
    QCOMPARE(array.componentCount(), 3);
    QCOMPARE(array.fieldCount(), 1);
    QVERIFY(!array.isEmpty());
    QVERIFY(array.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array.fields().fieldSize(0) == 3);
    QVERIFY(array.fields().fieldOffset(0) == 0);
    QVERIFY(array.attributeValue(1).isNull());
    QVERIFY(!array.attributeValue(0).isNull());
    QVERIFY(array.attributeValue(-1).isNull());
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array.attributeValue(QGL::Normal).size() == 3);
    QVERIFY(array.attributeValue(QGL::Position).isNull());
}

void tst_QGLVertexArray::addField()
{
    QGLVertexArray array1(QGL::Normal, 3);
    QCOMPARE(array1.stride(), 3);
    QCOMPARE(array1.vertexCount(), 0);
    QCOMPARE(array1.componentCount(), 0);
    QCOMPARE(array1.fieldCount(), 1);
    QVERIFY(array1.isEmpty());
    QVERIFY(array1.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array1.fields().fieldSize(0) == 3);
    QVERIFY(array1.fields().fieldOffset(0) == 0);

    QGLVertexArray array2(QGL::Normal, 3, QGL::TextureCoord0, 2);
    QCOMPARE(array2.stride(), 5);
    QCOMPARE(array2.vertexCount(), 0);
    QCOMPARE(array2.componentCount(), 0);
    QCOMPARE(array2.fieldCount(), 2);
    QVERIFY(array2.isEmpty());
    QVERIFY(array2.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array2.fields().fieldSize(0) == 3);
    QVERIFY(array2.fields().fieldOffset(0) == 0);
    QVERIFY(array2.fields().fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(array2.fields().fieldSize(1) == 2);
    QVERIFY(array2.fields().fieldOffset(1) == 3);

    QGLVertexArray array3(QGL::Normal, 3, QGL::TextureCoord0, 2,
                          QGL::Position, 1);
    QCOMPARE(array3.stride(), 6);
    QCOMPARE(array3.vertexCount(), 0);
    QCOMPARE(array3.componentCount(), 0);
    QCOMPARE(array3.fieldCount(), 3);
    QVERIFY(array3.isEmpty());
    QVERIFY(array3.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array3.fields().fieldSize(0) == 3);
    QVERIFY(array3.fields().fieldOffset(0) == 0);
    QVERIFY(array3.fields().fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(array3.fields().fieldSize(1) == 2);
    QVERIFY(array3.fields().fieldOffset(1) == 3);
    QVERIFY(array3.fields().fieldAttribute(2) == QGL::Position);
    QVERIFY(array3.fields().fieldSize(2) == 1);
    QVERIFY(array3.fields().fieldOffset(2) == 5);

    QGLVertexArray array4(QGL::Normal, 3, QGL::TextureCoord0, 2,
                          QGL::Position, 1, QGL::Color, 4);
    QCOMPARE(array4.stride(), 10);
    QCOMPARE(array4.vertexCount(), 0);
    QCOMPARE(array4.componentCount(), 0);
    QCOMPARE(array4.fieldCount(), 4);
    QVERIFY(array4.isEmpty());
    QVERIFY(array4.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array4.fields().fieldSize(0) == 3);
    QVERIFY(array4.fields().fieldOffset(0) == 0);
    QVERIFY(array4.fields().fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(array4.fields().fieldSize(1) == 2);
    QVERIFY(array4.fields().fieldOffset(1) == 3);
    QVERIFY(array4.fields().fieldAttribute(2) == QGL::Position);
    QVERIFY(array4.fields().fieldSize(2) == 1);
    QVERIFY(array4.fields().fieldOffset(2) == 5);
    QVERIFY(array4.fields().fieldAttribute(3) == QGL::Color);
    QVERIFY(array4.fields().fieldSize(3) == 4);
    QVERIFY(array4.fields().fieldOffset(3) == 6);

    array1.addField(QGL::TextureCoord0, 2);
    QCOMPARE(array1.stride(), 5);
    QCOMPARE(array1.vertexCount(), 0);
    QCOMPARE(array1.componentCount(), 0);
    QCOMPARE(array1.fieldCount(), 2);
    QVERIFY(array1.isEmpty());
    QVERIFY(array1.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array1.fields().fieldSize(0) == 3);
    QVERIFY(array1.fields().fieldOffset(0) == 0);
    QVERIFY(array1.fields().fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(array1.fields().fieldSize(1) == 2);
    QVERIFY(array1.fields().fieldOffset(1) == 3);

    array1.addField(QGL::Position, 1);
    QCOMPARE(array1.stride(), 6);
    QCOMPARE(array1.vertexCount(), 0);
    QCOMPARE(array1.componentCount(), 0);
    QCOMPARE(array1.fieldCount(), 3);
    QVERIFY(array1.isEmpty());
    QVERIFY(array1.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array1.fields().fieldSize(0) == 3);
    QVERIFY(array1.fields().fieldOffset(0) == 0);
    QVERIFY(array1.fields().fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(array1.fields().fieldSize(1) == 2);
    QVERIFY(array1.fields().fieldOffset(1) == 3);
    QVERIFY(array1.fields().fieldAttribute(2) == QGL::Position);
    QVERIFY(array1.fields().fieldSize(2) == 1);
    QVERIFY(array1.fields().fieldOffset(2) == 5);

    array1.addField(QGL::Color, 4);
    QCOMPARE(array1.stride(), 10);
    QCOMPARE(array1.vertexCount(), 0);
    QCOMPARE(array1.componentCount(), 0);
    QCOMPARE(array1.fieldCount(), 4);
    QVERIFY(array1.isEmpty());
    QVERIFY(array1.fields().fieldAttribute(0) == QGL::Normal);
    QVERIFY(array1.fields().fieldSize(0) == 3);
    QVERIFY(array1.fields().fieldOffset(0) == 0);
    QVERIFY(array1.fields().fieldAttribute(1) == QGL::TextureCoord0);
    QVERIFY(array1.fields().fieldSize(1) == 2);
    QVERIFY(array1.fields().fieldOffset(1) == 3);
    QVERIFY(array1.fields().fieldAttribute(2) == QGL::Position);
    QVERIFY(array1.fields().fieldSize(2) == 1);
    QVERIFY(array1.fields().fieldOffset(2) == 5);
    QVERIFY(array1.fields().fieldAttribute(3) == QGL::Color);
    QVERIFY(array1.fields().fieldSize(3) == 4);
    QVERIFY(array1.fields().fieldOffset(3) == 6);

    QVERIFY(array1.attributeValue(QGL::CustomVertex2).isNull());
}

void tst_QGLVertexArray::copy()
{
    QGLVertexArray array(QGL::Position, 3);
    array.append(1.0f, 2.0f, 3.0f);
    QCOMPARE(array.stride(), 3);
    QCOMPARE(array.vertexCount(), 1);

    // Copy the array and check its contents.
    QGLVertexArray array2;
    array2 = array;
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 1);
    QVERIFY(!array2.isEmpty());
    QVERIFY(array2.data() != 0);
    QVERIFY(array2.constData() != 0);
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));

    // Modify the original and check that the copy is unchanged.
    array.setAt(0, 0, QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(4.0f, 5.0f, 6.0f));
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 1);
    QVERIFY(!array2.isEmpty());
    QVERIFY(array2.data() != 0);
    QVERIFY(array2.constData() != 0);
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));

    // Copy the original back in.
    array2 = array;
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 1);
    QVERIFY(!array2.isEmpty());
    QVERIFY(array2.data() != 0);
    QVERIFY(array2.constData() != 0);
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(4.0f, 5.0f, 6.0f));

    // Copy the array using the constructor form.
    QGLVertexArray array3(array);
    QCOMPARE(array3.stride(), 3);
    QCOMPARE(array3.vertexCount(), 1);
    QVERIFY(!array3.isEmpty());
    QVERIFY(array3.data() != 0);
    QVERIFY(array3.constData() != 0);
    QVERIFY(array3.vector3DAt(0, 0) == QVector3D(4.0f, 5.0f, 6.0f));

    // Modify the original and check that the copy is unchanged.
    array.setAt(0, 0, QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QCOMPARE(array3.stride(), 3);
    QCOMPARE(array3.vertexCount(), 1);
    QVERIFY(!array3.isEmpty());
    QVERIFY(array3.data() != 0);
    QVERIFY(array3.constData() != 0);
    QVERIFY(array3.vector3DAt(0, 0) == QVector3D(4.0f, 5.0f, 6.0f));
}

void tst_QGLVertexArray::append1D()
{
    QGLVertexArray array(QGL::Position, 1);
    array.append(1.0f);
    array.append(2.0f);
    array.append(3.0f);
    QCOMPARE(array.floatAt(-1, 0), (qreal)0.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)2.0f);
    QCOMPARE(array.floatAt(2, 0), (qreal)3.0f);
    QCOMPARE(array.floatAt(3, 0), (qreal)0.0f);
    QCOMPARE(array.stride(), 1);
    QCOMPARE(array.vertexCount(), 3);
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));

    QGLVertexArray array2(QGL::Position, 3);
    array2.append(1.0f);
    array2.append(2.0f);
    array2.append(3.0f);
    array2.append(4.0f, 5.0f, 6.0f);
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 2);
    QCOMPARE(array2.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array2.floatAt(1, 0), (qreal)4.0f);
    QVERIFY(array2.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array2.vector2DAt(1, 0) == QVector2D(4.0f, 5.0f));
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array2.vector3DAt(1, 0) == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(array2.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));

    array2.append(7.0f, 8.0f);
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 2);
    array2.append(9.0f);
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 3);
    QVERIFY(array2.vector3DAt(2, 0) == QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(array2.vector4DAt(1, 0) == QVector4D(4.0f, 5.0f, 6.0f, 7.0f));

    QGLVertexArray array3(QGL::Position, 3, QGL::Normal, 3);
    array3.append(1.0f);
    array3.append(2.0f);
    array3.append(3.0f);
    array3.append(4.0f, 5.0f, 6.0f);
    QCOMPARE(array3.stride(), 6);
    QCOMPARE(array3.vertexCount(), 1);
    QCOMPARE(array3.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array3.floatAt(0, 1), (qreal)4.0f);
    QVERIFY(array3.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array3.vector2DAt(0, 1) == QVector2D(4.0f, 5.0f));
    QVERIFY(array3.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array3.vector3DAt(0, 1) == QVector3D(4.0f, 5.0f, 6.0f));
}

void tst_QGLVertexArray::append2D()
{
    QGLVertexArray array(QGL::Position, 2);
    array.append(1.0f, 2.0f);
    array.append(3.0f, 4.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)3.0f);
    QVERIFY(array.vector2DAt(-1, 0).isNull());
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array.vector2DAt(1, 0) == QVector2D(3.0f, 4.0f));
    QVERIFY(array.vector2DAt(2, 0).isNull());
    QCOMPARE(array.stride(), 2);
    QCOMPARE(array.vertexCount(), 2);
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));

    QGLVertexArray array2(QGL::Position, 3);
    array2.append(1.0f, 2.0f);
    array2.append(3.0f, 4.0f);
    array2.append(5.0f, 6.0f);
    array2.append(7.0f, 8.0f, 9.0f);
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 3);
    QCOMPARE(array2.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array2.floatAt(1, 0), (qreal)4.0f);
    QCOMPARE(array2.floatAt(2, 0), (qreal)7.0f);
    QVERIFY(array2.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array2.vector2DAt(1, 0) == QVector2D(4.0f, 5.0f));
    QVERIFY(array2.vector2DAt(2, 0) == QVector2D(7.0f, 8.0f));
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array2.vector3DAt(1, 0) == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(array2.vector3DAt(2, 0) == QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(array2.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array2.vector4DAt(1, 0) == QVector4D(4.0f, 5.0f, 6.0f, 7.0f));
    QVERIFY(array2.vector4DAt(2, 0).isNull());

    array2.append(10.0f, 11.0f);
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 3);
    array2.append(12.0f);
    QVERIFY(array2.vector4DAt(2, 0) == QVector4D(7.0f, 8.0f, 9.0f, 10.0f));
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 4);
    QVERIFY(array2.vector3DAt(3, 0) == QVector3D(10.0f, 11.0f, 12.0f));

    QGLVertexArray array3(QGL::Position, 2, QGL::TextureCoord0, 2);
    array3.append(1.0f, 2.0f);
    array3.append(3.0f, 4.0f);
    QCOMPARE(array3.stride(), 4);
    QCOMPARE(array3.vertexCount(), 1);
    QCOMPARE(array3.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array3.floatAt(0, 1), (qreal)3.0f);
    QVERIFY(array3.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array3.vector2DAt(0, 1) == QVector2D(3.0f, 4.0f));
    QVERIFY(array3.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array3.vector3DAt(0, 1).isNull());
    QVERIFY(array3.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array3.vector4DAt(0, 1).isNull());
}

void tst_QGLVertexArray::append3D()
{
    QGLVertexArray array(QGL::Position, 3);
    array.append(1.0f, 2.0f, 3.0f);
    array.append(4.0f, 5.0f, 6.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)4.0f);
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array.vector2DAt(1, 0) == QVector2D(4.0f, 5.0f));
    QVERIFY(array.vector3DAt(-1, 0).isNull());
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array.vector3DAt(1, 0) == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(array.vector3DAt(2, 0).isNull());
    QCOMPARE(array.stride(), 3);
    QCOMPARE(array.vertexCount(), 2);
    QVERIFY(array.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array.vector4DAt(1, 0).isNull());

    QGLVertexArray array2(QGL::Position, 4);
    array2.append(1.0f, 2.0f, 3.0f);
    array2.append(4.0f, 5.0f, 6.0f);
    array2.append(7.0f, 8.0f, 9.0f);
    QCOMPARE(array2.stride(), 4);
    QCOMPARE(array2.vertexCount(), 2);
    QCOMPARE(array2.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array2.floatAt(1, 0), (qreal)5.0f);
    QCOMPARE(array2.floatAt(2, 0), (qreal)9.0f);
    QVERIFY(array2.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array2.vector2DAt(1, 0) == QVector2D(5.0f, 6.0f));
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array2.vector3DAt(1, 0) == QVector3D(5.0f, 6.0f, 7.0f));
    QVERIFY(array2.vector3DAt(2, 0).isNull());
    QVERIFY(array2.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array2.vector4DAt(1, 0) == QVector4D(5.0f, 6.0f, 7.0f, 8.0f));
    QVERIFY(array2.vector4DAt(2, 0).isNull());

    array2.append(10.0f, 11.0f, 12.0f);
    QCOMPARE(array2.stride(), 4);
    QCOMPARE(array2.vertexCount(), 3);
    QCOMPARE(array2.floatAt(2, 0), (qreal)9.0f);
    QVERIFY(array2.vector2DAt(2, 0) == QVector2D(9.0f, 10.0f));
    QVERIFY(array2.vector3DAt(2, 0) == QVector3D(9.0f, 10.0f, 11.0f));
    QVERIFY(array2.vector4DAt(2, 0) == QVector4D(9.0f, 10.0f, 11.0f, 12.0f));

    QGLVertexArray array3(QGL::Position, 2, QGL::TextureCoord0, 2);
    array3.append(1.0f, 2.0f, 3.0f);
    array3.append(4.0f, 5.0f, 6.0f);
    array3.append(7.0f, 8.0f, 9.0f);
    QCOMPARE(array3.stride(), 4);
    QCOMPARE(array3.vertexCount(), 2);
    QCOMPARE(array3.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array3.floatAt(0, 1), (qreal)3.0f);
    QCOMPARE(array3.floatAt(1, 0), (qreal)5.0f);
    QCOMPARE(array3.floatAt(1, 1), (qreal)7.0f);
    QVERIFY(array3.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array3.vector2DAt(0, 1) == QVector2D(3.0f, 4.0f));
    QVERIFY(array3.vector2DAt(1, 0) == QVector2D(5.0f, 6.0f));
    QVERIFY(array3.vector2DAt(1, 1) == QVector2D(7.0f, 8.0f));
    QVERIFY(array3.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array3.vector3DAt(0, 1) == QVector3D(3.0f, 4.0f, 5.0f));
    QVERIFY(array3.vector3DAt(1, 0) == QVector3D(5.0f, 6.0f, 7.0f));
    QVERIFY(array3.vector3DAt(1, 1) == QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(array3.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array3.vector4DAt(0, 1) == QVector4D(3.0f, 4.0f, 5.0f, 6.0f));
    QVERIFY(array3.vector4DAt(1, 0) == QVector4D(5.0f, 6.0f, 7.0f, 8.0f));
    QVERIFY(array3.vector4DAt(1, 1).isNull());
}

void tst_QGLVertexArray::append4D()
{
    QGLVertexArray array(QGL::Position, 4);
    array.append(1.0f, 2.0f, 3.0f, 4.0f);
    array.append(5.0f, 6.0f, 7.0f, 8.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)5.0f);
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array.vector2DAt(1, 0) == QVector2D(5.0f, 6.0f));
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array.vector3DAt(1, 0) == QVector3D(5.0f, 6.0f, 7.0f));
    QVERIFY(array.vector3DAt(2, 0).isNull());
    QVERIFY(array.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array.vector4DAt(1, 0) == QVector4D(5.0f, 6.0f, 7.0f, 8.0f));
    QVERIFY(array.vector4DAt(2, 0).isNull());
    QCOMPARE(array.stride(), 4);
    QCOMPARE(array.vertexCount(), 2);

    QGLVertexArray array2(QGL::Position, 3);
    array2.append(1.0f, 2.0f, 3.0f, 4.0f);
    array2.append(5.0f, 6.0f, 7.0f, 8.0f);
    array2.append(9.0f, 10.0f, 11.0f, 12.0f);
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 4);
    QCOMPARE(array2.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array2.floatAt(1, 0), (qreal)4.0f);
    QCOMPARE(array2.floatAt(2, 0), (qreal)7.0f);
    QCOMPARE(array2.floatAt(3, 0), (qreal)10.0f);
    QVERIFY(array2.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array2.vector2DAt(1, 0) == QVector2D(4.0f, 5.0f));
    QVERIFY(array2.vector2DAt(2, 0) == QVector2D(7.0f, 8.0f));
    QVERIFY(array2.vector2DAt(3, 0) == QVector2D(10.0f, 11.0f));
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array2.vector3DAt(1, 0) == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(array2.vector3DAt(2, 0) == QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(array2.vector3DAt(3, 0) == QVector3D(10.0f, 11.0f, 12.0f));
    QVERIFY(array2.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array2.vector4DAt(1, 0) == QVector4D(4.0f, 5.0f, 6.0f, 7.0f));
    QVERIFY(array2.vector4DAt(2, 0) == QVector4D(7.0f, 8.0f, 9.0f, 10.0f));
    QVERIFY(array2.vector4DAt(3, 0).isNull());

    array2.append(13.0f, 14.0f, 15.0f);
    QCOMPARE(array2.stride(), 3);
    QCOMPARE(array2.vertexCount(), 5);
    QCOMPARE(array2.floatAt(4, 0), (qreal)13.0f);
    QVERIFY(array2.vector2DAt(4, 0) == QVector2D(13.0f, 14.0f));
    QVERIFY(array2.vector3DAt(4, 0) == QVector3D(13.0f, 14.0f, 15.0f));
    QVERIFY(array2.vector4DAt(4, 0).isNull());
    array2.append(16.0f);
    QVERIFY(array2.vector3DAt(5, 0).isNull());
    QVERIFY(array2.vector4DAt(4, 0) == QVector4D(13.0f, 14.0f, 15.0f, 16.0f));

    QGLVertexArray array3(QGL::Position, 2, QGL::TextureCoord0, 2);
    array3.append(1.0f, 2.0f, 3.0f, 4.0f);
    array3.append(5.0f, 6.0f, 7.0f, 8.0f);
    QCOMPARE(array3.stride(), 4);
    QCOMPARE(array3.vertexCount(), 2);
    QCOMPARE(array3.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array3.floatAt(0, 1), (qreal)3.0f);
    QCOMPARE(array3.floatAt(1, 0), (qreal)5.0f);
    QCOMPARE(array3.floatAt(1, 1), (qreal)7.0f);
    QVERIFY(array3.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array3.vector2DAt(0, 1) == QVector2D(3.0f, 4.0f));
    QVERIFY(array3.vector2DAt(1, 0) == QVector2D(5.0f, 6.0f));
    QVERIFY(array3.vector2DAt(1, 1) == QVector2D(7.0f, 8.0f));
    QVERIFY(array3.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array3.vector3DAt(0, 1) == QVector3D(3.0f, 4.0f, 5.0f));
    QVERIFY(array3.vector3DAt(1, 0) == QVector3D(5.0f, 6.0f, 7.0f));
    QVERIFY(array3.vector3DAt(1, 1).isNull());
    QVERIFY(array3.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array3.vector4DAt(0, 1) == QVector4D(3.0f, 4.0f, 5.0f, 6.0f));
    QVERIFY(array3.vector4DAt(1, 0) == QVector4D(5.0f, 6.0f, 7.0f, 8.0f));
    QVERIFY(array3.vector4DAt(1, 1).isNull());
}

void tst_QGLVertexArray::appendLots()
{
    QGLVertexArray array(QGL::Color, 4);
    int index;

    for (index = 0; index < 2048; ++index)
        array.append((qreal)index);

    QCOMPARE(array.stride(), 4);
    QCOMPARE(array.vertexCount(), 2048 / 4);

    for (index = 0; index < 2048; index += 4) {
        QVERIFY(array.vector4DAt(index / 4, 0) == QVector4D(index, index + 1, index + 2, index + 3));
    }
}

void tst_QGLVertexArray::setFloatAt()
{
    QGLVertexArray array(QGL::Position, 1);
    QCOMPARE(array.floatAt(0, 0), (qreal)0.0f);

    array.append(1.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array.floatAt(-1, 0), (qreal)0.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)0.0f);

    array.setAt(0, 0, 6.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)6.0f);

    array.append(2.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)6.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)2.0f);

    array.setAt(1, 0, 7.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)6.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)7.0f);

    array.setAt(0, 0, 3.0f, 4.0f);
    QCOMPARE(array.floatAt(0, 0), (qreal)3.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)4.0f);
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(3.0f, 4.0f));
    QVERIFY(array.vector2DAt(1, 0).isNull());

    array.setAt(0, 0, QVector2D(1.0f, 2.0f));
    QCOMPARE(array.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)2.0f);
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array.vector2DAt(1, 0).isNull());

    // Out of range set should change nothing.
    array.setAt(0, 0, QVector3D(4.0f, 5.0f, 6.0f));
    QCOMPARE(array.floatAt(0, 0), (qreal)1.0f);
    QCOMPARE(array.floatAt(1, 0), (qreal)2.0f);
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array.vector2DAt(1, 0).isNull());

    QGLVertexArray array2(QGL::Position, 3, QGL::TextureCoord0, 2);
    array2.append(1.0f, 2.0f, 3.0f);
    array2.append(4.0f, 5.0f);
    array2.setAt(0, 0, 6.0f);
    array2.setAt(0, 1, 7.0f);
    QVERIFY(array2.vector2DAt(0, 0) == QVector2D(6.0f, 2.0f));
    QVERIFY(array2.vector2DAt(0, 1) == QVector2D(7.0f, 5.0f));
}

void tst_QGLVertexArray::setVector2DAt()
{
    QGLVertexArray array(QGL::Position, 2);
    QVERIFY(array.vector2DAt(0, 0).isNull());

    array.append(1.0f, 2.0f);
    array.append(3.0f, 4.0f);
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array.vector2DAt(1, 0) == QVector2D(3.0f, 4.0f));

    array.setAt(0, 0, 6.0f, 7.0f);
    array.setAt(1, 0, QVector2D(8.0f, 9.0f));
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(6.0f, 7.0f));
    QVERIFY(array.vector2DAt(1, 0) == QVector2D(8.0f, 9.0f));

    // Out of range set should change nothing.
    array.setAt(1, 0, QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(array.vector2DAt(0, 0) == QVector2D(6.0f, 7.0f));
    QVERIFY(array.vector2DAt(1, 0) == QVector2D(8.0f, 9.0f));

    QGLVertexArray array2(QGL::Position, 3, QGL::TextureCoord0, 2);
    array2.append(1.0f, 2.0f, 3.0f);
    array2.append(4.0f, 5.0f);
    array2.setAt(0, 0, 6.0f, 7.0f);
    array2.setAt(0, 1, QVector2D(8.0f, 9.0f));
    QVERIFY(array2.vector2DAt(0, 0) == QVector2D(6.0f, 7.0f));
    QVERIFY(array2.vector2DAt(0, 1) == QVector2D(8.0f, 9.0f));
}

void tst_QGLVertexArray::setVector3DAt()
{
    QGLVertexArray array(QGL::Position, 3);
    QVERIFY(array.vector3DAt(0, 0).isNull());

    array.append(1.0f, 2.0f, 3.0f);
    array.append(4.0f, 5.0f, 6.0f);
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array.vector3DAt(1, 0) == QVector3D(4.0f, 5.0f, 6.0f));

    array.setAt(0, 0, 7.0f, 8.0f, 9.0f);
    array.setAt(1, 0, QVector3D(10.0f, 11.0f, 12.0f));
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(array.vector3DAt(1, 0) == QVector3D(10.0f, 11.0f, 12.0f));

    // Out of range set should change nothing.
    array.setAt(1, 0, QVector4D(20.0f, 21.0f, 22.0f, 23.0f));
    QVERIFY(array.vector3DAt(0, 0) == QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(array.vector3DAt(1, 0) == QVector3D(10.0f, 11.0f, 12.0f));

    QGLVertexArray array2(QGL::Position, 3, QGL::Normal, 3);
    array2.append(1.0f, 2.0f, 3.0f);
    array2.append(4.0f, 5.0f, 6.0f);
    array2.setAt(0, 0, 6.0f, 7.0f, 8.0f);
    array2.setAt(0, 1, QVector3D(9.0f, 10.0f, 11.0f));
    QVERIFY(array2.vector3DAt(0, 0) == QVector3D(6.0f, 7.0f, 8.0f));
    QVERIFY(array2.vector3DAt(0, 1) == QVector3D(9.0f, 10.0f, 11.0f));
}

void tst_QGLVertexArray::setVector4DAt()
{
    QGLVertexArray array(QGL::Position, 4);
    QVERIFY(array.vector4DAt(0, 0).isNull());

    array.append(1.0f, 2.0f, 3.0f, 4.0f);
    array.append(5.0f, 6.0f, 7.0f, 8.0f);
    QVERIFY(array.vector4DAt(0, 0) == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array.vector4DAt(1, 0) == QVector4D(5.0f, 6.0f, 7.0f, 8.0f));

    array.setAt(0, 0, 7.0f, 8.0f, 9.0f, 10.0f);
    array.setAt(1, 0, QVector4D(11.0f, 12.0f, 13.0f, 14.0f));
    QVERIFY(array.vector4DAt(0, 0) == QVector4D(7.0f, 8.0f, 9.0f, 10.0f));
    QVERIFY(array.vector4DAt(1, 0) == QVector4D(11.0f, 12.0f, 13.0f, 14.0f));

    QGLVertexArray array2(QGL::Position, 4, QGL::Color, 4);
    array2.append(1.0f, 2.0f, 3.0f, 4.0f);
    array2.append(5.0f, 6.0f, 7.0f, 8.0f);
    array2.setAt(0, 0, 6.0f, 7.0f, 8.0f, 9.0f);
    array2.setAt(0, 1, QVector4D(9.0f, 10.0f, 11.0f, 12.0f));
    QVERIFY(array2.vector4DAt(0, 0) == QVector4D(6.0f, 7.0f, 8.0f, 9.0f));
    QVERIFY(array2.vector4DAt(0, 1) == QVector4D(9.0f, 10.0f, 11.0f, 12.0f));
}

void tst_QGLVertexArray::extractAndInterleave()
{
    QGLVertexArray array(QGL::Position, 2, QGL::TextureCoord0, 2);
    array.append(1.0f, 2.0f, 3.0f, 4.0f);
    array.append(5.0f, 6.0f, 7.0f, 8.0f);

    QGLVertexArray array2 = array.extractField(0);
    QGLVertexArray array3 = array.extractField(QGL::TextureCoord0);

    QCOMPARE(array2.stride(), 2);
    QCOMPARE(array2.vertexCount(), 2);
    QCOMPARE(array2.fieldCount(), 1);
    QVERIFY(array2.fields().fieldAttribute(0) == QGL::Position);
    QVERIFY(array2.fields().fieldSize(0) == 2);
    QVERIFY(array2.fields().fieldOffset(0) == 0);
    QVERIFY(array2.vector2DAt(0, 0) == QVector2D(1.0f, 2.0f));
    QVERIFY(array2.vector2DAt(1, 0) == QVector2D(5.0f, 6.0f));

    QCOMPARE(array3.stride(), 2);
    QCOMPARE(array3.vertexCount(), 2);
    QCOMPARE(array3.fieldCount(), 1);
    QVERIFY(array3.fields().fieldAttribute(0) == QGL::TextureCoord0);
    QVERIFY(array3.fields().fieldSize(0) == 2);
    QVERIFY(array3.fields().fieldOffset(0) == 0);
    QVERIFY(array3.vector2DAt(0, 0) == QVector2D(3.0f, 4.0f));
    QVERIFY(array3.vector2DAt(1, 0) == QVector2D(7.0f, 8.0f));

    array3.append(9.0f, 10.0f);

    QGLVertexArray array4 = array2.interleaved(array3);
    QCOMPARE(array4.stride(), 4);
    QCOMPARE(array4.vertexCount(), 3);

    QVERIFY(array4.vector2DAt(0, 0) == array.vector2DAt(0, 0));
    QVERIFY(array4.vector2DAt(0, 1) == array.vector2DAt(0, 1));
    QVERIFY(array4.vector2DAt(1, 0) == array.vector2DAt(1, 0));
    QVERIFY(array4.vector2DAt(1, 1) == array.vector2DAt(1, 1));
    QVERIFY(array4.vector2DAt(2, 0) == QVector2D(0.0f, 0.0f));
    QVERIFY(array4.vector2DAt(2, 1) == QVector2D(9.0f, 10.0f));
}

void tst_QGLVertexArray::operatorEquals()
{
    QGLVertexArray array(QGL::Position, 2, QGL::TextureCoord0, 2);
    array.append(1.0f, 2.0f, 3.0f, 4.0f);
    array.append(5.0f, 6.0f, 7.0f, 8.0f);

    QGLVertexArray array2(QGL::Position, 2, QGL::TextureCoord0, 2); // null array
    QGLVertexArray array3(QGL::Position, 2, QGL::TextureCoord0, 2); // contains same items
    array3.append(1.0f, 2.0f, 3.0f, 4.0f);
    array3.append(5.0f, 6.0f, 7.0f, 8.0f);
    QGLVertexArray array4(QGL::Position, 2, QGL::TextureCoord0, 2); // another null
    QGLVertexArray array5(QGL::Position, 2, QGL::TextureCoord0, 2); // contains same number of items, but they differ
    array5.append(1.0f, 2.0f, 3.0f, 4.0f);
    array5.append(5.0f, 8.0f, 9.0f, 8.0f);
    QGLVertexArray array6(QGL::Position, 2, QGL::TextureCoord0, 2); // contains different number of items
    array6.append(1.0f, 2.0f, 3.0f, 4.0f);
    array6.append(5.0f, 6.0f, 7.0f, 8.0f);
    array6.append(1.0f, 2.0f, 3.0f, 4.0f);
    array6.append(5.0f, 6.0f, 7.0f, 8.0f);

    QCOMPARE(array, array);       // calls operator == under the hood, should be self equal
    QVERIFY(!(array == array2));  // not equal to a null
    QCOMPARE(array, array3);      // same items, should be equal
    QVERIFY(!(array == array5));  // same item count, but contents differ: !=
    QVERIFY(!(array == array6));  // different item count, should (quickly) return false

    QCOMPARE(array2, array2);     // should be self equal, even tho' null
    QVERIFY(!(array2 == array3)); // null not equal, other way round
    QCOMPARE(array2, array4);     // two null arrays equal
}

QTEST_APPLESS_MAIN(tst_QGLVertexArray)

#include "tst_qglvertexarray.moc"
