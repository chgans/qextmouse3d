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
#include "qvector2darray.h"
#include "qvector3darray.h"
#include "qvector4darray.h"

#include "qtest_helpers_p.h"

class tst_QVectorArray : public QObject
{
    Q_OBJECT
public:
    tst_QVectorArray() {}
    ~tst_QVectorArray() {}

private slots:
    void vector2DArray();
    void vector3DArray();
    void vector4DArray();
};

void tst_QVectorArray::vector2DArray()
{
    QVector2DArray array;
    QVERIFY(array.isEmpty());

    array.append(1.0f, 2.0f);
    array.append(3.0f, 4.0f);
    array.append(QVector2D(5.0f, 6.0f));
    array.append(QPointF(7.0f, 8.0f));
    array.append(QPoint(9, 10));

    QCOMPARE(array.size(), 5);
    QVERIFY(array[0] == QVector2D(1.0f, 2.0f));
    QVERIFY(array[1] == QVector2D(3.0f, 4.0f));
    QVERIFY(array[2] == QVector2D(5.0f, 6.0f));
    QVERIFY(array[3] == QVector2D(7.0f, 8.0f));
    QVERIFY(array[4] == QVector2D(9.0f, 10.0f));

    array.append(QVector2D(11.0f, 12.0f), QVector2D(13.0f, 14.0f));
    array.append(QVector2D(15.0f, 16.0f), QVector2D(17.0f, 18.0f),
                 QVector2D(19.0f, 20.0f));
    array.append(QVector2D(21.0f, 22.0f), QVector2D(23.0f, 24.0f),
                 QVector2D(25.0f, 26.0f));

    for (int index = 0; index < array.size(); ++index)
        QVERIFY(array[index] == QVector2D(index * 2 + 1, index * 2 + 2));

    int size = array.size();
    QVector2DArray result = array.scaled(1.0);
    // check did not change the original
    QCOMPARE(array.size(), size);
    QVERIFY(array[0] == QVector2D(1.0f, 2.0f));
    QVERIFY(array[4] == QVector2D(9.0f, 10.0f));
    // result should be copy - mult by 1.0 costs nothing
    QVERIFY(!result.isDetached());
    QCOMPARE(result.size(), size);
    QCOMPARE(result[0], QVector2D(1.0f, 2.0f));
    QCOMPARE(result[4], QVector2D(9.0f, 10.0f));
    QCOMPARE(result[12], QVector2D(25.0f, 26.0f));
    // now actually do a scale
    result = array.scaled(2.0);
    QCOMPARE(result.size(), size);
    QCOMPARE(array.size(), size);
    QCOMPARE(array[0], QVector2D(1.0f, 2.0f));
    QCOMPARE(array[4], QVector2D(9.0f, 10.0f));
    QCOMPARE(result[0], QVector2D(2.0f, 4.0f));
    QCOMPARE(result[4], QVector2D(18.0f, 20.0f));
    QCOMPARE(result[12], QVector2D(50.0f, 52.0f));

    array.scale(1.0);
    QCOMPARE(array.size(), size);  // should all be the same
    QCOMPARE(array[0], QVector2D(1.0f, 2.0f));
    QCOMPARE(array[4], QVector2D(9.0f, 10.0f));
    QCOMPARE(array[12], QVector2D(25.0f, 26.0f));

    array.scale(2.0);
    QCOMPARE(array.size(), size);  // size should be the same
    QCOMPARE(array[0], QVector2D(2.0f, 4.0f));
    QCOMPARE(array[4], QVector2D(18.0f, 20.0f));
    QCOMPARE(array[12], QVector2D(50.0f, 52.0f));
}

