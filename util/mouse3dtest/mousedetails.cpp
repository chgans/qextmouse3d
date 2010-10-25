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

#include "mousedetails.h"
#include "qmouse3devent.h"
#include "qglnamespace.h"
#include <QtCore/qmath.h>
#include <QtCore/qdebug.h>

MouseDetails::MouseDetails(QWidget *parent)
    : QWidget(parent)
    , changingFilter(false)
    , changingSensitivity(false)
{
    setupUi(this);

    mouse = new QMouse3DEventProvider(this);
    mouse->setWidget(this);
    connect(mouse, SIGNAL(availableChanged()),
            this, SLOT(availableChanged()));
    connect(mouse, SIGNAL(filtersChanged()),
            this, SLOT(filtersChanged()));
    connect(mouse, SIGNAL(sensitivityChanged()),
            this, SLOT(sensitivityChanged()));

    connect(translations, SIGNAL(clicked()),
            this, SLOT(toggleTranslations()));
    connect(rotations, SIGNAL(clicked()),
            this, SLOT(toggleRotations()));
    connect(dominantAxis, SIGNAL(clicked()),
            this, SLOT(toggleDominantAxis()));
    connect(sensitivity, SIGNAL(valueChanged(int)),
            this, SLOT(sliderChanged(int)));

    clearKeyTimer = new QTimer(this);
    clearKeyTimer->setSingleShot(true);
    connect(clearKeyTimer, SIGNAL(timeout()), this, SLOT(clearKeyName()));

    availableChanged();
    filtersChanged();
    sensitivityChanged();
}

MouseDetails::~MouseDetails()
{
}

void MouseDetails::availableChanged()
{
    if (mouse->isAvailable())
        deviceName->setText(mouse->deviceNames().join(QLatin1String(", ")));
    else
        deviceName->setText(tr("None"));
    translateX->setText(QLatin1String("0"));
    translateY->setText(QLatin1String("0"));
    translateZ->setText(QLatin1String("0"));
    rotateX->setText(QLatin1String("0"));
    rotateY->setText(QLatin1String("0"));
    rotateZ->setText(QLatin1String("0"));
    key->setText(QString());
}

void MouseDetails::filtersChanged()
{
    changingFilter = true;
    QMouse3DEventProvider::Filters filters = mouse->filters();
    translations->setChecked
        ((filters & QMouse3DEventProvider::Translations) != 0);
    rotations->setChecked
        ((filters & QMouse3DEventProvider::Rotations) != 0);
    dominantAxis->setChecked
        ((filters & QMouse3DEventProvider::DominantAxis) != 0);
    changingFilter = false;
}

static inline qreal qLog2(qreal x)
{
#ifdef Q_OS_SYMBIAN
    TReal result, lg2;
    Math::Ln(result, static_cast<TReal>(x));
    Math::Ln(lg2, static_cast<TReal>(2.0f));
    return static_cast<qreal>(result) / static_cast<qreal>(lg2);
#else
    return ::log(x) / ::log(2.0f);
#endif
}

void MouseDetails::sensitivityChanged()
{
    // Convert the logarithmic value between 1/64 to 64 into a
    // linear value for the slider between 0 and 120.
    if (!changingSensitivity) {
        changingSensitivity = true;
        qreal sensValue = mouse->sensitivity();
        qreal value = qreal((qLog2(sensValue) + 6.0f) * 10.0f);
        sensitivity->setValue(int(value));
        changingSensitivity = false;
    }
}

void MouseDetails::clearKeyName()
{
    key->setText(QString());
}

void MouseDetails::toggleTranslations()
{
    if (!changingFilter)
        mouse->toggleFilter(QMouse3DEventProvider::Translations);
}

void MouseDetails::toggleRotations()
{
    if (!changingFilter)
        mouse->toggleFilter(QMouse3DEventProvider::Rotations);
}

void MouseDetails::toggleDominantAxis()
{
    if (!changingFilter)
        mouse->toggleFilter(QMouse3DEventProvider::DominantAxis);
}

