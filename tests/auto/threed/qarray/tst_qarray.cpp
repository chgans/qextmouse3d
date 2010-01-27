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
#include "qarray.h"
#include <QtGui/qvector3d.h>
#include <QtCore/qstring.h>

// We ensure to test the following types to cover all of the
// relevant QTypeInfo variations:
//
// QArray<float>        Primitive types
// QArray<QString>      Movable types
// QArray<ComplexValue> Complex types

class tst_QArray : public QObject
{
    Q_OBJECT
public:
    tst_QArray() {}
    ~tst_QArray() {}

private slots:
    void create();
    void append();
    void appendTwoAtATime();
    void appendThreeAtATime();
    void appendFourAtATime();
    void appendArray();
    void setAt();
    void value();
    void replace();
    void copy();
    void resize();
    void reserve();
    void squeeze();
    void compare();
    void remove();
    void removeFirstLast();
    void extend();
    void reverse();
    void reversed();
    void mid();
    void left();
    void right();
    void arrayRef();
    void iterate();
    void copyPrealloc();
    void insert();
    void fromRawData();
    void fromWritableRawData();
    void search();
    void fill();
    void unsharedArray();

    // QVarLengthArray simulation tests.
    void QVarLengthArray_append();
    void QVarLengthArray_removeLast();
    void QVarLengthArray_oldTests();
    void QVarLengthArray_task214223();

    // QVector simulation tests.
    void QVector_outOfMemory();
    void QVector_QTBUG6416_reserve();

    // QList simulation tests.
    void QList_length() const;
    void QList_lengthSignature() const;
    void QList_append() const;
};

// This must match the default for PreallocSize.
static const int ExpectedMinCapacity = 8;

// Complex type that helps the tests determine if QArray is calling
// constructors, destructors, and copy constructors in the right places.
class ComplexValue
{
public:
    enum Mode
    {
        Default,
        Init,
        Copy,
        CopiedAgain,
        Assign
    };

    static int destroyCount;

    ComplexValue() : m_value(-1), m_mode(Default) {}
    ComplexValue(int value) : m_value(value), m_mode(Init) {}
    ComplexValue(const ComplexValue& other)
        : m_value(other.m_value)
    {
        if (other.m_mode == Copy || other.m_mode == CopiedAgain)
            m_mode = CopiedAgain;
        else
            m_mode = Copy;
    }
    ~ComplexValue() { ++destroyCount; }

    ComplexValue& operator=(const ComplexValue& other)
        { m_value = other.m_value; m_mode = Assign; return *this; }

    int value() const { return m_value; }
    Mode mode() const { return m_mode; }

    bool operator==(const ComplexValue& other) const
        { return m_value == other.m_value; }
    bool operator==(int other) const
        { return m_value == other; }
    bool operator!=(const ComplexValue& other) const
        { return m_value != other.m_value; }
    bool operator!=(int other) const
        { return m_value != other; }

private:
    int m_value;
    Mode m_mode;
};

int ComplexValue::destroyCount = 0;

