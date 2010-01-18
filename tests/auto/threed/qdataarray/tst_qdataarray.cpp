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
    void append();
    void appendTwoAtATime();
    void appendThreeAtATime();
    void appendFourAtATime();
    void appendArray();
    void setAt();
    void replace();
    void copy();
    void resize();
    void reserve();
    void squeeze();
    void compare();
    void remove();
    void extend();
    void mid();
    void left();
    void right();
    void iterate();
};

// This must match the default for PreallocSize.
static const int ExpectedMinCapacity = 8;

void tst_QDataArray::create()
{
    QDataArray<float> array;

    // Check the basic properties.
    QVERIFY(array.isEmpty());
    QCOMPARE(array.count(), 0);
    QCOMPARE(array.size(), 0);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
    QVERIFY(array.constData() != 0);
    QVERIFY(array.data() == array.constData());

    // The current constData() pointer should be pointing into
    // the middle of "array" at the m_prealloc structure.
    const float *d = array.constData();
    QVERIFY(d >= (const float *)&array);
    QVERIFY(d < (const float *)((&array) + 1));

    // Add one element and check the basic properties again.
    array.append(1.0f);
    QVERIFY(!array.isEmpty());
    QCOMPARE(array.count(), 1);
    QCOMPARE(array.size(), 1);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
    QVERIFY(array.constData() != 0);
    QVERIFY(array.data() == array.constData());

    // Create another array that is filled with an initial value.
    QDataArray<QVector3D> array2(100, QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(!array2.isEmpty());
    QCOMPARE(array2.count(), 100);
    QCOMPARE(array2.size(), 100);
    QVERIFY(array2.capacity() >= 100);
    QVERIFY(array2.constData() != 0);
    QVERIFY(array2.data() == array2.constData());
    for (int index = 0; index < 100; ++index)
        QVERIFY(array2.at(index) == QVector3D(1.0f, 2.0f, 3.0f));

    // Create a filled array that is within the prealloc area.
    QDataArray<float> array3(ExpectedMinCapacity, 42.5f);
    QVERIFY(!array3.isEmpty());
    QCOMPARE(array3.count(), ExpectedMinCapacity);
    QCOMPARE(array3.size(), ExpectedMinCapacity);
    QVERIFY(array3.capacity() == ExpectedMinCapacity);
    QVERIFY(array3.constData() != 0);
    QVERIFY(array3.data() == array3.constData());
    for (int index = 0; index < ExpectedMinCapacity; ++index)
        QVERIFY(array3.at(index) == 42.5f);
}

void tst_QDataArray::append()
{
    QDataArray<float> array;
    int index;

    // Appending up to the minimum capacity should not cause a realloc.
    const float *d = array.constData();
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
    array += float(1000.0f);
    QCOMPARE(array.count(), ExpectedMinCapacity + 1);
    QVERIFY(array.capacity() > ExpectedMinCapacity);
    QVERIFY(array.capacity() >= array.count());
    QCOMPARE(array.size(), array.count());

    // Check that the array still contains the values we expected.
    for (index = 0; index < ExpectedMinCapacity; ++index) {
        QCOMPARE(array[index], float(index));
    }
    QCOMPARE(array[ExpectedMinCapacity], 1000.0f);

    // Append a large number of values to test constant reallocation.
    for (index = 0; index < 1000; ++index)
        array.append(float(index));
    QCOMPARE(array.count(), ExpectedMinCapacity + 1 + 1000);
    QCOMPARE(array.size(), array.count());

    // Make two copies of the array.
    QDataArray<float> array2(array);
    QCOMPARE(array2.count(), ExpectedMinCapacity + 1 + 1000);
    QCOMPARE(array2.size(), array2.count());
    QVERIFY(!array2.isEmpty());
    QVERIFY(array2.capacity() >= array2.size());
    QDataArray<float> array3;
    array3 = array;
    QCOMPARE(array3.count(), ExpectedMinCapacity + 1 + 1000);
    QCOMPARE(array3.size(), array3.count());
    QVERIFY(!array3.isEmpty());
    QVERIFY(array3.capacity() >= array3.size());

    // Check that we actually are sharing the data between the copies.
    QVERIFY(array.constData() == array2.constData());
    QVERIFY(array.constData() == array3.constData());
    d = array2.constData();

    // Add another item to the original and check that the copy is unchanged.
    array << float(1500.0f);
    QCOMPARE(array.count(), ExpectedMinCapacity + 1 + 1000 + 1);
    QCOMPARE(array2.count(), ExpectedMinCapacity + 1 + 1000);
    QCOMPARE(array3.count(), ExpectedMinCapacity + 1 + 1000);

    // Check that we have detached the first array (copy-on-write).
    QVERIFY(array.constData() != array2.constData());
    QVERIFY(array.constData() != array3.constData());
    QVERIFY(array2.constData() == array3.constData());
    QVERIFY(array2.constData() == d);

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

void tst_QDataArray::appendTwoAtATime()
{
    int index;

    QDataArray<float> array;
    array.append(1.0f, 2.0f);
    array.append(3.0f, 4.0f);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);
    QCOMPARE(array.count(), 4);

    QDataArray<float> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(float(index), float(index + 1));
    QCOMPARE(array2.count(), 2000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index * 2], float(index));
        QCOMPARE(array2[index * 2 + 1], float(index + 1));
    }
}

