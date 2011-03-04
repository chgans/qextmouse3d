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
#include "qmouse3devent.h"
#include "qmouse3deventprovider.h"
#include "qmouse3ddevice_p.h"
#include "qglnamespace.h"
#include <QtGui/qevent.h>

class TestMouse3DDevice;

class tst_QMouse3DEvent : public QObject
{
    Q_OBJECT
public:
    tst_QMouse3DEvent() : device1(0), device2(0) {}
    ~tst_QMouse3DEvent() {}

private slots:
    void initTestCase();
    void createEvent();
    void availableDevice();
    void deliverEvents();
    void filterEvents();

private:
    TestMouse3DDevice *device1;
    TestMouse3DDevice *device2;
};

// Dummy 3D mouse device for simulating the behavior of actual devices.
class TestMouse3DDevice : public QMouse3DDevice
{
    Q_OBJECT
public:
    TestMouse3DDevice(QObject *parent = 0);
    ~TestMouse3DDevice() {}

    bool isAvailable() const { return available; }
    void setAvailable(bool value)
        { available = value; emit availableChanged(); }

    QStringList deviceNames() const { return names; }
    void setDeviceNames(const QStringList &value) { names = value; }

    void sendMotion(QMouse3DEvent *event) { motion(event); }
    void sendKeyPress(int key) { keyPress(key); }
    void sendKeyRelease(int key) { keyRelease(key); }

private:
    bool available;
    QStringList names;
};

TestMouse3DDevice::TestMouse3DDevice(QObject *parent)
    : QMouse3DDevice(parent)
    , available(false)
{
}

// Test widget for receiving 3D mouse events.
class TestMouse3DWidget : public QWidget
{
    Q_OBJECT
public:
    TestMouse3DWidget(QWidget *parent = 0);
    ~TestMouse3DWidget() {}

    int motionsSeen;
    int translateX;
    int translateY;
    int translateZ;
    int rotateX;
    int rotateY;
    int rotateZ;
    int keyPressesSeen;
    int keyPressed;
    int keyReleasesSeen;
    int keyReleased;