void tst_QArray::create()
{
    QArray<float> array;

    // Check the basic properties.
    QVERIFY(array.isEmpty());
    QCOMPARE(array.count(), 0);
    QCOMPARE(array.size(), 0);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
    QVERIFY(array.constData() != 0);
    QVERIFY(array.data() == array.constData());
    QVERIFY(((const QArray<float> *)&array)->data() == array.constData());

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
    QVERIFY(((const QArray<float> *)&array)->data() == array.constData());

    // Create another array that is filled with an initial value.
    QArray<QVector3D> array2(100, QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(!array2.isEmpty());
    QCOMPARE(array2.count(), 100);
    QCOMPARE(array2.size(), 100);
    QVERIFY(array2.capacity() >= 100);
    QVERIFY(array2.constData() != 0);
    QVERIFY(array2.data() == array2.constData());
    QVERIFY(((const QArray<QVector3D> *)&array2)->data() == array2.constData());
    for (int index = 0; index < 100; ++index)
        QVERIFY(array2.at(index) == QVector3D(1.0f, 2.0f, 3.0f));

    // Create a filled array that is within the prealloc area.
    QArray<float> array3(ExpectedMinCapacity, 42.5f);
    QVERIFY(!array3.isEmpty());
    QCOMPARE(array3.count(), ExpectedMinCapacity);
    QCOMPARE(array3.size(), ExpectedMinCapacity);
    QVERIFY(array3.capacity() == ExpectedMinCapacity);
    QVERIFY(array3.constData() != 0);
    QVERIFY(array3.data() == array3.constData());
    for (int index = 0; index < ExpectedMinCapacity; ++index)
        QVERIFY(array3.at(index) == 42.5f);

    // Create an array of strings.
    QArray<QString> array4;
    QCOMPARE(array4.size(), 0);
    array4.append(QLatin1String("foo"));
    array4.append(QLatin1String("bar"));
    array4.append(array4[0]);
    QCOMPARE(array4[0], QLatin1String("foo"));
    QCOMPARE(array4[1], QLatin1String("bar"));
    QCOMPARE(array4[2], QLatin1String("foo"));

    // Create an array of complex values and check that the
    // copy constructors were called correctly.  Also check that
    // the destructors are called when the array is destroyed.
    ComplexValue::destroyCount = 0;
    {
        QArray<ComplexValue> array5;
        array5.append(ComplexValue(1));
        array5.append(ComplexValue(2));
        array5.append(ComplexValue(3));
        QCOMPARE(ComplexValue::destroyCount, 3); // Destruction of temporaries.
        QCOMPARE(array5.size(), 3);
        QVERIFY(array5[0].value() == 1);
        QVERIFY(array5[0].mode()  == ComplexValue::Copy);
        QVERIFY(array5[1].value() == 2);
        QVERIFY(array5[1].mode()  == ComplexValue::Copy);
        QVERIFY(array5[2].value() == 3);
        QVERIFY(array5[2].mode()  == ComplexValue::Copy);
    }
    QCOMPARE(ComplexValue::destroyCount, 6);

    // Check the zero-prealloc case.
    QArray<float, 0> array6;
    QCOMPARE(array6.size(), 0);
    array6.append(1.0f);
    QCOMPARE(array6.size(), 1);
    QCOMPARE(array6[0], 1.0f);
    QArray<float, 0> array7(array6);
    QVERIFY(array6.constData() == array7.constData());
}

void tst_QArray::append()
{
    QArray<float> array;
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
    QVERIFY(array.isDetached());
    QArray<float> array2(array);
    QVERIFY(!array.isDetached());
    QVERIFY(!array2.isDetached());
    QCOMPARE(array2.count(), ExpectedMinCapacity + 1 + 1000);
    QCOMPARE(array2.size(), array2.count());
    QVERIFY(!array2.isEmpty());
    QVERIFY(array2.capacity() >= array2.size());
    QArray<float> array3;
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
    QVERIFY(array.isDetached());
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
        QCOMPARE(array.at(index), float(index));
        QCOMPARE(array2.at(index), float(index));
        QCOMPARE(array3.at(index), float(index));
    }
    QCOMPARE(array.at(ExpectedMinCapacity), 1000.0f);
    QCOMPARE(array2.at(ExpectedMinCapacity), 1000.0f);
    QCOMPARE(array3.at(ExpectedMinCapacity), 1000.0f);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array.at(index + ExpectedMinCapacity + 1), float(index));
    }
    QCOMPARE(array[ExpectedMinCapacity + 1000 + 1], 1500.0f);

    // Check the detach conditions.
    QVERIFY(!array2.isDetached());
    QVERIFY(!array3.isDetached());
    array3.append(1.0f);
    QVERIFY(array3.isDetached());
    QVERIFY(!array2.isDetached()); // Still thinks it is shared.
    array2.data();
    QVERIFY(array2.isDetached());  // Now knows that it isn't.

    // Create a large array of strings.
    QArray<QString> array4;
    for (index = 0; index < 1000; ++index)
        array4.append(QString::number(index));
    QCOMPARE(array4.size(), 1000);
    for (index = 0; index < 1000; ++index)
        QVERIFY(array4[index] == QString::number(index));

    // Make a copy of the array of strings and then force a detach.
    QArray<QString> array5(array4);
    QCOMPARE(array4.size(), 1000);
    QCOMPARE(array5.size(), 1000);
    for (index = 0; index < 1000; ++index) {
        QVERIFY(array4[index] == QString::number(index));
        QVERIFY(array5[index] == QString::number(index));
    }
    array5.append(QString::number(1000));
    QCOMPARE(array4.size(), 1000);
    QCOMPARE(array5.size(), 1001);
    for (index = 0; index < 1000; ++index) {
        QVERIFY(array4[index] == QString::number(index));
        QVERIFY(array5[index] == QString::number(index));
    }
    QVERIFY(array5[1000] == QString::number(1000));

    // Create an array of complex values and force one realloc
    // to test that copy constructors and destructors are called
    // when moving data from the prealloc array to the heap.
    QArray<ComplexValue> array6;
    ComplexValue::destroyCount = 0;
    for (index = 0; index < ExpectedMinCapacity; ++index)
        array6.append(ComplexValue(index));
    QCOMPARE(ComplexValue::destroyCount, ExpectedMinCapacity);
    ComplexValue::destroyCount = 0;
    array6.append(ComplexValue(ExpectedMinCapacity));
    QCOMPARE(ComplexValue::destroyCount, ExpectedMinCapacity + 1);
    for (index = 0; index < (ExpectedMinCapacity + 1); ++index) {
        QCOMPARE(array6[index].value(), index);
        // The last element should be Copy, but all others are CopiedAgain.
        if (index == ExpectedMinCapacity)
            QVERIFY(array6[index].mode() == ComplexValue::Copy);
        else
            QVERIFY(array6[index].mode() == ComplexValue::CopiedAgain);
    }

    // Force another realloc to test heap to heap copies.
    int capacity = array6.capacity();
    for (int index = array6.size(); index < capacity; ++index)
        array6.append(ComplexValue(index));
    ComplexValue::destroyCount = 0;
    array6.append(ComplexValue(capacity));
    QCOMPARE(ComplexValue::destroyCount, capacity + 1);
    for (index = 0; index < (capacity + 1); ++index) {
        QCOMPARE(array6[index].value(), index);
        // The last element should be Copy, but all others are CopiedAgain.
        if (index == capacity)
            QVERIFY(array6[index].mode() == ComplexValue::Copy);
        else
            QVERIFY(array6[index].mode() == ComplexValue::CopiedAgain);
    }

    // Make a copy of array6 and force a detach.
    int size = array6.size();
    QArray<ComplexValue> array7(array6);
    QCOMPARE(array6.size(), size);
    QCOMPARE(array7.size(), size);
    for (index = 0; index < size; ++index) {
        QVERIFY(array6[index].value() == index);
        QVERIFY(array7[index].value() == index);
    }
    array7.append(ComplexValue(size));
    QCOMPARE(array6.size(), size);
    QCOMPARE(array7.size(), size + 1);
    for (index = 0; index < size; ++index) {
        QVERIFY(array6[index].value() == index);
        QVERIFY(array7[index].value() == index);
    }
    QVERIFY(array7[size].value() == size);

    // Make another copy using operator=.
    QArray<ComplexValue> array8;
    QCOMPARE(array8.size(), 0);
    array8 = array6;
    QCOMPARE(array6.size(), size);
    QCOMPARE(array8.size(), size);
    for (index = 0; index < size; ++index) {
        QVERIFY(array6[index].value() == index);
        QVERIFY(array8[index].value() == index);
    }
    array8.append(ComplexValue(size));
    QCOMPARE(array6.size(), size);
    QCOMPARE(array8.size(), size + 1);
    for (index = 0; index < size; ++index) {
        QVERIFY(array6[index].value() == index);
        QVERIFY(array8[index].value() == index);
    }
    QVERIFY(array8[size].value() == size);

    // Copy the same object over itself.
    QArray<ComplexValue> array9(array8);
    QVERIFY(array9.constData() == array8.constData());
    for (index = 0; index < array8.size(); ++index)
        QCOMPARE((*((const QArray<ComplexValue> *)&array9))[index],
                 array8.at(index));
    array9 = array8;
    QVERIFY(array9.constData() == array8.constData());
    for (index = 0; index < array8.size(); ++index)
        QCOMPARE(array9.at(index), array8.at(index));
}

void tst_QArray::appendTwoAtATime()
{
    int index;

    QArray<float> array;
    array.append(1.0f, 2.0f);
    array.append(3.0f, 4.0f);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);
    QCOMPARE(array.count(), 4);

    QArray<float> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(float(index), float(index + 1));
    QCOMPARE(array2.count(), 2000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index * 2], float(index));
        QCOMPARE(array2[index * 2 + 1], float(index + 1));
    }

    QArray<QString> array3;
    for (index = 0; index < 1000; ++index)
        array3.append(QString::number(index), QString::number(index + 1));
    QCOMPARE(array3.count(), 2000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array3[index * 2], QString::number(index));
        QCOMPARE(array3[index * 2 + 1], QString::number(index + 1));
    }

    QArray<ComplexValue> array4;
    for (index = 0; index < 1000; ++index)
        array4.append(ComplexValue(index), ComplexValue(index + 1));
    QCOMPARE(array4.count(), 2000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array4[index * 2].value(), index);
        QCOMPARE(array4[index * 2 + 1].value(), index + 1);
    }
}

void tst_QArray::appendThreeAtATime()
{
    int index;

    QArray<float> array;
    array.append(1.0f, 2.0f, 3.0f);
    array.append(4.0f, 5.0f, 6.0f);
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    QCOMPARE(array[2], 3.0f);
    QCOMPARE(array[3], 4.0f);
    QCOMPARE(array[4], 5.0f);
    QCOMPARE(array[5], 6.0f);
    QCOMPARE(array.count(), 6);

    QArray<float> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(float(index), float(index + 1), float(index + 2));
    QCOMPARE(array2.count(), 3000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index * 3], float(index));
        QCOMPARE(array2[index * 3 + 1], float(index + 1));
        QCOMPARE(array2[index * 3 + 2], float(index + 2));
    }

    QArray<QString> array3;
    for (index = 0; index < 1000; ++index)
        array3.append(QString::number(index), QString::number(index + 1),
                      QString::number(index + 2));
    QCOMPARE(array3.count(), 3000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array3[index * 3], QString::number(index));
        QCOMPARE(array3[index * 3 + 1], QString::number(index + 1));
        QCOMPARE(array3[index * 3 + 2], QString::number(index + 2));
    }

    QArray<ComplexValue> array4;
    for (index = 0; index < 1000; ++index)
        array4.append(ComplexValue(index), ComplexValue(index + 1),
                      ComplexValue(index + 2));
    QCOMPARE(array4.count(), 3000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array4[index * 3].value(), index);
        QCOMPARE(array4[index * 3 + 1].value(), index + 1);
        QCOMPARE(array4[index * 3 + 2].value(), index + 2);
    }
}

