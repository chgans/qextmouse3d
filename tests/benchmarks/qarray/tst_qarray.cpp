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
#include <QtGui/private/qdatabuffer_p.h>
#include <QtGui/qvector3d.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include "qarray.h"
#ifndef QT_NO_STL
#include <vector>
#endif

#if QT_VERSION < 0x040700
// Fixed in Qt 4.7.
Q_DECLARE_TYPEINFO(QVector3D, Q_MOVABLE_TYPE);
#endif

//#define TEST_QLIST 1
//#define TEST_UNSHAREDARRAY 1

class tst_QArray : public QObject
{
    Q_OBJECT
public:
    tst_QArray() {}
    virtual ~tst_QArray() {}

private slots:
    void append_data();
    void append();
    void appendReserved_data();
    void appendReserved();
    void appendVector3D_data();
    void appendVector3D();
    void appendSmall_data();
    void appendSmall();
    void appendFourAtATime_data();
    void appendFourAtATime();
    void clear_data();
    void clear();
    void randomAccess_data();
    void randomAccess();
};

enum {
    Test_DataBuffer,
    Test_Vector,
    Test_List,
    Test_VarLengthArray,
    Test_Array,
    Test_UnsharedArray,
    Test_STLVector
};

void tst_QArray::append_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 0; size < 1024; size += 12) {
        name = "QDataBuffer--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataBuffer);

        name = "QVector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Vector);

#if TEST_QLIST
        name = "QList--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_List);
#endif

        name = "QVarLengthArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_VarLengthArray);

        name = "QArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Array);

#if TEST_UNSHAREDARRAY
        name = "QUnsharedArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_UnsharedArray);
#endif

#ifndef QT_NO_STL
        name = "std::vector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_STLVector);
#endif
    }
}

void tst_QArray::append()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_DataBuffer) {
        QDataBuffer<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.add(float(i));
        }
    } else if (type == Test_Vector) {
        QVector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_UnsharedArray) {
        QUnsharedArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
#ifndef QT_NO_STL
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
#endif
    }
}

void tst_QArray::appendReserved_data()
{
    append_data();
}

void tst_QArray::appendReserved()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_DataBuffer) {
        QDataBuffer<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.add(float(i));
        }
    } else if (type == Test_Vector) {
        QVector<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_UnsharedArray) {
        QUnsharedArray<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
#ifndef QT_NO_STL
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
#endif
    }
}

void tst_QArray::appendVector3D_data()
{
    append_data();
}

void tst_QArray::appendVector3D()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_DataBuffer) {
        QDataBuffer<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.add(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_Vector) {
        QVector<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_List) {
        QList<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_Array) {
        QArray<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_UnsharedArray) {
        QUnsharedArray<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
#ifndef QT_NO_STL
    } else if (type == Test_STLVector) {
        std::vector<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(QVector3D(i, i + 1, i + 2));
        }
#endif
    }
}

void tst_QArray::appendSmall_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 0; size < 16; ++size) {
        name = "QDataBuffer--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataBuffer);

        name = "QVector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Vector);

#if TEST_QLIST
        name = "QList--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_List);
#endif

        name = "QVarLengthArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_VarLengthArray);

        name = "QArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Array);

#if TEST_UNSHAREDARRAY
        name = "QUnsharedArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_UnsharedArray);
#endif

#ifndef QT_NO_STL
        name = "std::vector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_STLVector);
#endif
    }
}

void tst_QArray::appendSmall()
{
    append();
}

void tst_QArray::appendFourAtATime_data()
{
    append_data();
}

void tst_QArray::appendFourAtATime()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_DataBuffer) {
        QDataBuffer<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.add(float(i));
                buffer.add(float(i + 1));
                buffer.add(float(i + 2));
                buffer.add(float(i + 3));
            }
        }
    } else if (type == Test_Vector) {
        QVector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i));
                buffer.append(float(i + 1));
                buffer.append(float(i + 2));
                buffer.append(float(i + 3));
            }
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i));
                buffer.append(float(i + 1));
                buffer.append(float(i + 2));
                buffer.append(float(i + 3));
            }
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i));
                buffer.append(float(i + 1));
                buffer.append(float(i + 2));
                buffer.append(float(i + 3));
            }
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i), float(i + 1),
                              float(i + 2), float(i + 3));
            }
        }
    } else if (type == Test_UnsharedArray) {
        QUnsharedArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i), float(i + 1),
                              float(i + 2), float(i + 3));
            }
        }
#ifndef QT_NO_STL
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.push_back(float(i));
                buffer.push_back(float(i + 1));
                buffer.push_back(float(i + 2));
                buffer.push_back(float(i + 3));
            }
        }
#endif
    }
}

void tst_QArray::clear_data()
{
    append_data();
}

void tst_QArray::clear()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_DataBuffer) {
        QDataBuffer<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.add(float(i));
            buffer.reset();
            for (int i = 0; i < size; ++i)
                buffer.add(float(i));
        }
    } else if (type == Test_Vector) {
        QVector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.resize(0);
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_UnsharedArray) {
        QUnsharedArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.resize(0);
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
#ifndef QT_NO_STL
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
#endif
    }
}

void tst_QArray::randomAccess_data()
{
    QTest::addColumn<int>("type");

    QTest::newRow("QDataBuffer") << int(Test_DataBuffer);
    QTest::newRow("QVector") << int(Test_Vector);
#if TEST_QLIST
    QTest::newRow("QList") << int(Test_List);
#endif
    QTest::newRow("QVarLengthArray") << int(Test_VarLengthArray);
    QTest::newRow("QArray") << int(Test_Array);
    QTest::newRow("QUnsharedArray") << int(Test_UnsharedArray);
#ifndef QT_NO_STL
    QTest::newRow("std::vector") << int(Test_STLVector);
#endif
}

// To force the values below to be computed and stored.
static int volatile finalSum;

void tst_QArray::randomAccess()
{
    QFETCH(int, type);

    if (type == Test_DataBuffer) {
        QDataBuffer<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.add(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer.at(i) = buffer.at(i - 10) + buffer.at(i - 4) * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer.at(i);
            finalSum = sum;
        }
    } else if (type == Test_Vector) {
        QVector<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer.at(i - 10) + buffer.at(i - 4) * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer.at(i);
            finalSum = sum;
        }
    } else if (type == Test_List) {
        QList<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer[i - 10] + buffer[i - 4] * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer[i];
            finalSum = sum;
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer[i - 10] + buffer[i - 4] * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer[i];
            finalSum = sum;
        }
    } else if (type == Test_Array) {
        QArray<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer.at(i - 10) + buffer.at(i - 4) * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer.at(i);
            finalSum = sum;
        }
    } else if (type == Test_UnsharedArray) {
        QUnsharedArray<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer[i - 10] + buffer[i - 4] * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer[i];
            finalSum = sum;
        }
#ifndef QT_NO_STL
    } else if (type == Test_STLVector) {
        std::vector<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.push_back(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer[i - 10] + buffer[i - 4] * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer[i];
            finalSum = sum;
        }
#endif
    }
}

QTEST_MAIN(tst_QArray)

#include "tst_qarray.moc"
