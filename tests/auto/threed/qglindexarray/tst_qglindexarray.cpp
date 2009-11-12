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
#include "qglindexarray.h"

class tst_QGLIndexArray : public QObject
{
    Q_OBJECT
public:
    tst_QGLIndexArray() {}
    ~tst_QGLIndexArray() {}

private slots:
    void create();
    void accessors();
};

void tst_QGLIndexArray::create()
{
    QGLIndexArray indexes;
    QCOMPARE(indexes.size(), 0);
    QVERIFY(indexes.isEmpty());
    QCOMPARE(indexes, indexes);   // operator ==
    QCOMPARE(indexes.constData(), (void*)0);
    QCOMPARE(indexes.data(), (void*)0);
}

void tst_QGLIndexArray::accessors()
{
    QGLIndexArray indexes;
    QGLIndexArray indexes2;
    QCOMPARE(indexes, indexes2);
    indexes += 1;
    QCOMPARE((int)(indexes[0]), 1);
    QCOMPARE(indexes.size(), 1);
    indexes += 2;
    QCOMPARE((int)(indexes[1]), 2);
    QCOMPARE(indexes.size(), 2);
    indexes2 << 1;
    QCOMPARE((int)(indexes2[0]), 1);
    QCOMPARE(indexes2.size(), 1);
    indexes2 << 2;
    QCOMPARE((int)(indexes2[1]), 2);
    QCOMPARE(indexes2.size(), 2);
}

QTEST_APPLESS_MAIN(tst_QGLIndexArray)

#include "tst_qglindexarray.moc"