void tst_QArray::appendFourAtATime()
{
    int index;

    QArray<float> array;
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

    QArray<float> array2;
    for (index = 0; index < 1000; ++index)
        array2.append(float(index), float(index + 1), float(index + 2), float(index + 3));
    QCOMPARE(array2.count(), 4000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array2[index * 4], float(index));
        QCOMPARE(array2[index * 4 + 1], float(index + 1));
        QCOMPARE(array2[index * 4 + 2], float(index + 2));
        QCOMPARE(array2[index * 4 + 3], float(index + 3));
    }

    QArray<QString> array3;
    for (index = 0; index < 1000; ++index)
        array3.append(QString::number(index), QString::number(index + 1),
                      QString::number(index + 2), QString::number(index + 3));
    QCOMPARE(array3.count(), 4000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array3[index * 4], QString::number(index));
        QCOMPARE(array3[index * 4 + 1], QString::number(index + 1));
        QCOMPARE(array3[index * 4 + 2], QString::number(index + 2));
        QCOMPARE(array3[index * 4 + 3], QString::number(index + 3));
    }

    QArray<ComplexValue> array4;
    for (index = 0; index < 1000; ++index)
        array4.append(ComplexValue(index), ComplexValue(index + 1),
                      ComplexValue(index + 2), ComplexValue(index + 3));
    QCOMPARE(array4.count(), 4000);
    for (index = 0; index < 1000; ++index) {
        QCOMPARE(array4[index * 4].value(), index);
        QCOMPARE(array4[index * 4 + 1].value(), index + 1);
        QCOMPARE(array4[index * 4 + 2].value(), index + 2);
        QCOMPARE(array4[index * 4 + 3].value(), index + 3);
    }
}

void tst_QArray::appendArray()
{
    QArray<float> array;
    QArray<float> array2;
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

    array2 += (QArray<float>());
    QCOMPARE(array2.count(), 2000);

    QArray<float> array3(array2.constData(), array2.size());
    QCOMPARE(array3.size(), array2.size());
    for (index = 0; index < array2.size(); ++index)
        QCOMPARE(array3.at(index), array2.at(index));
}

void tst_QArray::setAt()
{
    int index;

    QArray<float> array;
    array.append(1.0f);
    QCOMPARE(array[0], 1.0f);

    array[0] = 6.0f;
    QCOMPARE(array[0], 6.0f);

    QArray<float> array2;
    for(index = 0; index < 1000; ++index)
        array2.append(index);
    for(index = 0; index < 1000; ++index)
        array2[index] = index + 1000;
    for(index = 0; index < 1000; ++index)
        QCOMPARE(array2[index], float(index + 1000));
    for(index = 0; index < 1000; ++index)
        array2.replace(index, float(-index));
    for(index = 0; index < 1000; ++index)
        QCOMPARE(array2[index], float(-index));
}

void tst_QArray::value()
{
    QArray<float> array;
    for (int index = 0; index < 1000; ++index)
        array.append(float(index));

    for (int index = 0; index < 1000; ++index) {
        QCOMPARE(array.value(index), float(index));
        QCOMPARE(array.value(index, 10001.0f), float(index));
    }

    QCOMPARE(array.value(-1), 0.0f);
    QCOMPARE(array.value(1000), 0.0f);

    QCOMPARE(array.value(-1, 10001.0f), 10001.0f);
    QCOMPARE(array.value(1000, 10001.0f), 10001.0f);
}

void tst_QArray::replace()
{
    QArray<float> array;
    QArray<float> array2;
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

    QArray<ComplexValue> array3;
    QArray<ComplexValue> array4;
    for (index = 0; index < 1000; ++index) {
        array3.append(ComplexValue(index));
        array4.append(ComplexValue(1000 - index));
    }
    array3.replace(0, array4.constData(), array4.size());
    for (index = 0; index < 1000; ++index)
        QVERIFY(array3[index] == array4[index]);
}

