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
    QCOMPARE(camera.screenRotation(), 0);
    QVERIFY(camera.eye() == QVector3D(0, 0, 10));
    QVERIFY(camera.upVector() == QVector3D(0, 1, 0));
    QVERIFY(camera.center() == QVector3D(0, 0, 0));
    QCOMPARE(camera.eyeSeparation(), (qreal)0.0f);
}

void tst_QGLCamera::modify()
{
    // Test modifying each field individually.
    QGLCamera camera;
    camera.setProjectionType(QGLCamera::Orthographic);
    QVERIFY(camera.projectionType() == QGLCamera::Orthographic);
    camera.setFieldOfView(60.0f);
    QCOMPARE(camera.fieldOfView(), (qreal)60.0f);
    camera.setNearPlane(-3.0f);
    QCOMPARE(camera.nearPlane(), (qreal)-3.0f);
    camera.setFarPlane(3000.0f);
    QCOMPARE(camera.farPlane(), (qreal)3000.0f);
    camera.setViewSize(QSizeF(45.0f, 25.5f));
    QCOMPARE(camera.viewSize(), QSizeF(45.0f, 25.5f));
    camera.setScreenRotation(270);
    QCOMPARE(camera.screenRotation(), 270);
    camera.setEye(QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    camera.setUpVector(QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    camera.setCenter(QVector3D(7.0f, 8.0f, 9.0f));
    QVERIFY(camera.center() == QVector3D(7.0f, 8.0f, 9.0f));
    camera.setEyeSeparation(3.0f);
    QCOMPARE(camera.eyeSeparation(), (qreal)3.0f);

    // Test that we don't get any side effects between properties.
    QVERIFY(camera.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(camera.fieldOfView(), (qreal)60.0f);
    QCOMPARE(camera.nearPlane(), (qreal)-3.0f);
    QCOMPARE(camera.farPlane(), (qreal)3000.0f);
    QCOMPARE(camera.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(camera.screenRotation(), 270);
    QVERIFY(camera.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera.center() == QVector3D(7.0f, 8.0f, 9.0f));
    QCOMPARE(camera.eyeSeparation(), (qreal)3.0f);

#if 0 // FIXME
    // Copy and test that the copied version is the same.
    QGLCamera camera2(camera);
    QVERIFY(camera2.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(camera2.fieldOfView(), (qreal)60.0f);
    QCOMPARE(camera2.nearPlane(), (qreal)-3.0f);
    QCOMPARE(camera2.farPlane(), (qreal)3000.0f);
    QCOMPARE(camera2.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(camera2.screenRotation(), 270);
    QVERIFY(camera2.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera2.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera2.center() == QVector3D(7.0f, 8.0f, 9.0f));

    QGLCamera camera3;
    camera3 = camera;
    QVERIFY(camera3.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(camera3.fieldOfView(), (qreal)60.0f);
    QCOMPARE(camera3.nearPlane(), (qreal)-3.0f);
    QCOMPARE(camera3.farPlane(), (qreal)3000.0f);
    QCOMPARE(camera3.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(camera3.screenRotation(), 270);
    QVERIFY(camera3.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera3.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera3.center() == QVector3D(7.0f, 8.0f, 9.0f));

    // Modify the copy and check that the original stays the same.
    camera2.setProjectionType(QGLCamera::Perspective);
    camera2.setFieldOfView(45.0f);
    camera2.setNearPlane(3.0f);
    camera2.setFarPlane(-3000.0f);
    camera2.setViewSize(QSizeF(2.0f, 2.5f));
    camera2.setScreenRotation(90);
    QVERIFY(camera.projectionType() == QGLCamera::Orthographic);
    QCOMPARE(camera.fieldOfView(), (qreal)60.0f);
    QCOMPARE(camera.nearPlane(), (qreal)-3.0f);
    QCOMPARE(camera.farPlane(), (qreal)3000.0f);
    QCOMPARE(camera.viewSize(), QSizeF(45.0f, 25.5f));
    QCOMPARE(camera.screenRotation(), 270);
    camera2.setEye(QVector3D(-1.0f, -2.0f, -3.0f));
    camera2.setUpVector(QVector3D(-4.0f, -5.0f, -6.0f));
    camera2.setCenter(QVector3D(-7.0f, -8.0f, -9.0f));
    QVERIFY(camera.eye() == QVector3D(1.0f, 2.0f, 3.0f));
    QVERIFY(camera.upVector() == QVector3D(4.0f, 5.0f, 6.0f));
    QVERIFY(camera.center() == QVector3D(7.0f, 8.0f, 9.0f));
#endif
}

// Check that the minimum view size works correctly, including when
// the view size goes negative (e.g. for flipped y co-ordinates).
void tst_QGLCamera::minViewSize()
{
    QGLCamera camera;
    camera.setViewSize(QSizeF(-20.0f, -30.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-20.0f, -30.0f));
    camera.setViewSize(QSizeF(0.0f, 1.0f));
    QCOMPARE(camera.viewSize(), QSizeF(0.0001f, 1.0f));
    camera.setViewSize(QSizeF(-1.0f, 0.0f));
    QCOMPARE(camera.viewSize(), QSizeF(-1.0f, 0.0001f));
}

QTEST_APPLESS_MAIN(tst_QGLCamera)

#include "tst_qglcamera.moc"
