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
#include "qglsection.h"
#include "qgldisplaylist.h"

class tst_QGLSection : public QObject
{
    Q_OBJECT
public:
    tst_QGLSection() {}
    ~tst_QGLSection() {}

private slots:
    void create();
    void modify();
};

void tst_QGLSection::create()
{
    // Test that a newly created object has the correct defaults.
    QGLDisplayList dl;
    QGLSection *patch = dl.newSection();
    QCOMPARE(patch->start(), 0);
    QCOMPARE(patch->count(), 0);
    QCOMPARE(patch->smoothing(), QGL::Smooth);
}

void tst_QGLSection::modify()
{
    QGLDisplayList dl;
    QGLSection *patch = dl.newSection();
    dl.addTriangle(QVector3D(), QVector3D(), QVector3D(), QVector3D(1.0f, 0.0f, 0.0f));
    QCOMPARE(patch->start(), 0);
    QCOMPARE(patch->count(), 3);
    QGLSection *patch2 = dl.newSection();
    dl.addTriangle(QVector3D(), QVector3D(), QVector3D(), QVector3D(1.0f, 0.0f, 0.0f));
    QCOMPARE(patch2->start(), 3);
    QCOMPARE(patch2->count(), 3);
}

QTEST_APPLESS_MAIN(tst_QGLSection)

#include "tst_qglsection.moc"