// Exercise the various conditionals in operator=().
void tst_QArray::copy()
{
    QArray<float> array, array2, array3;

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

    QArray<float> array4(array2);
    QCOMPARE(array2.count(), 2);
    QCOMPARE(array2.at(0), float(1.0f));
    QCOMPARE(array2.at(1), float(7.0f));

    QArray<float> array5, array6;
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

void tst_QArray::resize()
{
    QArray<int> array;

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

    QArray<int> array2(array);
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

void tst_QArray::reserve()
{
    QArray<float> array;
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

void tst_QArray::squeeze()
{
    QArray<float> array;
    array.reserve(100);
    QVERIFY(array.isEmpty());
    QVERIFY(array.capacity() >= 100);

    for (int index = 0; index < 100; ++index)
        array.append(float(index));

    array.reserve(400);
    QVERIFY(array.capacity() >= 400);

    array.squeeze();
    QCOMPARE(array.capacity(), 100);
    QCOMPARE(array.count(), 100);

    // Test squeezing within the preallocated area.
    QArray<float> array2;
    array2.append(1.0f);
    array2.append(2.0f);
    array2.append(3.0f);
    array2.squeeze();
    QCOMPARE(array2.capacity(), ExpectedMinCapacity);
    QCOMPARE(array2.count(), 3);

    // Test copy-on-write during squeezing.
    QArray<float> array3(array);
    array3.squeeze();
    QCOMPARE(array3.count(), 100);
    QCOMPARE(array.count(), 100);

    // Clear and check that the array reverts to preallocation.
    array.resize(0);
    array.squeeze();
    QCOMPARE(array.size(), 0);
    QCOMPARE(array.capacity(), ExpectedMinCapacity);
}

void tst_QArray::compare()
{
    QArray<float> array, array2, array3;

    QVERIFY(array == array2);

    array.append(1.0f);
    array3.append(1.0f);
    QArray<float> array4(array);

    QVERIFY(array == array);
    QVERIFY(array != array2);
    QVERIFY(array == array3);
    QVERIFY(array == array4);

    array2.append(2.0f);
    QVERIFY(array != array2);

    array2.append(1.0f);
    QVERIFY(array != array2);

    for (int index = 0; index < 100; ++index)
        array.append(index);
    array2 = array;
    QVERIFY(array2 == array);
    QVERIFY(!(array2 != array));
}

void tst_QArray::remove()
{
    QArray<float> array;

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

    QArray<float> array2(array);
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

    for (int index = 0; index < 100; ++index)
        array.append(float(index));
    array.erase(array.end() - 1);
    QCOMPARE(array.size(), 99);
    array.erase(array.begin(), array.end());
    QCOMPARE(array.count(), 0);
}

void tst_QArray::removeFirstLast()
{
    QArray<float> array;

    array.removeFirst();
    QVERIFY(array.isEmpty());
    array.removeLast();
    QVERIFY(array.isEmpty());

    for (int index = 0; index < 100; ++index)
        array.append(float(index));

    array.removeFirst();
    QCOMPARE(array.size(), 99);
    for (int index = 0; index < array.size(); ++index)
        QCOMPARE(array[index], float(index + 1));

    array.removeLast();
    QCOMPARE(array.size(), 98);
    for (int index = 0; index < array.size(); ++index)
        QCOMPARE(array[index], float(index + 1));
}

void tst_QArray::extend()
{
    QArray<float> array;

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

    QArray<float> array2(array);

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

void tst_QArray::reverse()
{
    QArray<float> array0;

    // null case
    array0.reverse();
    QCOMPARE(array0.size(), 0);

    // basic case
    float *ptr = array0.extend(1);
    ptr[0] = 1.0f;
    array0.reverse();
    QCOMPARE(array0.size(), 1);
    QCOMPARE(array0.at(0), 1.0f);

    // odd numbered size
    QArray<float> array1;
    ptr = array1.extend(5);
    ptr[0] = 1.0f;
    ptr[1] = 2.0f;
    ptr[2] = 3.0f;
    ptr[3] = 4.0f;
    ptr[4] = 5.0f;
    array1.reverse();
    QCOMPARE(array1.size(), 5);
    QCOMPARE(array1.at(0), 5.0f);
    QCOMPARE(array1.at(2), 3.0f);
    QCOMPARE(array1.at(4), 1.0f);

    // even numbered size
    QArray<float> array2;
    ptr = array2.extend(6);
    ptr[0] = 1.0f;
    ptr[1] = 2.0f;
    ptr[2] = 3.0f;
    ptr[3] = 4.0f;
    ptr[4] = 5.0f;
    ptr[5] = 6.0f;
    array2.reverse();
    QCOMPARE(array2.size(), 6);
    QCOMPARE(array2.at(0), 6.0f);
    QCOMPARE(array2.at(2), 4.0f);
    QCOMPARE(array2.at(3), 3.0f);
    QCOMPARE(array2.at(5), 1.0f);

    // complex type
    QVERIFY(QTypeInfo<QString>::isComplex);
    QArray<QString> array3(6, QString("test"));
    array3[0] = "zero";
    array3[1] = "one";
    array3[2] = "two";
    array3[3] = "three";
    array3[4] = "four";
    array3[5] = "five";
    array3.reverse();
    QCOMPARE(array3.size(), 6);
    QCOMPARE(array3.at(0), QString("five"));
    QCOMPARE(array3.at(2), QString("three"));
    QCOMPARE(array3.at(3), QString("two"));
    QCOMPARE(array3.at(5), QString("zero"));
}

void tst_QArray::reversed()
{
    QArray<float> array0;

    // null case
    QArray<float> res0 = array0.reversed();
    QCOMPARE(res0.size(), 0);

    // basic case
    float *ptr = array0.extend(1);
    ptr[0] = 1.0f;
    res0 = array0.reversed();
    QCOMPARE(res0.size(), 1);
    QCOMPARE(res0.at(0), 1.0f);

    // odd numbered size
    QArray<float> array1;
    ptr = array1.extend(5);
    ptr[0] = 1.0f;
    ptr[1] = 2.0f;
    ptr[2] = 3.0f;
    ptr[3] = 4.0f;
    ptr[4] = 5.0f;
    QArray<float> res1 = array1.reversed();
    QCOMPARE(res1.size(), 5);
    QCOMPARE(res1.at(0), 5.0f);
    QCOMPARE(res1.at(2), 3.0f);
    QCOMPARE(res1.at(4), 1.0f);

    // even numbered size
    QArray<float> array2;
    ptr = array2.extend(6);
    ptr[0] = 1.0f;
    ptr[1] = 2.0f;
    ptr[2] = 3.0f;
    ptr[3] = 4.0f;
    ptr[4] = 5.0f;
    ptr[5] = 6.0f;
    QArray<float> res2 = array2.reversed();
    QCOMPARE(res2.size(), 6);
    QCOMPARE(res2.at(0), 6.0f);
    QCOMPARE(res2.at(2), 4.0f);
    QCOMPARE(res2.at(3), 3.0f);
    QCOMPARE(res2.at(5), 1.0f);

    // simple type with larger size
    QArray<QVector3D> array4;
    array4.extend(6);
    QVector3D va(1.0f, 2.0f, 3.0f);
    QVector3D vb(11.0f, 12.0f, 13.0f);
    QVector3D vc(21.0f, 22.0f, 23.0f);
    QVector3D vd(31.0f, 32.0f, 33.0f);
    QVector3D ve(41.0f, 42.0f, 43.0f);
    QVector3D vf(51.0f, 52.0f, 53.0f);
    array4[0] = va;
    array4[1] = vb;
    array4[2] = vc;
    array4[3] = vd;
    array4[4] = ve;
    array4[5] = vf;
    QArray<QVector3D> res4 = array4.reversed();
    QCOMPARE(res4.size(), 6);
    QCOMPARE(res4.at(0), vf);
    QCOMPARE(res4.at(2), vd);
    QCOMPARE(res4.at(3), vc);
    QCOMPARE(res4.at(5), va);
}

void tst_QArray::mid()
{
    QArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    // Check the null case.
    QArrayRef<float> mid;
    QCOMPARE(mid.size(), 0);
    QCOMPARE(mid.count(), 0);
    QCOMPARE(mid.offset(), 0);
    QVERIFY(mid.isEmpty());
    QVERIFY(mid.isNull());
    QVERIFY(mid.data() == 0);
    QVERIFY(mid.constData() == 0);

    QArray<float> midarray = mid.toArray();
    QVERIFY(midarray.isEmpty());

    mid = array.mid(0);
    QCOMPARE(mid.offset(), 0);
    QCOMPARE(mid.size(), array.size());
    QVERIFY(!mid.isEmpty());
    QVERIFY(!mid.isNull());
    QVERIFY(mid.data() == array.data());
    QVERIFY(mid.constData() == array.constData());
    QVERIFY(mid.array() == &array);
    for (int index = 0; index < 1024; ++index) {
        QCOMPARE(mid.at(index), float(index));
        QCOMPARE(mid[index], float(index));
    }

    midarray = mid.toArray();
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
    QVERIFY(mid.array() == &array);
    for (int index = 0; index < 20; ++index) {
        QCOMPARE(mid.at(index), float(index + 500));
        QCOMPARE(mid[index], float(index + 500));
    }

    mid[10] = 0.0f;
    QCOMPARE(array.at(510), float(0.0f));
    mid[10] = 510.0f;
    QCOMPARE(array.at(510), float(510.0f));

    QArrayRef<float> mid2(&array, 500, 20);
    QVERIFY(mid == mid2);
    QVERIFY(!(mid != mid2));

    QArrayRef<float> mid3(&array, 500, 21);
    QVERIFY(mid != mid3);
    QVERIFY(!(mid == mid3));

    QArrayRef<float> mid4(&array, 400, 20);
    QVERIFY(mid != mid4);
    QVERIFY(!(mid == mid4));

    QArrayRef<float> mid5;
    QArrayRef<float> mid6;
    QVERIFY(mid != mid5);
    QVERIFY(!(mid == mid5));
    QVERIFY(mid5 != mid);
    QVERIFY(!(mid5 == mid));
    QVERIFY(mid5 == mid6);
    QVERIFY(!(mid5 != mid6));

    midarray = mid.toArray();
    QCOMPARE(midarray.size(), mid.size());
    for (int index = 0; index < mid.size(); ++index)
        QCOMPARE(midarray[index], mid[index]);

    QArray<float> midarray2(mid.toArray());
    QVERIFY(midarray == midarray2);

    mid = array.mid(500, 0);
    QCOMPARE(mid.offset(), 500);
    QCOMPARE(mid.size(), 0);
    QVERIFY(mid.isEmpty());

    midarray = mid.toArray();
    QVERIFY(midarray.isEmpty());

    mid = array.mid(1000, 30);
    QCOMPARE(mid.offset(), 1000);
    QCOMPARE(mid.size(), 24);
    QVERIFY(!mid.isEmpty());
    QVERIFY(!mid.isNull());
    QVERIFY(mid.data() == (array.data() + 1000));
    QVERIFY(mid.constData() == (array.constData() + 1000));
    QVERIFY(mid.array() == &array);
    for (int index = 0; index < 24; ++index) {
        QCOMPARE(mid.at(index), float(index + 1000));
        QCOMPARE(mid[index], float(index + 1000));
    }

    // Constructor should act like mid().
    QArrayRef<float> mid7(&array, 1000, 30);
    QCOMPARE(mid7.offset(), 1000);
    QCOMPARE(mid7.size(), 24);
    QVERIFY(!mid7.isEmpty());
    QVERIFY(!mid7.isNull());
    QVERIFY(mid7.data() == (array.data() + 1000));
    QVERIFY(mid7.constData() == (array.constData() + 1000));
    QVERIFY(mid7.array() == &array);
    for (int index = 0; index < 24; ++index) {
        QCOMPARE(mid7.at(index), float(index + 1000));
        QCOMPARE(mid7[index], float(index + 1000));
    }

    QArrayRef<float> mid8(&array);
    QCOMPARE(mid8.offset(), 0);
    QCOMPARE(mid8.size(), array.size());
    QVERIFY(!mid8.isEmpty());
    QVERIFY(!mid8.isNull());
    QVERIFY(mid8.data() == array.data());
    QVERIFY(mid8.constData() == array.constData());
    QVERIFY(mid8.array() == &array);
    for (int index = 0; index < 1024; ++index) {
        QCOMPARE(mid8.at(index), float(index));
        QCOMPARE(mid8[index], float(index));
    }

    QArrayRef<float> mid9(0);
    QCOMPARE(mid9.offset(), 0);
    QCOMPARE(mid9.size(), 0);
    QVERIFY(mid9.isEmpty());
    QVERIFY(mid9.isNull());
    QVERIFY(mid9.data() == 0);
    QVERIFY(mid9.constData() == 0);
    QVERIFY(mid9.array() == 0);
}

void tst_QArray::left()
{
    QArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    QArrayRef<float> left = array.left(-1);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 1024);
    QVERIFY(left.array() == &array);

    left = array.left(0);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 0);
    QVERIFY(left.array() == &array);

    left = array.left(500);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 500);
    QVERIFY(left.array() == &array);

    left = array.left(2048);
    QCOMPARE(left.offset(), 0);
    QCOMPARE(left.size(), 1024);
    QVERIFY(left.array() == &array);
}

void tst_QArray::right()
{
    QArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    QArrayRef<float> right = array.right(-1);
    QCOMPARE(right.offset(), 0);
    QCOMPARE(right.size(), 1024);
    QVERIFY(right.array() == &array);

    right = array.right(0);
    QCOMPARE(right.offset(), 1024);
    QCOMPARE(right.size(), 0);
    QVERIFY(right.array() == &array);

    right = array.right(500);
    QCOMPARE(right.offset(), 1024 - 500);
    QCOMPARE(right.size(), 500);
    QVERIFY(right.array() == &array);

    right = array.right(2048);
    QCOMPARE(right.offset(), 0);
    QCOMPARE(right.size(), 1024);
    QVERIFY(right.array() == &array);
}

void tst_QArray::arrayRef()
{
    QArray<float> array;
    QArray<float> array3;
    for (int index = 0; index < 1024; ++index) {
        array.append(float(index));
        array3.append(float(index));
    }

    QArrayRef<float> ref = array.mid(100, 50);
    QVERIFY(ref.array() == &array);
    QCOMPARE(ref.offset(), 100);
    QCOMPARE(ref.size(), 50);

    QArray<float> array2(ref);
    QCOMPARE(array2.size(), 50);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], array[index + 100]);

    array2 = array.mid(50, 100);
    QCOMPARE(array2.size(), 100);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], array[index + 50]);

    array2 += array.mid(150, 20);
    QCOMPARE(array2.size(), 120);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], array[index + 50]);

    array2 << array3.mid(170, 30);
    QCOMPARE(array2.size(), 150);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], array[index + 50]);

    array2 = array2.mid(10);
    QCOMPARE(array2.size(), 140);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], array[index + 60]);

    array2.append(array2.mid(0));
    QCOMPARE(array2.size(), 280);
    for (int index = 0; index < 140; ++index) {
        QCOMPARE(array2[index], array[index + 60]);
        QCOMPARE(array2[index + 140], array[index + 60]);
    }
}

