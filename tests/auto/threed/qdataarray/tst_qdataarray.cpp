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
#include "qdataarray.h"
#include <QtGui/qvector2d.h>
#include <QtGui/qvector3d.h>
#include <QtGui/qvector4d.h>

class tst_QDataArray : public QObject
{
    Q_OBJECT
public:
    tst_QDataArray() {}
    ~tst_QDataArray() {}

private slots:
    void create();
    void append1D();
    void append2D();
    void append3D();
    void append4D();
    void appendArray();
    void setAt();
    void replace();
};

// This must match the default for PreallocSize.
static const int ExpectedMinCapacity = 8;

void tst_QDataArray::create()
{
    QDataArray<float> array;
    QCOMPARE(array.count(), 0);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
    QVERIFY(array.constData() != 0);
    QVERIFY(array.data() == array.constData());
}

void tst_QDataArray::append1D()
{
    QDataArray<float> array;
    int index;

    // The current constData() pointer should be pointing into
    // the middle of "array" at the m_prealloc structure.
    const float *d = array.constData();
    QVERIFY(d >= (const float *)&array);
    QVERIFY(d < (const float *)((&array) + 1));

    // Appending up to the minimum capacity should not cause a realloc.
    for (index = 0; index < ExpectedMinCapacity; ++index) {
        array.append(float(index));
        QVERIFY(array.constData() == d);
    }

    // Check that the array contains the values we expected.
    QCOMPARE(array.count(), ExpectedMinCapacity);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
    for (index = 0; index < ExpectedMinCapacity; ++index) {
        QCOMPARE(array[index], float(index));
    }

    // Append 1 more item and check for realloc.
    array.append(1000.0f);
    QCOMPARE(array.count(), ExpectedMinCapacity + 1);
    QVERIFY(array.capacity() > ExpectedMinCapacity);
    QVERIFY(array.capacity() >= array.count());

    // Check that the array still contains the values we expected.
    for (index = 0; index < ExpectedMinCapacity; ++index) {
        QCOMPARE(array[index], float(index));
    }
    QCOMPARE(array[ExpectedMinCapacity], 1000.0f);

    // Append a large number of values to test constant reallocation.
    for (index = 0; index < 1000; ++index)
        array.append(float(index));
    QCOMPARE(array.count(), ExpectedMinCapacity + 1 + 1000);

    // Make two copies of the array.
    QDataArray<float> array2(array);
    QCOMPARE(array2.count(), ExpectedMinCapacity + 1 + 1000);
    QDataArray<float> array3;
    array3 = array;
    QCOMPARE(array3.count(), ExpectedMinCapacity + 1 + 1000);

    // Add another item to the original and check that the copy is unchanged.
    array.append(1500.0f);
    QCOMPARE(array.count(), ExpectedMinCapacity + 1 + 1000 + 1);
    QCOMPARE(array2.count(), ExpectedMinCapacity + 1 + 1000);
    QCOMPARE(array3.count(), ExpectedMinCapacity + 1 + 1000);

    // Check that the original and the copy contain the right values.
    for (index = 0; index < ExpectedMinCapacity; ++index) {
        QCOMPARE(array[index], float(index));
        QCOMPARE(array2[index], float(index));
        QCOMPARE(array3[index], float(index));
    }
    QCOMPARE(array[ExpectedMinCapacity], 1000.0f);
    QCOMPARE(array2[ExpectedMinCapacity], 1000.0f);
    QCOMPARE(array3[ExpectedMinCapacity], 1000.0f);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array[index + ExpectedMinCapacity + 1], float(index));
    }
    QCOMPARE(array[ExpectedMinCapacity + 1000 + 1], 1500.0f);
}

void tst_QDataArray::append2D()
{
    int index;

    QDataArray<QVector2D> array;
    array.append(QVector2D(1.0f, 2.0f));
    array.append(QVector2D(3.0f, 4.0f));
    QCOMPARE(array[0].x(), qreal(1.0f));
    QCOMPARE(array[0].y(), qreal(2.0f));
    QCOMPARE(array[1].x(), qreal(3.0f));
    QCOMPARE(array[1].y(), qreal(4.0f));
    QCOMPARE(array.count(), 2);

    QDataArray<QVector2D> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(QVector2D(index, index + 1));
    QCOMPARE(array2.count(), 1000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index].x(), qreal(index));
        QCOMPARE(array2[index].y(), qreal(index + 1));
    }
}