void tst_QDataArray::appendThreeAtATime()
{
    int index;

    QDataArray<float> array;
    array.append(1.0f, 2.0f, 3.0f);
    array.append(4.0f, 5.0f, 6.0f);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);
    QCOMPARE(array[4], 5.0f);
    QCOMPARE(array[5], 6.0f);
    QCOMPARE(array.count(), 6);

    QDataArray<float> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(float(index), float(index + 1), float(index + 2));
    QCOMPARE(array2.count(), 3000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index * 3], float(index));
        QCOMPARE(array2[index * 3 + 1], float(index + 1));
        QCOMPARE(array2[index * 3 + 2], float(index + 2));
    }
}

void tst_QDataArray::appendFourAtATime()
{
    int index;

    QDataArray<float> array;
    array.append(1.0f, 2.0f, 3.0f, 4.0f);
    array.append(5.0f, 6.0f, 7.0f, 8.0f);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);
    QCOMPARE(array[4], 5.0f);
    QCOMPARE(array[5], 6.0f);
    QCOMPARE(array[6], 7.0f);
    QCOMPARE(array[7], 8.0f);
    QCOMPARE(array.count(), 8);

    QDataArray<float> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(float(index), float(index + 1), float(index + 2), float(index + 3));
    QCOMPARE(array2.count(), 4000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index * 4], float(index));
        QCOMPARE(array2[index * 4 + 1], float(index + 1));
        QCOMPARE(array2[index * 4 + 2], float(index + 2));
        QCOMPARE(array2[index * 4 + 3], float(index + 3));
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

    array2 << array2;
    QCOMPARE(array2.count(), 2000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index], float(1000 - index));
        QCOMPARE(array2[index + 1000], float(1000 - index));
    }

    array2 += (QDataArray<float>());
    QCOMPARE(array2.count(), 2000);
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

    // Replace and extend the array from the middle.
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

    // Check the bail-out cases when index is negative or count zero.
    array.replace(900, array2.constData(), 0);
    array.replace(-1, array2.constData(), 900);
    QCOMPARE(array.count(), 1400);
    for (index = 0; index < 1400; ++index) {
        if (index < 500)
            QCOMPARE(array[index], float(index));
        else if (index < 900)
            QCOMPARE(array[index], float(1000 - (index - 500)));
        else
            QCOMPARE(array[index], float(1000 - (index - 900)));
    }

    // Replace beyond the end of the array.
    static float const extras[] = {0.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f};
    array.replace(1402, extras + 2, 4);
    QCOMPARE(array.count(), 1406);
    for (index = 0; index < 1406; ++index) {
        if (index < 500)
            QCOMPARE(array[index], float(index));
        else if (index < 900)
            QCOMPARE(array[index], float(1000 - (index - 500)));
        else if (index < 1400)
            QCOMPARE(array[index], float(1000 - (index - 900)));
        else
            QCOMPARE(array[index], extras[index - 1400]);
    }
}