void tst_QArray::iterate()
{
    QArray<float> array;
    for (int index = 0; index < 1024; ++index)
        array.append(float(index));

    QArray<float>::Iterator it1;
    int value = 0;
    for (it1 = array.begin(); it1 != array.end(); ++it1)
        QCOMPARE(*it1, float(value++));
    QCOMPARE(value, array.size());

    QArray<float>::ConstIterator it2;
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

    QArrayRef<float> mid = array.mid(512, 256);
    QArrayRef<float>::Iterator it3;
    value = 512;
    for (it3 = mid.begin(); it3 != mid.end(); ++it3)
        QCOMPARE(*it3, float(value++));
    QCOMPARE(value - 512, mid.size());

    QArrayRef<float>::Iterator it4;
    value = 512;
    for (it4 = mid.begin(); it4 != mid.end(); ++it4)
        *it4 = float(1024 - value++);

    QArrayRef<float>::ConstIterator it5;
    value = 512;
    for (it5 = mid.constBegin(); it5 != mid.constEnd(); ++it5)
        QCOMPARE(*it5, float(1024 - value++));
    QCOMPARE(value - 512, mid.size());
}

// Verify that when the data is in the preallocated section, it is
// copied across and the original constData() pointer remains the same.
void tst_QArray::copyPrealloc()
{
    QArray<float> array1;
    array1.append(1.0f);
    array1.append(2.0f);

    const float *data = array1.constData();

    QArray<float> array2(array1);

    QVERIFY(array1.constData() == data);
    QVERIFY(array2.constData() != data);

    QCOMPARE(array2.size(), 2);
    QCOMPARE(array2[0], float(1.0f));
    QCOMPARE(array2[1], float(2.0f));

    QArray<float> array3;
    QCOMPARE(array3.size(), 0);
    array3 = array1;

    QVERIFY(array1.constData() == data);
    QVERIFY(array3.constData() != data);

    QCOMPARE(array3.size(), 2);
    QCOMPARE(array3[0], float(1.0f));
    QCOMPARE(array3[1], float(2.0f));
}

