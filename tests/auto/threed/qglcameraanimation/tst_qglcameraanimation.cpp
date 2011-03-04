/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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
#include "qglcameraanimation.h"
#include "qglcamera.h"

class tst_QGLCameraAnimation : public QObject
{
    Q_OBJECT
public:
    tst_QGLCameraAnimation() {}
    ~tst_QGLCameraAnimation() {}

private slots:
    void properties();
    void animationStartEnd();
};

// Will try to wait for the condition while allowing event processing
#define QTRY_VERIFY(__expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)

// Will try to wait for the condition while allowing event processing
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)

void tst_QGLCameraAnimation::properties()
{
    // Check the default property values.
    QGLCameraAnimation anim;
    QVERIFY(anim.camera() == 0);
    QVERIFY(anim.startEye() == QVector3D(0.0f, 0.0f, 10.0f));
    QVERIFY(anim.startUpVector() == QVector3D(0.0f, 1.0f, 0.0f));
    QVERIFY(anim.startCenter() == QVector3D(0.0f, 0.0f, 0.0f));
    QVERIFY(anim.endEye() == QVector3D(0.0f, 0.0f, 10.0f));
    QVERIFY(anim.endUpVector() == QVector3D(0.0f, 1.0f, 0.0f));
    QVERIFY(anim.endCenter() == QVector3D(0.0f, 0.0f, 0.0f));
    QCOMPARE(anim.duration(), 250);
    QVERIFY(anim.easingCurve() == QEasingCurve());

    // Change the properties.
    QGLCamera camera;
    anim.setCamera(&camera);
    anim.setStartEye(QVector3D(1, -2, 3));
    anim.setStartUpVector(QVector3D(-4, 5, -6));
    anim.setStartCenter(QVector3D(7, -8, 9));
    anim.setEndEye(QVector3D(-1, 2, -3));
    anim.setEndUpVector(QVector3D(4, -5, 6));
    anim.setEndCenter(QVector3D(-7, 8, -9));
    anim.setDuration(500);
    anim.setEasingCurve(QEasingCurve::OutQuad);

    // Check that the properties have their new values.
    QVERIFY(anim.camera() == &camera);
    QVERIFY(anim.startEye() == QVector3D(1, -2, 3));
    QVERIFY(anim.startUpVector() == QVector3D(-4, 5, -6));
    QVERIFY(anim.startCenter() == QVector3D(7, -8, 9));
    QVERIFY(anim.endEye() == QVector3D(-1, 2, -3));
    QVERIFY(anim.endUpVector() == QVector3D(4, -5, 6));
    QVERIFY(anim.endCenter() == QVector3D(-7, 8, -9));
    QCOMPARE(anim.duration(), 500);
    QVERIFY(anim.easingCurve() == QEasingCurve(QEasingCurve::OutQuad));
}

void tst_QGLCameraAnimation::animationStartEnd()
{
    QGLCameraAnimation anim;
    QGLCamera camera;
    QGLCamera camera2;

    anim.setCamera(&camera);
    anim.setStartEye(QVector3D(1, -2, 3));
    anim.setStartUpVector(QVector3D(-4, 5, -6));
    anim.setStartCenter(QVector3D(7, -8, 9));
    anim.setEndEye(QVector3D(-1, 2, -3));
    anim.setEndUpVector(QVector3D(4, -5, 6));
    anim.setEndCenter(QVector3D(-7, 8, -9));
    anim.setDuration(1);

    // Verify the start state.
    anim.start();
    QVERIFY(camera.eye() == anim.startEye());
    QVERIFY(camera.upVector() == anim.startUpVector());
    QVERIFY(camera.center() == anim.startCenter());

    // Wait for the animation to end
    QTRY_COMPARE(anim.state(), QAbstractAnimation::Stopped);
    QVERIFY(camera.eye() == anim.endEye());
    QVERIFY(camera.upVector() == anim.endUpVector());
    QVERIFY(camera.center() == anim.endCenter());

    // Zero duration animation instantly transitions to the end state.
    anim.setCamera(&camera2);
    anim.setDuration(0);
    anim.start();
    QVERIFY(camera2.eye() == anim.endEye());
    QVERIFY(camera2.upVector() == anim.endUpVector());
    QVERIFY(camera2.center() == anim.endCenter());
}

QTEST_MAIN(tst_QGLCameraAnimation)

#include "tst_qglcameraanimation.moc"