// Exercise the various conditionals in operator=().
void tst_QDataArray::copy()
{
    QDataArray<float> array, array2, array3;

    array2.append(1.0f);
    array2.append(7.0f);
    array = array2;
    QCOMPARE(array.count(), 2);
    QCOMPARE(array.at(0), float(1.0f));
    QCOMPARE(array.at(1), float(7.0f));

    array = array3;
    QCOMPARE(array.count(), 0);

    array2 = array2;
    QCOMPARE(array2.count(), 2);
    QCOMPARE(array2.at(0), float(1.0f));
    QCOMPARE(array2.at(1), float(7.0f));

    QDataArray<float> array4(array2);
    QCOMPARE(array2.count(), 2);
    QCOMPARE(array2.at(0), float(1.0f));
    QCOMPARE(array2.at(1), float(7.0f));

    QDataArray<float> array5, array6;
    for (int index = 0; index < 32; ++index)
        array5.append(2.0f);
    QCOMPARE(array5.count(), 32);
    array6.append(1.0f);
    array5 = array6;
    QCOMPARE(array5.count(), 1);
    QCOMPARE(array5.at(0), float(1.0f));

    array5.clear();
    QCOMPARE(array5.count(), 0);
    QCOMPARE(array6.count(), 1);
    QCOMPARE(array6.at(0), float(1.0f));
    array6.clear();
    QCOMPARE(array5.count(), 0);
}

void tst_QDataArray::resize()
{
    QDataArray<int> array;

    array.resize(-1);
    QCOMPARE(array.count(), 0);

    array.resize(ExpectedMinCapacity);
    QCOMPARE(array.count(), ExpectedMinCapacity);
    for (int index = 0; index < ExpectedMinCapacity; ++index)
        QCOMPARE(array[index], 0);
    array.append(1);

    array.resize(100);
    QCOMPARE(array.count(), 100);
    QVERIFY(array.capacity() >= 100);
    int cap = array.capacity();
    for (int index = 0; index < 100; ++index){
        if (index != 8)
            QCOMPARE(array[index], 0);
        else
            QCOMPARE(array[index], 1);
    }

    array.resize(50);
    QCOMPARE(array.count(), 50);
    QVERIFY(array.capacity() == cap); // Shouldn't change the capacity.
    for (int index = 0; index < 50; ++index){
        if (index != 8)
            QCOMPARE(array[index], 0);
        else
            QCOMPARE(array[index], 1);
    }

    array.resize(50);
    QCOMPARE(array.count(), 50);

    QDataArray<int> array2(array);
    array.resize(40);
    QCOMPARE(array.count(), 40);
    QCOMPARE(array2.count(), 50);

    array2.resize(20);
    QCOMPARE(array2.count(), 20);

    for (int index = 0; index < 40; ++index){
        if (index != 8)
            QCOMPARE(array[index], 0);
        else
            QCOMPARE(array[index], 1);
    }
    for (int index = 0; index < 20; ++index){
        if (index != 8)
            QCOMPARE(array2[index], 0);
        else
            QCOMPARE(array2[index], 1);
    }

    // Check that resizing to zero keeps the same memory storage.
    const int *d = array.constData();
    array.resize(0);
    QVERIFY(array.constData() == d);
    QVERIFY(array.capacity() != ExpectedMinCapacity);

    // Calling clear will reclaim the storage.
    array.clear();
    QVERIFY(array.constData() != d);
    QVERIFY(array.capacity() == ExpectedMinCapacity);
}

void tst_QDataArray::reserve()
{
    QDataArray<float> array;
    array.reserve(1000);
    QVERIFY(array.isEmpty());
    QVERIFY(array.capacity() >= 1000);

    // Append elements and check for reallocation.
    const float *d = array.constData();
    for (int index = 0; index < 1000; ++index) {
        array.append(float(index));
        QVERIFY(array.constData() == d);
    }

    // Reserving less doesn't change the capacity, or the count.
    array.reserve(50);
    QVERIFY(array.capacity() >= 1000);
    QCOMPARE(array.count(), 1000);
}