void tst_QArray::insert()
{
    QArray<float> array;
    for (int index = 0; index < 10; ++index)
        array.append(float(index));

    array.prepend(-1.0f);
    QCOMPARE(array.size(), 11);
    for (int index = 0; index < array.size(); ++index)
        QCOMPARE(array[index], float(index - 1));

    array.insert(array.size(), 10.0f);
    QCOMPARE(array.size(), 12);
    for (int index = 0; index < array.size(); ++index)
        QCOMPARE(array[index], float(index - 1));

    array.insert(1, 0.5f);
    QCOMPARE(array.size(), 13);
    QCOMPARE(array[0], -1.0f);
    QCOMPARE(array[1], 0.5f);
    QCOMPARE(array[2], 0.0f);
    QCOMPARE(array[12], 10.0f);

    array.insert(10, 0, 7.5f);
    array.insert(10, 4, 7.5f);
    QCOMPARE(array.size(), 17);
    QCOMPARE(array[9], 7.0f);
    QCOMPARE(array[10], 7.5f);
    QCOMPARE(array[11], 7.5f);
    QCOMPARE(array[12], 7.5f);
    QCOMPARE(array[13], 7.5f);
    QCOMPARE(array[14], 8.0f);
    QCOMPARE(array[15], 9.0f);
    QCOMPARE(array[16], 10.0f);

    // Repeat the tests with QString
    QArray<QString> array2;
    for (int index = 0; index < 10; ++index)
        array2.append(QString::number(index));

    array2.prepend(QString::number(-1));
    QCOMPARE(array2.size(), 11);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], QString::number(index - 1));

    array2.insert(array2.size(), QString::number(10));
    QCOMPARE(array2.size(), 12);
    for (int index = 0; index < array2.size(); ++index)
        QCOMPARE(array2[index], QString::number(index - 1));

    array2.insert(1, QString::number(5));
    QCOMPARE(array2.size(), 13);
    QCOMPARE(array2[0], QString::number(-1));
    QCOMPARE(array2[1], QString::number(5));
    QCOMPARE(array2[2], QString::number(0));
    QCOMPARE(array2[12], QString::number(10));

    array2.insert(10, 4, QString::number(3));
    QCOMPARE(array2.size(), 17);
    QCOMPARE(array2[9], QString::number(7));
    QCOMPARE(array2[10], QString::number(3));
    QCOMPARE(array2[11], QString::number(3));
    QCOMPARE(array2[12], QString::number(3));
    QCOMPARE(array2[13], QString::number(3));
    QCOMPARE(array2[14], QString::number(8));
    QCOMPARE(array2[15], QString::number(9));
    QCOMPARE(array2[16], QString::number(10));

    // Repeat the tests with ComplexValue
    QArray<ComplexValue> array3;
    for (int index = 0; index < 10; ++index)
        array3.append(ComplexValue(index));

    array3.prepend(ComplexValue(-1));
    ComplexValue::destroyCount = 0;
    QCOMPARE(array3.size(), 11);
    for (int index = 0; index < array3.size(); ++index) {
        QVERIFY(array3[index] == (index - 1));
        QVERIFY(array3[index].mode() == ComplexValue::Assign);
    }
    QCOMPARE(ComplexValue::destroyCount, 0);

    array3.insert(array3.size(), ComplexValue(10));
    QCOMPARE(array3.size(), 12);
    for (int index = 0; index < array3.size(); ++index)
        QVERIFY(array3[index] == (index - 1));

    array3.insert(1, ComplexValue(5));
    QCOMPARE(array3.size(), 13);
    QVERIFY(array3[0] == -1);
    QVERIFY(array3[1] == 5);
    QVERIFY(array3[2] == 0);
    QVERIFY(array3[12] == 10);

    array3.insert(10, 4, ComplexValue(3));
    QCOMPARE(array3.size(), 17);
    QVERIFY(array3[9] == 7);
    QVERIFY(array3[10] == 3);
    QVERIFY(array3[11] == 3);
    QVERIFY(array3[12] == 3);
    QVERIFY(array3[13] == 3);
    QVERIFY(array3[14] == 8);
    QVERIFY(array3[15] == 9);
    QVERIFY(array3[16] == 10);
}

void tst_QArray::fromRawData()
{
    QArray<float> array;
    float contents[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f,
                        7.0f, 8.0f, 9.0f, 10.0f, 11.0f, 12.0f};

    array = QArray<float>::fromRawData(contents, 0);
    QCOMPARE(array.size(), 0);
    QCOMPARE(array.capacity(), 0);
    QVERIFY(!array.isDetached());
    array.append(1.0f);
    QCOMPARE(array.size(), 1);
    QVERIFY(array.capacity() > 0);
    QCOMPARE(array.at(0), 1.0f);
    QVERIFY(array.isDetached());

    array = QArray<float>::fromRawData(contents, 6);
    QCOMPARE(array.size(), 6);
    QCOMPARE(array.capacity(), 6);
    for (int index = 0; index < 6; ++index)
        QCOMPARE(array.at(index), contents[index]);
    QVERIFY(array.constData() == contents);
    QVERIFY(!array.isDetached());

    // Force a copy-on-write.
    array[3] = 42.0f;
    QVERIFY(array.isDetached());
    QCOMPARE(contents[3], 4.0f);
    QCOMPARE(array.size(), 6);
    QVERIFY(array.capacity() > 6);
    for (int index = 0; index < 6; ++index) {
        if (index != 3)
            QCOMPARE(array.at(index), contents[index]);
        else
            QCOMPARE(array.at(index), 42.0f);
    }
    QVERIFY(array.constData() != contents);

    array = QArray<float>::fromRawData(contents, 12);
    QCOMPARE(array.size(), 12);
    QCOMPARE(array.capacity(), 12);
    for (int index = 0; index < 12; ++index)
        QCOMPARE(array.at(index), contents[index]);
    QVERIFY(array.constData() == contents);

    QString strings[] = {QLatin1String("foo"), QLatin1String("bar")};
    QArray<QString> array2;
    array2 = QArray<QString>::fromRawData(strings, 2);
    QCOMPARE(array2.size(), 2);
    QCOMPARE(array2.capacity(), 2);
    QCOMPARE(array2.at(0), QLatin1String("foo"));
    QCOMPARE(array2.at(1), QLatin1String("bar"));
    QVERIFY(array2.constData() == strings);

    // Force a copy-on-write.
    array2[1] = QLatin1String("baz");
    QCOMPARE(array2.size(), 2);
    QVERIFY(array2.capacity() > 2);
    QCOMPARE(array2.at(0), QLatin1String("foo"));
    QCOMPARE(array2.at(1), QLatin1String("baz"));
    QVERIFY(array2.constData() != strings);
    QCOMPARE(strings[0], QLatin1String("foo"));
    QCOMPARE(strings[1], QLatin1String("bar"));
}