void tst_QVectorArray::vector3DArray()
{
    QVector3DArray array;
    QVERIFY(array.isEmpty());

    array.append(1.0f, 2.0f, 3.0f);
    array.append(3.0f, 4.0f, 5.0f);
    array.append(QVector3D(5.0f, 6.0f, 7.0f));

    QCOMPARE(array.size(), 3);
    QVERIFY(array[0] == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(array[1] == QVector3D(3.0f, 4.0f, 5.0f));
    QVERIFY(array[2] == QVector3D(5.0f, 6.0f, 7.0f));

    array.append(QVector3D(7.0f, 8.0f, 9.0f),
                 QVector3D(9.0f, 10.0f, 11.0f));
    array.append(QVector3D(11.0f, 12.0f, 13.0f),
                 QVector3D(13.0f, 14.0f, 15.0f),
                 QVector3D(15.0f, 16.0f, 17.0f));
    array.append(QVector3D(17.0f, 18.0f, 19.0f),
                 QVector3D(19.0f, 20.0f, 21.0f),
                 QVector3D(21.0f, 22.0f, 23.0f));

    for (int index = 0; index < array.size(); ++index) {
        QVERIFY(array[index] == QVector3D(index * 2 + 1,
                                          index * 2 + 2,
                                          index * 2 + 3));
    }

    int size = array.size();
    QVector3DArray result = array.scaled(1.0);
    // check did not change the original
    QCOMPARE(array.size(), size);
    QCOMPARE(array[0], QVector3D(1.0f, 2.0f, 3.0));
    QCOMPARE(array[4], QVector3D(9.0f, 10.0f, 11.0f));
    // result should be copy - mult by 1.0 costs nothing
    QVERIFY(!result.isDetached());
    QCOMPARE(result.size(), size);
    QCOMPARE(result[0], QVector3D(1.0f, 2.0f, 3.0f));
    QCOMPARE(result[4], QVector3D(9.0f, 10.0f, 11.0f));
    QCOMPARE(result[10], QVector3D(21.0f, 22.0f, 23.0f));
    // now actually do a scale
    result = array.scaled(2.0);
    QCOMPARE(result.size(), size);
    QCOMPARE(array.size(), size);
    QCOMPARE(result[0], QVector3D(2.0f, 4.0f, 6.0f));
    QCOMPARE(result[4], QVector3D(18.0f, 20.0f, 22.0f));
    QCOMPARE(result[10], QVector3D(42.0f, 44.0f, 46.0f));

    array.scale(1.0);
    QCOMPARE(array.size(), size);  // should all be the same
    QCOMPARE(array[0], QVector3D(1.0f, 2.0f, 3.0f));
    QCOMPARE(array[4], QVector3D(9.0f, 10.0f, 11.0f));
    QCOMPARE(array[10], QVector3D(21.0f, 22.0f, 23.0f));

    array.scale(2.0);
    QCOMPARE(array.size(), size);  // size should be the same
    QCOMPARE(array[0], QVector3D(2.0f, 4.0f, 6.0f));
    QCOMPARE(array[4], QVector3D(18.0f, 20.0f, 22.0f));
    QCOMPARE(array[10], QVector3D(42.0f, 44.0f, 46.0f));
}

void tst_QVectorArray::vector4DArray()
{
    QVector4DArray array;
    QVERIFY(array.isEmpty());

    array.append(1.0f, 2.0f, 3.0f, 4.0f);
    array.append(3.0f, 4.0f, 5.0f, 6.0f);
    array.append(QVector4D(5.0f, 6.0f, 7.0f, 8.0f));

    QCOMPARE(array.size(), 3);
    QVERIFY(array[0] == QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QVERIFY(array[1] == QVector4D(3.0f, 4.0f, 5.0f, 6.0f));
    QVERIFY(array[2] == QVector4D(5.0f, 6.0f, 7.0f, 8.0f));

    array.append(QVector4D(7.0f, 8.0f, 9.0f, 10.0f),
                 QVector4D(9.0f, 10.0f, 11.0f, 12.0f));
    array.append(QVector4D(11.0f, 12.0f, 13.0f, 14.0f),
                 QVector4D(13.0f, 14.0f, 15.0f, 16.0f),
                 QVector4D(15.0f, 16.0f, 17.0f, 18.0f));
    array.append(QVector4D(17.0f, 18.0f, 19.0f, 20.0f),
                 QVector4D(19.0f, 20.0f, 21.0f, 22.0f),
                 QVector4D(21.0f, 22.0f, 23.0f, 24.0f));

    for (int index = 0; index < array.size(); ++index) {
        QVERIFY(array[index] == QVector4D(index * 2 + 1,
                                          index * 2 + 2,
                                          index * 2 + 3,
                                          index * 2 + 4));
    }

    int size = array.size();
    QVector4DArray result = array.scaled(1.0);
    // check did not change the original
    QCOMPARE(array.size(), size);
    QCOMPARE(array[0], QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QCOMPARE(array[4], QVector4D(9.0f, 10.0f, 11.0f, 12.0));
    // result should be copy - mult by 1.0 costs nothing
    QVERIFY(!result.isDetached());
    QCOMPARE(result.size(), size);
    QCOMPARE(result[0], QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QCOMPARE(result[4], QVector4D(9.0f, 10.0f, 11.0f, 12.0));
    // now actually do a scale
    result = array.scaled(2.0);
    QCOMPARE(result.size(), size);
    QCOMPARE(array.size(), size);
    QCOMPARE(result[0], QVector4D(2.0f, 4.0f, 6.0f, 8.0f));
    QCOMPARE(result[4], QVector4D(18.0f, 20.0f, 22.0f, 24.0f));

    array.scale(1.0);
    QCOMPARE(array.size(), size);  // should all be the same
    QCOMPARE(array[0], QVector4D(1.0f, 2.0f, 3.0f, 4.0f));
    QCOMPARE(array[4], QVector4D(9.0f, 10.0f, 11.0f, 12.0));

    array.scale(2.0);
    QCOMPARE(array.size(), size);  // size should be the same
    QCOMPARE(array[0], QVector4D(2.0f, 4.0f, 6.0f, 8.0f));
    QCOMPARE(array[4], QVector4D(18.0f, 20.0f, 22.0f, 24.0f));
}

QTEST_APPLESS_MAIN(tst_QVectorArray)

#include "tst_qvectorarray.moc"