void tst_QDataArray::squeeze()
{
    QDataArray<float> array;
    array.reserve(100);
    QVERIFY(array.isEmpty());
    QVERIFY(array.capacity() >= 100);

    for (int index = 0; index < 100; ++index)
        array.append(float(index));

    array.reserve(400);
    QVERIFY(array.capacity() >= 400);

    // Squeezing sets the capacity to exactly the value that is specified.
    array.squeeze(200);
    QCOMPARE(array.capacity(), 200);
    QCOMPARE(array.count(), 100);

    // Can't squeeze to something larger.
    array.squeeze(300);
    QCOMPARE(array.capacity(), 200);
    QCOMPARE(array.count(), 100);

    // Drop elements from the end.
    array.squeeze(50);
    QCOMPARE(array.capacity(), 50);
    QCOMPARE(array.count(), 50);
    for (int index = 0; index < 50; ++index)
        QCOMPARE(array[index], float(index));

    // Test squeezing within the preallocated area.
    QDataArray<float> array2;
    array2.append(1.0f);
    array2.append(2.0f);
    array2.append(3.0f);
    array2.squeeze(2);
    QCOMPARE(array2.capacity(), ExpectedMinCapacity);
    QCOMPARE(array2.count(), 2);

    // Test copy-on-write during squeezing.
    QDataArray<float> array3(array);
    array3.squeeze(20);
    QCOMPARE(array3.count(), 20);
    QCOMPARE(array.count(), 50);

    // Clear and check that the array reverts to preallocation.
    array.squeeze(0);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
}

void tst_QDataArray::compare()
{
    QDataArray<float> array, array2, array3;

    QVERIFY(array == array2);

    array.append(1.0f);
    array3.append(1.0f);
    QDataArray<float> array4(array);

    QVERIFY(array == array);
    QVERIFY(array != array2);
    QVERIFY(array == array3);
    QVERIFY(array == array4);

    array2.append(2.0f);
    QVERIFY(array != array2);

    array2.append(1.0f);
    QVERIFY(array != array2);
}

void tst_QDataArray::remove()
{
    QDataArray<float> array;

    array.remove(0, 100);
    QCOMPARE(array.count(), 0);

    for (int index = 0; index < 100; ++index)
        array.append(float(index));

    array.remove(100, 60);
    QCOMPARE(array.count(), 100);
    array.remove(-60, 60);

    array.remove(10, 20);
    QCOMPARE(array.count(), 80);
    for (int index = 0; index < 80; ++index) {
        if (index < 10)
            QCOMPARE(array[index], float(index));
        else
            QCOMPARE(array[index], float(index + 20));
    }

    QDataArray<float> array2(array);
    array2.remove(-10, 20);
    array2.remove(60, 20);
    QCOMPARE(array2.count(), 60);
    for (int index = 0; index < 60; ++index) {
        QCOMPARE(array2[index], float(index + 30));
    }

    array.remove(0, 10);
    QCOMPARE(array.count(), 70);
    for (int index = 0; index < 70; ++index) {
        QCOMPARE(array[index], float(index + 30));
    }

    // Remove everything and check that it returns to preallocation.
    array.remove(0, array.size());
    QCOMPARE(array.count(), 0);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
}

void tst_QDataArray::extend()
{
    QDataArray<float> array;

    float *ptr = array.extend(4);
    ptr[0] = 1.0f;
    ptr[1] = 2.0f;
    ptr[2] = 3.0f;
    ptr[3] = 4.0f;
    QCOMPARE(array.size(), 4);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);

    array.append(5.0f);
    ptr = array.extend(1);
    ptr[0] = 6.0f;
    QCOMPARE(array.size(), 6);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);
    QCOMPARE(array[4], 5.0f);
    QCOMPARE(array[5], 6.0f);

    QDataArray<float> array2(array);

    ptr = array.extend(1);
    ptr[0] = 7.0f;
    QCOMPARE(array.size(), 7);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);
    QCOMPARE(array[4], 5.0f);
    QCOMPARE(array[5], 6.0f);
    QCOMPARE(array[6], 7.0f);

    QCOMPARE(array2.size(), 6);
}