void tst_QArray::fromWritableRawData()
{
    QArray<float> array;
    float contents[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    float contentsModified[] =
        {1.0f, 2.0f, 3.0f, 42.0f, 5.0f, 6.0f, 53.0f};

    array = QArray<float>::fromWritableRawData(contents, 0);
    QCOMPARE(array.size(), 0);
    QCOMPARE(array.capacity(), 0);
    array.append(0.0f);
    QCOMPARE(array.size(), 1);
    QVERIFY(array.capacity() > 0);
    QCOMPARE(array.at(0), 0.0f);

    array = QArray<float>::fromWritableRawData(contents, 6);
    QCOMPARE(array.size(), 6);
    QCOMPARE(array.capacity(), 6);
    for (int index = 0; index < 6; ++index)
        QCOMPARE(array.at(index), contents[index]);
    QVERIFY(array.constData() == contents);

    // Modify the raw data in-place.
    array[3] = 42.0f;
    QVERIFY(array.constData() == contents);

    // Force a copy.
    array.append(53.0f);
    QVERIFY(array.constData() != contents);
    for (int index = 0; index < array.size(); ++index)
        QCOMPARE(array.at(index), contentsModified[index]);

    // Resize to smaller should stay within the raw data.
    array = QArray<float>::fromWritableRawData(contents, 6);
    array.resize(6);
    QCOMPARE(array.size(), 6);
    array.resize(5);
    QCOMPARE(array.size(), 5);
    QVERIFY(array.constData() == contents);
    for (int index = 0; index < array.size(); ++index)
        QCOMPARE(array.at(index), contentsModified[index]);
    array.append(6.0f);
    QVERIFY(array.constData() == contents);
    for (int index = 0; index < array.size(); ++index)
        QCOMPARE(array.at(index), contentsModified[index]);

    // Resize to larger should force a copy.
    array = QArray<float>::fromWritableRawData(contents, 6);
    array.resize(7);
    QVERIFY(array.constData() != contents);
    for (int index = 0; index < 6; ++index)
        QCOMPARE(array.at(index), contentsModified[index]);
    QCOMPARE(array.at(6), 0.0f);

    // Reserve to a larger size should force a copy.
    array = QArray<float>::fromWritableRawData(contents, 6);
    array.reserve(7);
    QCOMPARE(array.size(), 6);
    QVERIFY(array.capacity() >= 7);
    QVERIFY(array.constData() != contents);
    for (int index = 0; index < 6; ++index)
        QCOMPARE(array.at(index), contentsModified[index]);
}

void tst_QArray::search()
{
    QArray<float> array;
    for (int index = 0; index < 1000; ++index)
        array.append(float(index));

    QCOMPARE(array.indexOf(0.0f), 0);
    QCOMPARE(array.indexOf(10.0f), 10);
    QCOMPARE(array.indexOf(999.0f), 999);
    QCOMPARE(array.indexOf(1000.0f), -1);
    QCOMPARE(array.indexOf(10.0f, 9), 10);
    QCOMPARE(array.indexOf(10.0f, 10), 10);
    QCOMPARE(array.indexOf(10.0f, 11), -1);
    QCOMPARE(array.indexOf(999.0f, -1), 999);
    QCOMPARE(array.indexOf(998.0f, -1), -1);
    QCOMPARE(array.indexOf(998.0f, -2), 998);
    QCOMPARE(array.indexOf(998.0f, -3), 998);
    QCOMPARE(array.indexOf(998.0f, -2000), 998);
    QCOMPARE(array.indexOf(998.0f, 2000), -1);

    QCOMPARE(array.lastIndexOf(0.0f), 0);
    QCOMPARE(array.lastIndexOf(10.0f), 10);
    QCOMPARE(array.lastIndexOf(999.0f), 999);
    QCOMPARE(array.lastIndexOf(1000.0f), -1);
    QCOMPARE(array.lastIndexOf(10.0f, 9), -1);
    QCOMPARE(array.lastIndexOf(10.0f, 10), 10);
    QCOMPARE(array.lastIndexOf(10.0f, 11), 10);
    QCOMPARE(array.lastIndexOf(999.0f, -1), 999);
    QCOMPARE(array.lastIndexOf(998.0f, -1), 998);
    QCOMPARE(array.lastIndexOf(998.0f, -2), 998);
    QCOMPARE(array.lastIndexOf(998.0f, -3), -1);
    QCOMPARE(array.lastIndexOf(998.0f, -2000), -1);
    QCOMPARE(array.lastIndexOf(998.0f, 2000), 998);

    QVERIFY(array.contains(0.0f));
    QVERIFY(array.contains(10.0f));
    QVERIFY(array.contains(999.0f));
    QVERIFY(!array.contains(1000.0f));
    QVERIFY(!array.contains(-1.0f));

    array.append(500.0f);
    QCOMPARE(array.count(0.0f), 1);
    QCOMPARE(array.count(10.0f), 1);
    QCOMPARE(array.count(500.0f), 2);
    QCOMPARE(array.count(999.0f), 1);
    QCOMPARE(array.count(1000.0f), 0);

    QVERIFY(array.startsWith(0.0f));
    QVERIFY(!array.startsWith(1.0f));

    QVERIFY(array.endsWith(500.0f));
    QVERIFY(!array.endsWith(1.0f));

    QCOMPARE(array.first(), 0.0f);
    QCOMPARE(array.last(), 500.0f);
}

void tst_QArray::fill()
{
    QArray<float> array;
    array.fill(1.0f);
    QCOMPARE(array.size(), 0);

    array.fill(1.0f, 100);
    QCOMPARE(array.size(), 100);
    for (int index = 0; index < 100; ++index)
        QCOMPARE(array.at(index), 1.0f);

    array.fill(2.0f);
    QCOMPARE(array.size(), 100);
    for (int index = 0; index < 100; ++index)
        QCOMPARE(array.at(index), 2.0f);

    array.fill(3.0f, 20);
    QCOMPARE(array.size(), 20);
    for (int index = 0; index < 20; ++index)
        QCOMPARE(array.at(index), 3.0f);
}

void tst_QArray::unsharedArray()
{
    QUnsharedArray<float> array;
    array.append(1.0f);
    array.append(2.0f);

    QArray<float> array2(array);
    QVERIFY(array.constData() != array2.constData());

    array = array2;
    QVERIFY(array.constData() != array2.constData());

    QCOMPARE(array.size(), 2);
    QCOMPARE(array2.size(), 2);

    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 2.0f);
    array[1] = 3.0f;
    QCOMPARE(array[0], 1.0f);
    QCOMPARE(array[1], 3.0f);

    // Raw data will be forcibly copied with QUnsharedArray.
    float contents[] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    array = QArray<float>::fromRawData(contents, 6);
    QVERIFY(array.constData() != contents);

    QUnsharedArray<float> array3;
    for (int index = 0; index < 100; ++index)
        array3.append(float(index));
    QArray<float> array4(array3);
    QVERIFY(array3.constData() != array4.constData());

    array3.setSharable(true);
    QArray<float> array5(array3);
    QVERIFY(array3.constData() == array5.constData());
}

// The following tests check that if QVarLengthArray was typedef'ed
// to QArray, the behavior would be identical.

#define QVarLengthArray QArray

int fooCtor = 0;
int fooDtor = 0;

struct Foo
{
    int *p;

    Foo() { p = new int; ++fooCtor; }
    Foo(const Foo &other) { Q_UNUSED(other); p = new int; ++fooCtor; }

    void operator=(const Foo & /* other */) { }

    ~Foo() { delete p; ++fooDtor; }
};

void tst_QArray::QVarLengthArray_append()
{
    QVarLengthArray<QString> v;
    v.append(QString("hello"));

    QVarLengthArray<int> v2; // rocket!
    v2.append(5);
}

void tst_QArray::QVarLengthArray_removeLast()
{
    {
        QVarLengthArray<char, 2> v;
        v.append(0);
        v.append(1);
        QCOMPARE(v.size(), 2);
        v.append(2);
        v.append(3);
        QCOMPARE(v.size(), 4);
        v.removeLast();
        QCOMPARE(v.size(), 3);
        v.removeLast();
        QCOMPARE(v.size(), 2);
    }

    {
        QVarLengthArray<QString, 2> v;
        v.append("0");
        v.append("1");
        QCOMPARE(v.size(), 2);
        v.append("2");
        v.append("3");
        QCOMPARE(v.size(), 4);
        v.removeLast();
        QCOMPARE(v.size(), 3);
        v.removeLast();
        QCOMPARE(v.size(), 2);
    }
}

void tst_QArray::QVarLengthArray_oldTests()
{
    {
	QVarLengthArray<int, 256> sa(128);
	QVERIFY(sa.data() == &sa[0]);
	sa[0] = 0xfee;
	sa[10] = 0xff;
	QVERIFY(sa[0] == 0xfee);
	QVERIFY(sa[10] == 0xff);
	sa.resize(512);
	QVERIFY(sa.data() == &sa[0]);
	QVERIFY(sa[0] == 0xfee);
	QVERIFY(sa[10] == 0xff);
	QVERIFY(sa.size() == 512);
	sa.reserve(1024);
	QVERIFY(sa.capacity() == 1024);
	QVERIFY(sa.size() == 512);
    }
    {
	QVarLengthArray<QString> sa(10);
	sa[0] = "Hello";
	sa[9] = "World";
	QVERIFY(*sa.data() == "Hello");
	QVERIFY(sa[9] == "World");
	sa.reserve(512);
	QVERIFY(*sa.data() == "Hello");
	QVERIFY(sa[9] == "World");
	sa.resize(512);
	QVERIFY(*sa.data() == "Hello");
	QVERIFY(sa[9] == "World");
    }
    {
        int arr[2] = {1, 2};
        QVarLengthArray<int> sa(10);
        QCOMPARE(sa.size(), 10);
        sa.append(arr, 2);
        QCOMPARE(sa.size(), 12);
        QCOMPARE(sa[10], 1);
        QCOMPARE(sa[11], 2);
    }
    {
        QString arr[2] = { QString("hello"), QString("world") };
        QVarLengthArray<QString> sa(10);
        QCOMPARE(sa.size(), 10);
        sa.append(arr, 2);
        QCOMPARE(sa.size(), 12);
        QCOMPARE(sa[10], QString("hello"));
        QCOMPARE(sa[11], QString("world"));

        sa.append(arr, 1);
        QCOMPARE(sa.size(), 13);
        QCOMPARE(sa[12], QString("hello"));

        sa.append(arr, 0);
        QCOMPARE(sa.size(), 13);
    }
    {
        // assignment operator and copy constructor

        QVarLengthArray<int> sa(10);
        sa[5] = 5;

        QVarLengthArray<int> sa2(10);
        sa2[5] = 6;
        sa2 = sa;
        QCOMPARE(sa2[5], 5);

        QVarLengthArray<int> sa3(sa);
        QCOMPARE(sa3[5], 5);
    }

    //QSKIP("This test causes the machine to crash when allocating too much memory.", SkipSingle);
    {
        QVarLengthArray<Foo> a;
        //const int N = 0x7fffffff / sizeof(Foo);
        const int N = 0x7fff / sizeof(Foo);
        const int Prealloc = a.capacity();
        const Foo *data0 = a.constData();

        a.resize(N);
        if (a.size() == N) {
            QVERIFY(a.capacity() >= N);
            QCOMPARE(fooCtor, N);
            QCOMPARE(fooDtor, 0);

            for (int i = 0; i < N; i += 35000)
                a[i] = Foo();
        } else {
            // this is the case we're actually testing
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), Prealloc);
            QCOMPARE(a.constData(), data0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);

            a.resize(5);
            QCOMPARE(a.size(), 5);
            QCOMPARE(a.capacity(), Prealloc);
            QCOMPARE(a.constData(), data0);
            QCOMPARE(fooCtor, 5);
            QCOMPARE(fooDtor, 0);

            a.resize(Prealloc + 1);
            QCOMPARE(a.size(), Prealloc + 1);
            QVERIFY(a.capacity() >= Prealloc + 1);
            QVERIFY(a.constData() != data0);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, 5);

            const Foo *data1 = a.constData();

            a.resize(0x10000000);
            QCOMPARE(a.size(), 0);
            QVERIFY(a.capacity() >= Prealloc + 1);
            QVERIFY(a.constData() == data1);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, Prealloc + 6);
        }
    }
}

