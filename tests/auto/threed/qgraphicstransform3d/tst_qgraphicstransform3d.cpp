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
#include "qgraphicsrotation3d.h"
#include "qgraphicsscale3d.h"
#include "qgraphicstranslation3d.h"

class tst_QGraphicsTransform3D : public QObject
{
    Q_OBJECT
public:
    tst_QGraphicsTransform3D() {}
    ~tst_QGraphicsTransform3D() {}

private slots:
    void rotation3D();
    void scale3D();
    void translation3D();
};

static bool isSameMatrix(const QMatrix4x4 &m1, const QMatrix4x4 &m2)
{
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            if (!qFuzzyIsNull(float(m1(row, col) - m2(row, col))))
                return false;
        }
    }
    return true;
}

void tst_QGraphicsTransform3D::rotation3D()
{
    QGraphicsRotation3D rot1;
    QVERIFY(rot1.origin() == QVector3D(0, 0, 0));
    QVERIFY(rot1.axis() == QVector3D(0, 0, 1));
    QCOMPARE(rot1.angle(), qreal(0.0f));

    QSignalSpy spy1(&rot1, SIGNAL(originChanged()));
    QSignalSpy spy2(&rot1, SIGNAL(axisChanged()));
    QSignalSpy spy3(&rot1, SIGNAL(angleChanged()));

    rot1.setOrigin(QVector3D(1, 2, 3));
    rot1.setAxis(QVector3D(4, -5, 6));
    rot1.setAngle(45.0f);
    QVERIFY(rot1.origin() == QVector3D(1, 2, 3));
    QVERIFY(rot1.axis() == QVector3D(4, -5, 6));
    QCOMPARE(rot1.angle(), qreal(45.0f));

    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);
    QCOMPARE(spy3.size(), 1);

    QMatrix4x4 m1;
    rot1.applyTo(&m1);

    QMatrix4x4 m2;
    m2.translate(1, 2, 3);
    m2.rotate(45.0f, QVector3D(4, -5, 6));
    m2.translate(-1, -2, -3);
    QVERIFY(isSameMatrix(m1, m2));
}

void tst_QGraphicsTransform3D::scale3D()
{
    QGraphicsScale3D scale1;
    QVERIFY(scale1.origin() == QVector3D(0, 0, 0));
    QVERIFY(scale1.scale() == QVector3D(1, 1, 1));

    QSignalSpy spy1(&scale1, SIGNAL(originChanged()));
    QSignalSpy spy2(&scale1, SIGNAL(scaleChanged()));

    scale1.setOrigin(QVector3D(1, 2, 3));
    scale1.setScale(QVector3D(4, -6, 0.5f));
    QVERIFY(scale1.origin() == QVector3D(1, 2, 3));
    QVERIFY(scale1.scale() == QVector3D(4, -6, 0.5f));

    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);

    QMatrix4x4 m1;
    scale1.applyTo(&m1);

    QMatrix4x4 m2;
    m2.translate(1, 2, 3);
    m2.scale(QVector3D(4, -6, 0.5f));
    m2.translate(-1, -2, -3);
    QVERIFY(isSameMatrix(m1, m2));

    scale1.setScale(QVector3D(20, -4, 42));
    QCOMPARE(spy2.size(), 2);

    QVERIFY(scale1.scale() == QVector3D(20, -4, 42));

    scale1.setScale(33);
    QCOMPARE(spy2.size(), 3);

    QVERIFY(scale1.scale() == QVector3D(33, 33, 33));

    QVERIFY(scale1.origin() == QVector3D(1, 2, 3));
    QCOMPARE(spy1.size(), 1);
}

void tst_QGraphicsTransform3D::translation3D()
{
    QGraphicsTranslation3D translate1;
    QVERIFY(translate1.translate() == QVector3D(0, 0, 0));
    QCOMPARE(translate1.progress(), qreal(1.0f));

    QSignalSpy spy1(&translate1, SIGNAL(translateChanged()));
    QSignalSpy spy2(&translate1, SIGNAL(progressChanged()));

    translate1.setTranslate(QVector3D(4, -6, 0.5f));
    translate1.setProgress(2.0f);
    QVERIFY(translate1.translate() == QVector3D(4, -6, 0.5f));
    QCOMPARE(translate1.progress(), qreal(2.0f));

    QCOMPARE(spy1.size(), 1);
    QCOMPARE(spy2.size(), 1);

    QMatrix4x4 m1;
    translate1.applyTo(&m1);

    QMatrix4x4 m2;
    m2.translate(QVector3D(8, -12, 1));
    QVERIFY(isSameMatrix(m1, m2));
}

QTEST_APPLESS_MAIN(tst_QGraphicsTransform3D)

#include "tst_qgraphicstransform3d.moc"
