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
#include "qglcamera.h"

class tst_QGLCamera : public QObject
{
    Q_OBJECT
public:
    tst_QGLCamera() {}
    ~tst_QGLCamera() {}

private slots:
    void create();
    void modify();
    void minViewSize();
};

void tst_QGLCamera::create()
{
    // Test that a newly created object has the correct defaults.
    QGLCamera camera;
    QVERIFY(camera.projectionType() == QGLCamera::Perspective);
    QCOMPARE(camera.fieldOfView(), (qreal)0.0f);
    QCOMPARE(camera.nearPlane(), (qreal)5.0f);
    QCOMPARE(camera.farPlane(), (qreal)1000.0f);
    QCOMPARE(camera.viewSize(), QSizeF(2.0f, 2.0f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.0001f, 0.0001f));
    QCOMPARE(camera.screenRotation(), 0);
    QVERIFY(camera.eye() == QVector3D(0, 0, 10));
    QVERIFY(camera.upVector() == QVector3D(0, 1, 0));
    QVERIFY(camera.center() == QVector3D(0, 0, 0));
    QCOMPARE(camera.eyeSeparation(), (qreal)0.0f);
    QVERIFY(camera.motionAdjustment() == QVector3D(0, 0, 1));
    QVERIFY(camera.adjustForAspectRatio());
}

void tst_QGLCamera::modify()
{
    // Test modifying each field individually.
    QGLCamera camera;
    QSignalSpy spy1(&camera, SIGNAL(projectionChanged()));
    QSignalSpy spy2(&camera, SIGNAL(viewChanged()));
    camera.setProjectionType(QGLCamera::Orthographic);
    QVERIFY(camera.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(spy1.size(), 1);
    camera.setFieldOfView(60.0f);
    QCOMPARE(camera.fieldOfView(), (qreal)60.0f);
    QCOMPARE(spy1.size(), 2);
    camera.setNearPlane(-3.0f);
    QCOMPARE(camera.nearPlane(), (qreal)-3.0f);
    QCOMPARE(spy1.size(), 3);
    camera.setFarPlane(3000.0f);
    QCOMPARE(camera.farPlane(), (qreal)3000.0f);
    QCOMPARE(spy1.size(), 4);
    camera.setViewSize(QSizeF(45.0f, 25.5f));
    QCOMPARE(camera.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(spy1.size(), 5);
    camera.setMinViewSize(QSizeF(0.05f, 0.025f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.05f, 0.025f));
    QCOMPARE(spy1.size(), 6);
    camera.setScreenRotation(270);
    QCOMPARE(camera.screenRotation(), 270);
    QCOMPARE(spy1.size(), 7);
    camera.setEye(QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QCOMPARE(spy2.size(), 1);
    camera.setUpVector(QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QCOMPARE(spy2.size(), 2);
    camera.setCenter(QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(camera.center() == QVector3D(7.0f, 8.0f, 9.0f));
    QCOMPARE(spy2.size(), 3);
    camera.setEyeSeparation(3.0f);
    QCOMPARE(camera.eyeSeparation(), (qreal)3.0f);
    QCOMPARE(spy2.size(), 4);
    camera.setMotionAdjustment(QVector3D(10.0f, 11.0f, 12.0f));
    QVERIFY(camera.motionAdjustment() == QVector3D(10.0f, 11.0f, 12.0f));
    QCOMPARE(spy2.size(), 5);
    camera.setAdjustForAspectRatio(false);
    QVERIFY(!camera.adjustForAspectRatio());
    QCOMPARE(spy2.size(), 6);

    // Test that we don't get any side effects between properties.
    QVERIFY(camera.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(camera.fieldOfView(), (qreal)60.0f);
    QCOMPARE(camera.nearPlane(), (qreal)-3.0f);
    QCOMPARE(camera.farPlane(), (qreal)3000.0f);
    QCOMPARE(camera.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.05f, 0.025f));
    QCOMPARE(camera.screenRotation(), 270);
    QVERIFY(camera.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera.center() == QVector3D(7.0f, 8.0f, 9.0f));
    QCOMPARE(camera.eyeSeparation(), (qreal)3.0f);
    QVERIFY(camera.motionAdjustment() == QVector3D(10.0f, 11.0f, 12.0f));
    QVERIFY(!camera.adjustForAspectRatio());

    // Test that changing to the same values does not emit any signals.
    camera.setProjectionType(QGLCamera::Orthographic);
    camera.setFieldOfView(60.0f);
    camera.setNearPlane(-3.0f);
    camera.setFarPlane(3000.0f);
    camera.setViewSize(QSizeF(45.0f, 25.5f));
    camera.setMinViewSize(QSizeF(0.05f, 0.025f));
    camera.setScreenRotation(270);
    camera.setEye(QVector3D(1.0f, 2.0f, 3.0f));
    camera.setUpVector(QVector3D(4.0f, 5.0f, 6.0f));
    camera.setCenter(QVector3D(7.0f, 8.0f, 9.0f));
    camera.setEyeSeparation(3.0f);
    camera.setMotionAdjustment(QVector3D(10.0f, 11.0f, 12.0f));
    camera.setAdjustForAspectRatio(false);
    QCOMPARE(spy1.size(), 7);
    QCOMPARE(spy2.size(), 6);
}

// Check that the minimum view size works correctly, including when
// the view size goes negative (e.g. for flipped y co-ordinates).
void tst_QGLCamera::minViewSize()
{
    QGLCamera camera;
    camera.setMinViewSize(QSizeF(0.05f, 0.05f));
    QCOMPARE(camera.minViewSize(), QSizeF(0.05f, 0.05f));
    camera.setViewSize(QSizeF(-20.0f, -30.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-20.0f, -30.0f));
    camera.setViewSize(QSizeF(0.0f, 1.0f));
    QCOMPARE(camera.viewSize(), QSizeF(0.05f, 1.0f));
    camera.setViewSize(QSizeF(-0.01f, 1.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-0.05f, 1.0f));
    camera.setViewSize(QSizeF(-1.0f, 0.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-1.0f, 0.05f));
    camera.setViewSize(QSizeF(-1.0f, -0.01f));
    QCOMPARE(camera.viewSize(), QSizeF(-1.0f, -0.05f));
}

QTEST_APPLESS_MAIN(tst_QGLCamera)

#include "tst_qglcamera.moc"