void tst_QArray::QVarLengthArray_task214223()
{
    //creating a QVarLengthArray of the same size as the prealloc size
    // will make the next call to append(const T&) corrupt the memory
    // you should get a segfault pretty soon after that :-)
    QVarLengthArray<float, 1> d(1);
    for (int i=0; i<30; i++) 
        d.append(i);
}

#undef QVarLengthArray

// The following tests check that if QVector was typedef'ed
// to QArray, the behavior would be identical.

#define QVector QArray

void tst_QArray::QVector_outOfMemory()
{
    fooCtor = 0;
    fooDtor = 0;

    //const int N = 0x7fffffff / sizeof(Foo);
    const int N = 0x7fff / sizeof(Foo);

    {
        QVector<Foo> a;

        //QSKIP("This test crashes on many of our machines.", SkipSingle);
        a.resize(N);
        if (a.size() == N) {
            QVERIFY(a.capacity() >= N);
            QCOMPARE(fooCtor, N);
            QCOMPARE(fooDtor, 0);

            for (int i = 0; i < N; i += 35000)
                a[i] = Foo();
        } else {
            // this is the case we're actually testing
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);

            a.resize(5);
            QCOMPARE(a.size(), 5);
            QVERIFY(a.capacity() >= 5);
            QCOMPARE(fooCtor, 5);
            QCOMPARE(fooDtor, 0);

            const int Prealloc = a.capacity();
            a.resize(Prealloc + 1);
            QCOMPARE(a.size(), Prealloc + 1);
            QVERIFY(a.capacity() >= Prealloc + 1);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, 5);

            a.resize(0x10000000);
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, Prealloc + 6);
            QCOMPARE(fooDtor, Prealloc + 6);
        }
    }

    fooCtor = 0;
    fooDtor = 0;

    {
        QVector<Foo> a(N);
        if (a.size() == N) {
            QVERIFY(a.capacity() >= N);
            QCOMPARE(fooCtor, N);
            QCOMPARE(fooDtor, 0);

            for (int i = 0; i < N; i += 35000)
                a[i] = Foo();
        } else {
            // this is the case we're actually testing
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);
        }
    }

    Foo foo;

    fooCtor = 0;
    fooDtor = 0;

    {
        QVector<Foo> a(N, foo);
        if (a.size() == N) {
            QVERIFY(a.capacity() >= N);
            QCOMPARE(fooCtor, N);
            QCOMPARE(fooDtor, 0);

            for (int i = 0; i < N; i += 35000)
                a[i] = Foo();
        } else {
            // this is the case we're actually testing
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 0);
            QCOMPARE(fooDtor, 0);
        }
    }

    fooCtor = 0;
    fooDtor = 0;

    {
        QVector<Foo> a;
        a.resize(10);
        QCOMPARE(fooCtor, 10);
        QCOMPARE(fooDtor, 0);

        QVector<Foo> b(a);
        QCOMPARE(fooCtor, 10);
        QCOMPARE(fooDtor, 0);

        a.resize(N);
        if (a.size() == N) {
            QCOMPARE(fooCtor, N + 10);
        } else {
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);
            QCOMPARE(fooCtor, 10);
            QCOMPARE(fooDtor, 0);

            QCOMPARE(b.size(), 10);
            QVERIFY(b.capacity() >= 10);
        }
    }

    {
        QVector<int> a;
        a.resize(10);

        QVector<int> b(a);

        a.resize(N);
        if (a.size() == N) {
            for (int i = 0; i < N; i += 60000)
                a[i] = i;
        } else {
            QCOMPARE(a.size(), 0);
            QCOMPARE(a.capacity(), 0);

            QCOMPARE(b.size(), 10);
            QVERIFY(b.capacity() >= 10);
        }

        b.resize(N - 1);
        if (b.size() == N - 1) {
            for (int i = 0; i < N - 1; i += 60000)
                b[i] = i;
        } else {
            QCOMPARE(b.size(), 0);
            QCOMPARE(b.capacity(), 0);
        }
    }
}

void tst_QArray::QVector_QTBUG6416_reserve()
{
    fooCtor = 0;
    fooDtor = 0;
    {
        QVector<Foo> a;
        a.resize(2);
        QVector<Foo> b(a);
        b.reserve(1);
    }
    QCOMPARE(fooCtor, fooDtor);
}

#undef QVector

// The following tests check that if QList was typedef'ed
// to QArray, the behavior would be identical.

#define QVector QArray

void tst_QArray::QList_length() const
{
    /* Empty list. */
    {
        const QList<int> list;
        QCOMPARE(list.length(), 0);
    }

    /* One entry. */
    {
        QList<int> list;
        list.append(0);
        QCOMPARE(list.length(), 1);
    }

    /* Two entries. */
    {
        QList<int> list;
        list.append(0);
        list.append(1);
        QCOMPARE(list.length(), 2);
    }

    /* Three entries. */
    {
        QList<int> list;
        list.append(0);
        list.append(0);
        list.append(0);
        QCOMPARE(list.length(), 3);
    }
}

void tst_QArray::QList_lengthSignature() const
{
    /* Constness. */
    {
        const QList<int> list;
        /* The function should be const. */
        list.length();
    }
}

void tst_QArray::QList_append() const
{
    /* test append(const QList<T> &) function */
    QString one("one");
    QString two("two");
    QString three("three");
    QString four("four");
    QList<QString> list1;
    QList<QString> list2;
    QList<QString> listTotal;
    list1.append(one);
    list1.append(two);
    list2.append(three);
    list2.append(four);
    list1.append(list2);
    //qDebug() << list1;
    listTotal.append(one);
    listTotal.append(two);
    listTotal.append(three);
    listTotal.append(four);
    QCOMPARE(list1, listTotal);

}

#undef QList

QTEST_APPLESS_MAIN(tst_QArray)

#include "tst_qarray.moc"
