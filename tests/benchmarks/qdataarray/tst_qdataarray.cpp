/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the test suite of the Qt Toolkit.
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
#include "qdataarray.h"
#include <vector>

// Needs to be put into qvector3d.h.  Fix in Qt 4.7.
Q_DECLARE_TYPEINFO(QVector3D, Q_MOVABLE_TYPE);

//#define TEST_QLIST 1

class tst_QDataArray : public QObject
{
    Q_OBJECT
public:
    tst_QDataArray() {}
    virtual ~tst_QDataArray() {}

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
};

enum {
    Test_DataBuffer,
    Test_Vector,
    Test_List,
    Test_VarLengthArray,
    Test_DataArray,
    Test_STLVector
};

void tst_QDataArray::append_data()
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

        name = "QDataArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataArray);

        name = "std::vector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_STLVector);
    }
}

void tst_QDataArray::append()
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
    } else if (type == Test_DataArray) {
        QDataArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
    }
}

void tst_QDataArray::appendReserved_data()
{
    append_data();
}

void tst_QDataArray::appendReserved()
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
    } else if (type == Test_DataArray) {
        QDataArray<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
    }
}

void tst_QDataArray::appendVector3D_data()
{
    append_data();
}

void tst_QDataArray::appendVector3D()
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
    } else if (type == Test_DataArray) {
        QDataArray<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_STLVector) {
        std::vector<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(QVector3D(i, i + 1, i + 2));
        }
    }
}

void tst_QDataArray::appendSmall_data()
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

        name = "QDataArray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataArray);

        name = "std::vector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_STLVector);
    }
}

void tst_QDataArray::appendSmall()
{
    append();
}

void tst_QDataArray::appendFourAtATime_data()
{
    append_data();
}

void tst_QDataArray::appendFourAtATime()
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
    } else if (type == Test_DataArray) {
        QDataArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i), float(i + 1),
                              float(i + 2), float(i + 3));
            }
        }
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
    }
}

void tst_QDataArray::clear_data()
{
    append_data();
}

void tst_QDataArray::clear()
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
    } else if (type == Test_DataArray) {
        QDataArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.resize(0);
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
    }
}

QTEST_MAIN(tst_QDataArray)

#include "tst_qdataarray.moc"
