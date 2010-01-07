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
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qvector.h>
#include "qdataarray.h"

class tst_QDataArray : public QObject
{
    Q_OBJECT
public:
    tst_QDataArray() {}
    virtual ~tst_QDataArray() {}

private slots:
    void append_data();
    void append();
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
    Test_VarLengthArray,
    Test_DataArray
};

void tst_QDataArray::append_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 0; size < 1024; size += 12) {
        name = "databuffer--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataBuffer);

        name = "vector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Vector);

        name = "varlengtharray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_VarLengthArray);

        name = "dataarray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataArray);
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
    }
}

void tst_QDataArray::appendSmall_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 0; size < 16; ++size) {
        name = "databuffer--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataBuffer);

        name = "vector--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Vector);

        name = "varlengtharray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_VarLengthArray);

        name = "dataarray--";
        name += QString::number(size);
        QTest::newRow(name.constData()) << size << int(Test_DataArray);
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
    }
}

QTEST_MAIN(tst_QDataArray)

#include "tst_qdataarray.moc"