void MouseDetails::sliderChanged(int value)
{
    if (!changingSensitivity) {
        // Convert the slider value from 0 to 120 into a
        // logarithmic sensitivity value between 1/64 and 64.
        changingSensitivity = true;
        qreal sensValue = qPow(qreal(2.0f), qreal((value - 60) / 10.0f));
        mouse->setSensitivity(sensValue);
        changingSensitivity = false;
    }
}

bool MouseDetails::event(QEvent *e)
{
    if (e->type() == QMouse3DEvent::type) {
        QMouse3DEvent *event = static_cast<QMouse3DEvent *>(e);
        translateX->setText(QString::number(event->translateX()));
        translateY->setText(QString::number(event->translateY()));
        translateZ->setText(QString::number(event->translateZ()));
        rotateX->setText(QString::number(event->rotateX()));
        rotateY->setText(QString::number(event->rotateY()));
        rotateZ->setText(QString::number(event->rotateZ()));

        // Determine which axis is dominant.
        int values[6] = {event->translateX(), event->translateY(),
                         event->translateZ(), event->rotateX(),
                         event->rotateY(), event->rotateZ()};
        int largest = 0;
        int value = qAbs(values[0]);
        for (int index = 1; index < 6; ++index) {
            int value2 = qAbs(values[index]);
            if (value2 > value) {
                largest = index;
                value = value2;
            }
        }
        switch (largest) {
        case 0:
            if (values[0] < 0)
                dominantMovement->setText(tr("Move left"));
            else if (values[0] > 0)
                dominantMovement->setText(tr("Move right"));
            else
                dominantMovement->setText(tr("Centered"));
            break;
        case 1:
            if (values[1] < 0)
                dominantMovement->setText(tr("Move forward"));
            else
                dominantMovement->setText(tr("Move back"));
            break;
        case 2:
            if (values[2] < 0)
                dominantMovement->setText(tr("Pulled up"));
            else
                dominantMovement->setText(tr("Pushed down"));
            break;
        case 3:
            if (values[3] < 0)
                dominantMovement->setText(tr("Rocked forward"));
            else
                dominantMovement->setText(tr("Rocked back"));
            break;
        case 4:
            if (values[4] < 0)
                dominantMovement->setText(tr("Rocked right"));
            else
                dominantMovement->setText(tr("Rocked left"));
            break;
        case 5:
            if (values[5] < 0)
                dominantMovement->setText(tr("Twist left"));
            else
                dominantMovement->setText(tr("Twist right"));
            break;
        }
        return true;
    } else {
        return QWidget::event(e);
    }
}

static const char *const keyNames[] = {
    "Fit",
    "TopView",
    "LeftView",
    "RightView",
    "FrontView",
    "BottomView",
    "BackView",
    "RotateCW90",
    "RotateCCW90",
    "ISO1",
    "ISO2",
    "Button1",
    "Button2",
    "Button3",
    "Button4",
    "Button5",
    "Button6",
    "Button7",
    "Button8",
    "Button9",
    "Button10",
    "0x01200017",
    "0x01200018",
    "0x01200019",
    "0x0120001a",
    "Rotations",
    "Translations",
    "DominantAxis",
    "IncreaseSensitivity",
    "DecreaseSensitivity"
};

void MouseDetails::keyPressEvent(QKeyEvent *e)
{
    if (e->key() >= QGL::Key_Fit && e->key() <= QGL::Key_DecreaseSensitivity)
        key->setText(tr("Key: %1").arg(QLatin1String(keyNames[e->key() - QGL::Key_Fit])));
    else if (e->key() == Qt::Key_Menu)
        key->setText(tr("Key: Menu"));
    QWidget::keyPressEvent(e);
}

void MouseDetails::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() >= QGL::Key_Fit && e->key() <= QGL::Key_DecreaseSensitivity) {
        key->setText(tr("Key: %1").arg(QLatin1String(keyNames[e->key() - QGL::Key_Fit])));
        clearKeyTimer->start(2000);
    } else if (e->key() == Qt::Key_Menu) {
        key->setText(tr("Key: Menu"));
        clearKeyTimer->start(2000);
    }
    QWidget::keyReleaseEvent(e);
}