    void clear();

protected:
    bool event(QEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
};

TestMouse3DWidget::TestMouse3DWidget(QWidget *parent)
    : QWidget(parent)
{
    clear();
}

void TestMouse3DWidget::clear()
{
    motionsSeen = 0;
    translateX = 0;
    translateY = 0;
    translateZ = 0;
    rotateX = 0;
    rotateY = 0;
    rotateZ = 0;
    keyPressesSeen = 0;
    keyPressed = 0;
    keyReleasesSeen = 0;
    keyReleased = 0;
}

void TestMouse3DWidget::keyPressEvent(QKeyEvent *e)
{
    ++keyPressesSeen;
    keyPressed = e->key();
    QWidget::keyPressEvent(e);
}

void TestMouse3DWidget::keyReleaseEvent(QKeyEvent *e)
{
    ++keyReleasesSeen;
    keyReleased = e->key();
    QWidget::keyReleaseEvent(e);
}

bool TestMouse3DWidget::event(QEvent *e)
{
    if (e->type() == QMouse3DEvent::type) {
        QMouse3DEvent *event = static_cast<QMouse3DEvent *>(e);
        ++motionsSeen;
        translateX = event->translateX();
        translateY = event->translateY();
        translateZ = event->translateZ();
        rotateX = event->rotateX();
        rotateY = event->rotateY();
        rotateZ = event->rotateZ();
    }
    return QWidget::event(e);
}

void tst_QMouse3DEvent::initTestCase()
{
    device1 = new TestMouse3DDevice(this);
    device2 = new TestMouse3DDevice(this);
    QMouse3DDevice::testDevice1 = device1;
    QMouse3DDevice::testDevice2 = device2;
}

void tst_QMouse3DEvent::createEvent()
{
    QMouse3DEvent event(1, -2, 3, -4, 5, -6);
    QCOMPARE(event.translateX(), short(1));
    QCOMPARE(event.translateY(), short(-2));
    QCOMPARE(event.translateZ(), short(3));
    QCOMPARE(event.rotateX(), short(-4));
    QCOMPARE(event.rotateY(), short(5));
    QCOMPARE(event.rotateZ(), short(-6));
}

void tst_QMouse3DEvent::availableDevice()
{
    QVERIFY(device1 != 0);
    QVERIFY(device2 != 0);

    QMouse3DEventProvider provider;
    QVERIFY(!provider.isAvailable());
    QVERIFY(provider.deviceNames().isEmpty());

    QStringList names1;
    names1 += QLatin1String("foo");
    names1 += QLatin1String("bar");

    QStringList names2;
    names2 += QLatin1String("baz");
    device2->setDeviceNames(names2);

    QSignalSpy spy1(&provider, SIGNAL(availableChanged()));
    device1->setDeviceNames(names1);
    device1->setAvailable(true);
    QCOMPARE(spy1.size(), 1);
    QVERIFY(provider.isAvailable());
    QCOMPARE(provider.deviceNames(), names1);

    device2->setAvailable(true);
    QCOMPARE(spy1.size(), 2);
    QVERIFY(provider.isAvailable());
    QCOMPARE(provider.deviceNames(), names1 + names2);

    device1->setAvailable(false);
    QCOMPARE(spy1.size(), 3);
    QVERIFY(provider.isAvailable());
    QCOMPARE(provider.deviceNames(), names2);

    device2->setAvailable(false);
    QCOMPARE(spy1.size(), 4);
    QVERIFY(!provider.isAvailable());
    QVERIFY(provider.deviceNames().isEmpty());

    // Leave the first device available for the following tests.
    device1->setAvailable(true);
}

void tst_QMouse3DEvent::deliverEvents()
{
    TestMouse3DWidget widget;
    QMouse3DEventProvider provider;
    provider.setWidget(&widget);

    QVERIFY(device1->widget() == &widget);
    QVERIFY(device2->widget() == 0);    // device not available

    QMouse3DEvent event(1, -2, 3, -4, 5, -6);
    device1->sendMotion(&event);

    QCOMPARE(widget.motionsSeen, 1);
    QCOMPARE(widget.translateX, 1);
    QCOMPARE(widget.translateY, -2);
    QCOMPARE(widget.translateZ, 3);
    QCOMPARE(widget.rotateX, -4);
    QCOMPARE(widget.rotateY, 5);
    QCOMPARE(widget.rotateZ, -6);

    device1->sendKeyPress(QGL::Key_TopView);
    QCOMPARE(widget.keyPressesSeen, 1);
    QCOMPARE(widget.keyPressed, int(QGL::Key_TopView));

    device1->sendKeyRelease(QGL::Key_TopView);
    QCOMPARE(widget.keyReleasesSeen, 1);
    QCOMPARE(widget.keyReleased, int(QGL::Key_TopView));
}

void tst_QMouse3DEvent::filterEvents()
{
    TestMouse3DWidget widget;
    QMouse3DEventProvider provider;
    provider.setWidget(&widget);

    QVERIFY(device1->widget() == &widget);
    QVERIFY(device2->widget() == 0);    // device not available

    QSignalSpy filterSpy(&provider, SIGNAL(filtersChanged()));
    QSignalSpy sensitivitySpy(&provider, SIGNAL(sensitivityChanged()));

    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Rotations |
                 QMouse3DEventProvider::Sensitivity));
    QCOMPARE(provider.sensitivity(), qreal(1.0f));

    QMouse3DEvent event(10, 20, 30, 40, 50, 60);

    // Toggle rotation mode (which will turn it off).
    provider.toggleFilter(QMouse3DEventProvider::Rotations);

    device1->sendMotion(&event);
    QCOMPARE(widget.motionsSeen, 1);
    QCOMPARE(widget.translateX, 10);
    QCOMPARE(widget.translateY, 20);
    QCOMPARE(widget.translateZ, 30);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Sensitivity));
    QCOMPARE(provider.sensitivity(), qreal(1.0f));
    QCOMPARE(filterSpy.size(), 1);
    QCOMPARE(sensitivitySpy.size(), 0);

    // Turn translation mode off (and rotation mode back on again).
    provider.toggleFilter(QMouse3DEventProvider::Translations);

    device1->sendMotion(&event);
    QCOMPARE(widget.motionsSeen, 2);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 40);
    QCOMPARE(widget.rotateY, 50);
    QCOMPARE(widget.rotateZ, 60);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Rotations |
                 QMouse3DEventProvider::Sensitivity));
    QCOMPARE(provider.sensitivity(), qreal(1.0f));
    QCOMPARE(filterSpy.size(), 2);
    QCOMPARE(sensitivitySpy.size(), 0);

    // Turn translation mode back on (rotation should stay on).
    provider.toggleFilter(QMouse3DEventProvider::Translations);

    device1->sendMotion(&event);
    QCOMPARE(widget.motionsSeen, 3);
    QCOMPARE(widget.translateX, 10);
    QCOMPARE(widget.translateY, 20);
    QCOMPARE(widget.translateZ, 30);
    QCOMPARE(widget.rotateX, 40);
    QCOMPARE(widget.rotateY, 50);
    QCOMPARE(widget.rotateZ, 60);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Rotations |
                 QMouse3DEventProvider::Sensitivity));
    QCOMPARE(provider.sensitivity(), qreal(1.0f));
    QCOMPARE(filterSpy.size(), 3);
    QCOMPARE(sensitivitySpy.size(), 0);

    // Turn on dominant mode.
    provider.toggleFilter(QMouse3DEventProvider::DominantAxis);

    QMouse3DEvent eventTXN(-100, 20, 30, 40, 50, 60);
    device1->sendMotion(&eventTXN);
    QCOMPARE(widget.motionsSeen, 4);
    QCOMPARE(widget.translateX, -100);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Rotations |
                 QMouse3DEventProvider::Sensitivity |
                 QMouse3DEventProvider::DominantAxis));
    QCOMPARE(provider.sensitivity(), qreal(1.0f));
    QCOMPARE(filterSpy.size(), 4);
    QCOMPARE(sensitivitySpy.size(), 0);

    QMouse3DEvent eventTXP(100, 20, 30, 40, 50, 60);
    device1->sendMotion(&eventTXP);
    QCOMPARE(widget.motionsSeen, 5);
    QCOMPARE(widget.translateX, 100);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventTYN(10, -200, 30, 40, 50, 60);
    device1->sendMotion(&eventTYN);
    QCOMPARE(widget.motionsSeen, 6);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, -200);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventTYP(10, 200, 30, 40, 50, 60);
    device1->sendMotion(&eventTYP);
    QCOMPARE(widget.motionsSeen, 7);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 200);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventTZN(10, 20, -300, 40, 50, 60);
    device1->sendMotion(&eventTZN);
    QCOMPARE(widget.motionsSeen, 8);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, -300);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventTZP(10, 20, 300, 40, 50, 60);
    device1->sendMotion(&eventTZP);
    QCOMPARE(widget.motionsSeen, 9);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 300);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventRXN(10, 20, 30, -400, 50, 60);
    device1->sendMotion(&eventRXN);
    QCOMPARE(widget.motionsSeen, 10);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, -400);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventRXP(10, 20, 30, 400, 50, 60);
    device1->sendMotion(&eventRXP);
    QCOMPARE(widget.motionsSeen, 11);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 400);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventRYN(10, 20, 30, 40, -500, 60);
    device1->sendMotion(&eventRYN);
    QCOMPARE(widget.motionsSeen, 12);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, -500);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventRYP(10, 20, 30, 40, 500, 60);
    device1->sendMotion(&eventRYP);
    QCOMPARE(widget.motionsSeen, 13);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 500);
    QCOMPARE(widget.rotateZ, 0);

    QMouse3DEvent eventRZN(10, 20, 30, 40, 50, -600);
    device1->sendMotion(&eventRZN);
    QCOMPARE(widget.motionsSeen, 14);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, -600);

    QMouse3DEvent eventRZP(10, 20, 30, 40, 50, 600);
    device1->sendMotion(&eventRZP);
    QCOMPARE(widget.motionsSeen, 15);
    QCOMPARE(widget.translateX, 0);
    QCOMPARE(widget.translateY, 0);
    QCOMPARE(widget.translateZ, 0);
    QCOMPARE(widget.rotateX, 0);
    QCOMPARE(widget.rotateY, 0);
    QCOMPARE(widget.rotateZ, 600);

    // Turn off dominant mode and then increase the sensitivity
    // to double its normal value.
    provider.toggleFilter(QMouse3DEventProvider::DominantAxis);
    provider.setSensitivity(2.0f);

    device1->sendMotion(&event);
    QCOMPARE(widget.motionsSeen, 16);
    QCOMPARE(widget.translateX, 20);
    QCOMPARE(widget.translateY, 40);
    QCOMPARE(widget.translateZ, 60);
    QCOMPARE(widget.rotateX, 80);
    QCOMPARE(widget.rotateY, 100);
    QCOMPARE(widget.rotateZ, 120);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Rotations |
                 QMouse3DEventProvider::Sensitivity));
    QCOMPARE(provider.sensitivity(), qreal(2.0f));
    QCOMPARE(filterSpy.size(), 5);
    QCOMPARE(sensitivitySpy.size(), 1);

    // Decrease sensitivity to half its normal value.
    provider.setSensitivity(0.5f);

    device1->sendMotion(&event);
    QCOMPARE(widget.motionsSeen, 17);
    QCOMPARE(widget.translateX, 5);
    QCOMPARE(widget.translateY, 10);
    QCOMPARE(widget.translateZ, 15);
    QCOMPARE(widget.rotateX, 20);
    QCOMPARE(widget.rotateY, 25);
    QCOMPARE(widget.rotateZ, 30);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Rotations |
                 QMouse3DEventProvider::Sensitivity));
    QCOMPARE(provider.sensitivity(), qreal(0.5f));
    QCOMPARE(filterSpy.size(), 5);
    QCOMPARE(sensitivitySpy.size(), 2);

    // Disable sensitivity filtering.
    provider.toggleFilter(QMouse3DEventProvider::Sensitivity);

    device1->sendMotion(&event);
    QCOMPARE(widget.motionsSeen, 18);
    QCOMPARE(widget.translateX, 10);
    QCOMPARE(widget.translateY, 20);
    QCOMPARE(widget.translateZ, 30);
    QCOMPARE(widget.rotateX, 40);
    QCOMPARE(widget.rotateY, 50);
    QCOMPARE(widget.rotateZ, 60);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Rotations));
    QCOMPARE(provider.sensitivity(), qreal(0.5f));
    QCOMPARE(filterSpy.size(), 6);
    QCOMPARE(sensitivitySpy.size(), 2);

    // Return sensitivity to normal.
    provider.toggleFilter(QMouse3DEventProvider::Sensitivity);
    provider.setSensitivity(1.0f);

    device1->sendMotion(&event);
    QCOMPARE(widget.motionsSeen, 19);
    QCOMPARE(widget.translateX, 10);
    QCOMPARE(widget.translateY, 20);
    QCOMPARE(widget.translateZ, 30);
    QCOMPARE(widget.rotateX, 40);
    QCOMPARE(widget.rotateY, 50);
    QCOMPARE(widget.rotateZ, 60);
    QVERIFY(provider.filters() ==
                (QMouse3DEventProvider::Translations |
                 QMouse3DEventProvider::Rotations |
                 QMouse3DEventProvider::Sensitivity));
    QCOMPARE(provider.sensitivity(), qreal(1.0f));
    QCOMPARE(filterSpy.size(), 7);
    QCOMPARE(sensitivitySpy.size(), 3);
}

QTEST_MAIN(tst_QMouse3DEvent)

#include "tst_qmouse3devent.moc"