void tst_QDataArray::mid()
{
    QDataArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    // Check the null case.
    QDataArrayRef<float> mid;
    QCOMPARE(mid.size(), 0);
    QCOMPARE(mid.count(), 0);
    QCOMPARE(mid.offset(), 0);
    QVERIFY(mid.isEmpty());
    QVERIFY(mid.isNull());
    QVERIFY(mid.data() == 0);
    QVERIFY(mid.constData() == 0);

    QDataArray<float> midarray = mid.toDataArray();
    QVERIFY(midarray.isEmpty());

    mid = array.mid(0);
    QCOMPARE(mid.offset(), 0);
    QCOMPARE(mid.size(), array.size());
    QVERIFY(!mid.isEmpty());
    QVERIFY(!mid.isNull());
    QVERIFY(mid.data() == array.data());
    QVERIFY(mid.constData() == array.constData());
    QVERIFY(mid.dataArray() == &array);
    for (int index = 0; index < 1024; ++index) {
        QCOMPARE(mid.at(index), float(index));
        QCOMPARE(mid[index], float(index));
    }

    midarray = mid.toDataArray();
    QCOMPARE(midarray.size(), mid.size());
    for (int index = 0; index < mid.size(); ++index)
        QCOMPARE(midarray[index], mid[index]);

    mid = array.mid(500, 20);
    QCOMPARE(mid.offset(), 500);
    QCOMPARE(mid.size(), 20);
    QVERIFY(!mid.isEmpty());
    QVERIFY(!mid.isNull());
    QVERIFY(mid.data() == (array.data() + 500));
    QVERIFY(mid.constData() == (array.constData() + 500));
    QVERIFY(mid.dataArray() == &array);
    for (int index = 0; index < 20; ++index) {
        QCOMPARE(mid.at(index), float(index + 500));
        QCOMPARE(mid[index], float(index + 500));
    }

    mid[10] = 0.0f;
    QCOMPARE(array.at(510), float(0.0f));
    mid[10] = 510.0f;
    QCOMPARE(array.at(510), float(510.0f));

    QDataArrayRef<float> mid2(&array, 500, 20);
    QVERIFY(mid == mid2);
    QVERIFY(!(mid != mid2));

    QDataArrayRef<float> mid3(&array, 500, 21);
    QVERIFY(mid != mid3);
    QVERIFY(!(mid == mid3));

    QDataArrayRef<float> mid4(&array, 400, 20);
    QVERIFY(mid != mid4);
    QVERIFY(!(mid == mid4));

    QDataArrayRef<float> mid5;
    QDataArrayRef<float> mid6;
    QVERIFY(mid != mid5);
    QVERIFY(!(mid == mid5));
    QVERIFY(mid5 != mid);
    QVERIFY(!(mid5 == mid));
    QVERIFY(mid5 == mid6);
    QVERIFY(!(mid5 != mid6));

    midarray = mid.toDataArray();
    QCOMPARE(midarray.size(), mid.size());
    for (int index = 0; index < mid.size(); ++index)
        QCOMPARE(midarray[index], mid[index]);

    QDataArray<float> midarray2(mid.toDataArray());
    QVERIFY(midarray == midarray2);

    mid = array.mid(500, 0);
    QCOMPARE(mid.offset(), 500);
    QCOMPARE(mid.size(), 0);
    QVERIFY(mid.isEmpty());

    midarray = mid.toDataArray();
    QVERIFY(midarray.isEmpty());

    mid = array.mid(1000, 30);
    QCOMPARE(mid.offset(), 1000);
    QCOMPARE(mid.size(), 24);
    QVERIFY(!mid.isEmpty());
    QVERIFY(!mid.isNull());
    QVERIFY(mid.data() == (array.data() + 1000));
    QVERIFY(mid.constData() == (array.constData() + 1000));
    QVERIFY(mid.dataArray() == &array);
    for (int index = 0; index < 24; ++index) {
        QCOMPARE(mid.at(index), float(index + 1000));
        QCOMPARE(mid[index], float(index + 1000));
    }

    // Constructor should act like mid().
    QDataArrayRef<float> mid7(&array, 1000, 30);
    QCOMPARE(mid7.offset(), 1000);
    QCOMPARE(mid7.size(), 24);
    QVERIFY(!mid7.isEmpty());
    QVERIFY(!mid7.isNull());
    QVERIFY(mid7.data() == (array.data() + 1000));
    QVERIFY(mid7.constData() == (array.constData() + 1000));
    QVERIFY(mid7.dataArray() == &array);
    for (int index = 0; index < 24; ++index) {
        QCOMPARE(mid7.at(index), float(index + 1000));
        QCOMPARE(mid7[index], float(index + 1000));
    }

    QDataArrayRef<float> mid8(&array);
    QCOMPARE(mid8.offset(), 0);
    QCOMPARE(mid8.size(), array.size());
    QVERIFY(!mid8.isEmpty());
    QVERIFY(!mid8.isNull());
    QVERIFY(mid8.data() == array.data());
    QVERIFY(mid8.constData() == array.constData());
    QVERIFY(mid8.dataArray() == &array);
    for (int index = 0; index < 1024; ++index) {
        QCOMPARE(mid8.at(index), float(index));
        QCOMPARE(mid8[index], float(index));
    }

    QDataArrayRef<float> mid9(0);
    QCOMPARE(mid9.offset(), 0);
    QCOMPARE(mid9.size(), 0);
    QVERIFY(mid9.isEmpty());
    QVERIFY(mid9.isNull());
    QVERIFY(mid9.data() == 0);
    QVERIFY(mid9.constData() == 0);
    QVERIFY(mid9.dataArray() == 0);
}