void tst_QDataArray::append3D()
{
    int index;

    QDataArray<QVector3D> array;
    array.append(QVector3D(1.0f, 2.0f, 3.0f));
    array.append(QVector3D(4.0f, 5.0f, 6.0f));
    QCOMPARE(array[0].x(), qreal(1.0f));
    QCOMPARE(array[0].y(), qreal(2.0f));
    QCOMPARE(array[0].z(), qreal(3.0f));
    QCOMPARE(array[1].x(), qreal(4.0f));
    QCOMPARE(array[1].y(), qreal(5.0f));
    QCOMPARE(array[1].z(), qreal(6.0f));
    QCOMPARE(array.count(), 2);

    QDataArray<QVector3D> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(QVector3D(index, index + 1, index + 2));
    QCOMPARE(array2.count(), 1000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index].x(), qreal(index));
        QCOMPARE(array2[index].y(), qreal(index + 1));
        QCOMPARE(array2[index].z(), qreal(index + 2));
    }
}

void tst_QDataArray::append4D()
{
    int index;

    QDataArray<QVector4D> array;
    array.append(QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    array.append(QVector4D(5.0f, 6.0f, 7.0f, 8.0f));
    QCOMPARE(array[0].x(), qreal(1.0f));
    QCOMPARE(array[0].y(), qreal(2.0f));
    QCOMPARE(array[0].z(), qreal(3.0f));
    QCOMPARE(array[0].w(), qreal(4.0f));
    QCOMPARE(array[1].x(), qreal(5.0f));
    QCOMPARE(array[1].y(), qreal(6.0f));
    QCOMPARE(array[1].z(), qreal(7.0f));
    QCOMPARE(array[1].w(), qreal(8.0f));
    QCOMPARE(array.count(), 2);

    QDataArray<QVector4D> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(QVector4D(index, index + 1, index + 2, index + 3));
    QCOMPARE(array2.count(), 1000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index].x(), qreal(index));
        QCOMPARE(array2[index].y(), qreal(index + 1));
        QCOMPARE(array2[index].z(), qreal(index + 2));
        QCOMPARE(array2[index].w(), qreal(index + 3));
    }
}

void tst_QDataArray::appendArray()
{
    QDataArray<float> array;
    QDataArray<float> array2;
    int index;

    for (index = 0; index < 1000; ++index) {
        array.append(index);
        array2.append(1000 - index);
    }

    array.append(array2);
    QCOMPARE(array.count(), 2000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array[index], float(index));
        QCOMPARE(array[index + 1000], float(1000 - index));
    }

    array2.append(array2);
    QCOMPARE(array2.count(), 2000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index], float(1000 - index));
        QCOMPARE(array2[index + 1000], float(1000 - index));
    }
}

void tst_QDataArray::setAt()
{
    int index;

    QDataArray<float> array;
    array.append(1.0f);
    QCOMPARE(array[0], 1.0f);

    array[0] = 6.0f;
    QCOMPARE(array[0], 6.0f);

    QDataArray<float> array2;
    for(index = 0; index < 1000; ++index)
        array2.append(index);
    for(index = 0; index < 1000; ++index)
        array2[index] = index + 1000;
    for(index = 0; index < 1000; ++index)
        QCOMPARE(array2[index], float(index + 1000));
}

void tst_QDataArray::replace()
{
    QDataArray<float> array;
    QDataArray<float> array2;
    int index;

    for (index = 0; index < 1000; ++index) {
        array.append(index);
        array2.append(1000 - index);
    }

    array.replace(500, array2.constData(), 500);
    QCOMPARE(array.count(), 1000);
    for (index = 0; index < 1000; ++index) {
        if (index < 500)
            QCOMPARE(array[index], float(index));
        else
            QCOMPARE(array[index], float(1000 - (index - 500)));
    }

    array.replace(900, array2.constData(), 500);
    QCOMPARE(array.count(), 1400);
    for (index = 0; index < 1400; ++index) {
        if (index < 500)
            QCOMPARE(array[index], float(index));
        else if (index < 900)
            QCOMPARE(array[index], float(1000 - (index - 500)));
        else
            QCOMPARE(array[index], float(1000 - (index - 900)));
    }
}

QTEST_APPLESS_MAIN(tst_QDataArray)

#include "tst_qdataarray.moc"
