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
#include "qresult.h"
#include "qvector3d.h"

class tst_QResult : public QObject
{
    Q_OBJECT
public:
    tst_QResult() {}
    ~tst_QResult() {}

private slots:
    void create();
    void implicitValidity();
    void implicitValue();
    void implicitConversion();

private:
    QResult<QVector3D> returnValidity(QResult<QVector3D>::Validity validity);
    QResult<QVector3D> returnValue(const QVector3D& value);
};

void tst_QResult::create()
{
    QResult<QVector3D> result1;
    QVERIFY(result1.validity() == QResult<QVector3D>::NoResult);
    QVERIFY(result1.value().isNull());

    QResult<QVector3D> result2(QResult<QVector3D>::InvalidArgument);
    QVERIFY(result2.validity() == QResult<QVector3D>::InvalidArgument);
    QVERIFY(result2.value().isNull());

    QResult<QVector3D> result3(QVector3D(1, 2, 3));
    QVERIFY(result3.validity() == QResult<QVector3D>::ResultExists);
    QVERIFY(result3.value() == QVector3D(1, 2, 3));

    QResult<QVector3D> result4(QVector3D(4, 5, 6), QResult<QVector3D>::OutOfRange);
    QVERIFY(result4.validity() == QResult<QVector3D>::OutOfRange);
    QVERIFY(result4.value() == QVector3D(4, 5, 6));
}

void tst_QResult::implicitValidity()
{
    QResult<QVector3D> result1 = returnValidity(QResult<QVector3D>::OutOfRange);
    QVERIFY(result1.validity() == QResult<QVector3D>::OutOfRange);
    QVERIFY(result1.value().isNull());
}

void tst_QResult::implicitValue()
{
    QResult<QVector3D> result1 = returnValue(QVector3D(1, 2, 3));
    QVERIFY(result1.validity() == QResult<QVector3D>::ResultExists);
    QVERIFY(result1.value() == QVector3D(1, 2, 3));
}

void tst_QResult::implicitConversion()
{
    QResult<QVector3D> result1(QVector3D(1, 2, 3));
    QVector3D v = result1;
    QVERIFY(v == QVector3D(1, 2, 3));
}

QResult<QVector3D> tst_QResult::returnValidity(QResult<QVector3D>::Validity validity)
{
    return validity;
}

QResult<QVector3D> tst_QResult::returnValue(const QVector3D& value)
{
    return value;
}

QTEST_APPLESS_MAIN(tst_QResult)

#include "tst_qresult.moc"