void tst_QDataArray::left()
{
    QDataArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    QDataArrayRef<float> left = array.left(-1);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 1024);
    QVERIFY(left.dataArray() == &array);

    left = array.left(0);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 0);
    QVERIFY(left.dataArray() == &array);

    left = array.left(500);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 500);
    QVERIFY(left.dataArray() == &array);

    left = array.left(2048);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 1024);
    QVERIFY(left.dataArray() == &array);
}

void tst_QDataArray::right()
{
    QDataArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    QDataArrayRef<float> right = array.right(-1);
    QCOMPARE(right.offset(), 0);
    QCOMPARE(right.size(), 1024);
    QVERIFY(right.dataArray() == &array);

    right = array.right(0);
    QCOMPARE(right.offset(), 1024);
    QCOMPARE(right.size(), 0);
    QVERIFY(right.dataArray() == &array);

    right = array.right(500);
    QCOMPARE(right.offset(), 1024 - 500);
    QCOMPARE(right.size(), 500);
    QVERIFY(right.dataArray() == &array);

    right = array.right(2048);
    QCOMPARE(right.offset(), 0);
    QCOMPARE(right.size(), 1024);
    QVERIFY(right.dataArray() == &array);
}

void tst_QDataArray::iterate()
{
    QDataArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    QDataArray<float>::Iterator it1;
    int value = 0;
    for (it1 = array.begin(); it1 != array.end(); ++it1)
        QCOMPARE(*it1, float(value++));
    QCOMPARE(value, array.size());

    QDataArray<float>::ConstIterator it2;
    value = 0;
    for (it2 = array.constBegin(); it2 != array.constEnd(); ++it2)
        QCOMPARE(*it2, float(value++));
    QCOMPARE(value, array.size());

    value = 0;
    for (it1 = array.begin(); it1 != array.end(); ++it1)
        *it1 = float(1024 - value++);
    value = 0;
    for (it2 = array.constBegin(); it2 != array.constEnd(); ++it2) {
        QCOMPARE(*it2, float(1024 - value));
        QCOMPARE(array[value], float(1024 - value));
        ++value;
    }

    for (int index = 0; index < 1024; ++index)
        array[index] = float(index);

    QDataArrayRef<float> mid = array.mid(512, 256);
    QDataArrayRef<float>::Iterator it3;
    value = 512;
    for (it3 = mid.begin(); it3 != mid.end(); ++it3)
        QCOMPARE(*it3, float(value++));
    QCOMPARE(value - 512, mid.size());

    QDataArrayRef<float>::Iterator it4;
    value = 512;
    for (it4 = mid.begin(); it4 != mid.end(); ++it4)
        *it4 = float(1024 - value++);

    QDataArrayRef<float>::ConstIterator it5;
    value = 512;
    for (it5 = mid.constBegin(); it5 != mid.constEnd(); ++it5)
        QCOMPARE(*it5, float(1024 - value++));
    QCOMPARE(value - 512, mid.size());
}

QTEST_APPLESS_MAIN(tst_QDataArray)

#include "tst_qdataarray.moc"
