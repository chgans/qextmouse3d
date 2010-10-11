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

#include "qmouse3deventprovider.h"
#include "qmouse3ddevice_p.h"
#include "qmouse3ddevicelist_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QMouse3DEventProvider
    \brief The QMouse3DEventProvider class handles 3D mouse devices, configuring delivery of events to widgets.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::viewing

    3D mice usually have six degrees of freedom of motion, corresponding
    to translations along the X, Y, and Z axes, and rotations around
    each of those axes.  Such mice also typically have an array of
    special purpose buttons for 3D navigation operations.

    QMouse3DEventProvider simplifies the process of interfacing a QWidget
    to a 3D mouse device.  Once initialized, the application can detect
    the presence of a 3D mouse with isAvailable() and availableChanged().
    If there are multiple 3D mice attached to the machine, then this
    class will make it appear as though there is a single event source.

    Motions in 3D are delivered to widget() in the form of a QMouse3DEvent
    and special purpose buttons are delivered to widget() in the form of a
    QKeyEvent.  Any key code that appears in the Qt::Key enumeration may
    be delivered from a 3D mouse.  In addition, the QGL::Mouse3DKeys
    enumeration defines extra key codes that are specific to 3D mice.

    Multiple QMouse3DEventProvider objects can be created by an
    application, each associated with a separate widget().  Internally,
    the widget that currently has focus will receive 3D mouse events.
    QMouse3DEventProvider can also be used without a widget to
    obtain information about the 3D mice that are attached to the machine.

    Applications use QMouse3DEventProvider to register a QWidget to
    receive 3D mouse events.  The events will arrive via the
    QObject::event() and QWidget::keyPressEvent() overrides in
    the widget subclass:

    \code
    MyWidget::MyWidget(QWidget *parent)
        : QWidget(parent)
    {
        QMouse3DEventProvider *provider;
        provider = new QMouse3DEventProvider(this);
        provider->setWidget(this);
    }

    bool MyWidget::event(QEvent *e)
    {
        if (e->type() == QMouse3DEvent::type) {
            QMouse3DEvent *mouse = static_cast<QMouse3DEvent *>(e);
            ...
            return true;
        }
        return QWidget::event(e);
    }

    void MyWidget::keyPressEvent(QKeyEvent *e)
    {
        if (e->key() == QGL::Key_TopView) {
            ...
        }
        QWidget::keyPressEvent(e);
    }
    \endcode

    The setFilters() and setSensitivity() functions can be used
    to filter 3D mouse events before they are delivered to the
    widget(), which can help the user navigate through 3D space
    more reliably.

    \sa QMouse3DEvent
*/

class QMouse3DEventProviderPrivate
{
public:
    QMouse3DEventProviderPrivate()
        : widget(0)
        , filters(QMouse3DEventProvider::Translations |
                  QMouse3DEventProvider::Rotations |
                  QMouse3DEventProvider::Sensitivity)
        , keyFilters(QMouse3DEventProvider::AllFilters)
        , sensitivity(1.0f)
    {
        devices = QMouse3DDeviceList::attach();
    }
    ~QMouse3DEventProviderPrivate()
    {
        QMouse3DDeviceList::detach(devices);
    }

    QWidget *widget;
    QMouse3DDeviceList *devices;
    QMouse3DEventProvider::Filters filters;
    QMouse3DEventProvider::Filters keyFilters;
    qreal sensitivity;
};

/*!
    Constructs an event provider for the 3D mice attached to this
    machine and associates it with \a parent.
*/
QMouse3DEventProvider::QMouse3DEventProvider(QObject *parent)
    : QObject(parent)
    , d_ptr(new QMouse3DEventProviderPrivate)
{
    Q_D(QMouse3DEventProvider);
    connect(d->devices, SIGNAL(availableChanged()),
            this, SIGNAL(availableChanged()));
}

/*!
    Destroys this 3D mouse event provider.
*/
QMouse3DEventProvider::~QMouse3DEventProvider()
{
    Q_D(QMouse3DEventProvider);
    if (d->widget)
        d->devices->detachWidget(this, d->widget);
}

/*!
    Returns true if a 3D mouse device is currently available for
    use by the application; false otherwise.

    If there are multiple 3D mice attached to the machine, this will
    return true if any single one of them is available.

    \sa availableChanged(), deviceNames()
*/
bool QMouse3DEventProvider::isAvailable() const
{
    Q_D(const QMouse3DEventProvider);
    QList<QMouse3DDevice *> devices = d->devices->devices;
    for (int index = 0; index < devices.size(); ++index) {
        if (devices[index]->isAvailable())
            return true;
    }
    return false;
}

/*!
    Returns the list of all 3D mouse devices that are attached to
    the machine, associated with this event provider, and which
    are currently available.  This list is intended for debugging
    purposes.

    \sa isAvailable()
*/
QStringList QMouse3DEventProvider::deviceNames() const
{
    Q_D(const QMouse3DEventProvider);
    QList<QMouse3DDevice *> devices = d->devices->devices;
    QStringList names;
    for (int index = 0; index < devices.size(); ++index) {
        if (devices[index]->isAvailable())
            names += devices[index]->deviceNames();
    }
    return names;
}

/*!
    Returns the widget that will receive QMouse3DEvent and QKeyEvent
    events from the machine's 3D mouse device; or null if the widget
    has not yet been set.

    \sa setWidget()
*/
QWidget *QMouse3DEventProvider::widget() const
{
    Q_D(const QMouse3DEventProvider);
    return d->widget;
}

/*!
    Sets the \a widget that will receive QMouse3DEvent and QKeyEvent
    events from the machine's 3D mouse device.

    \sa widget()
*/
void QMouse3DEventProvider::setWidget(QWidget *widget)
{
    Q_D(QMouse3DEventProvider);
    if (d->widget != widget) {
        if (d->widget)
            d->devices->detachWidget(this, d->widget);
        d->widget = widget;
        if (widget)
            d->devices->attachWidget(this, widget);
    }
}

/*!
    \enum QMouse3DEventProvider::Filter
    This enum defines filters that can be applied to incoming
    3D mouse events to constrain the reported axes.

    \value NoFilters Special value with no filter bits set.
    \value Translations Report translation axes.
    \value Rotations Report rotation axes.
    \value DominantAxis Report only the most dominant axis.
    \value Sensitivity Apply sensitivity() to the axes.
    \value AllFilters Special value with all filter bits set.
*/

/*!
    Returns the currently active event filters for widget().
    The default value is \l Translations | \l Rotations | \l Sensitivity.

    \sa setFilters(), toggleFilter(), filtersChanged(), keyFilters()
*/
QMouse3DEventProvider::Filters QMouse3DEventProvider::filters() const
{
    Q_D(const QMouse3DEventProvider);
    return d->filters;
}

/*!
    Sets the list of \a filters to apply to incoming 3D mouse events
    on widget().

    \sa filters(), toggleFilter(), filtersChanged()
*/
void QMouse3DEventProvider::setFilters
    (QMouse3DEventProvider::Filters filters)
{
    Q_D(QMouse3DEventProvider);
    if ((filters & (Translations | Rotations)) == 0)
        filters |= Rotations;   // Need at least 1 of these set.
    if (d->filters != filters) {
        d->filters = filters;
        d->devices->updateFilters(this, filters);
        emit filtersChanged();
    }
}

/*!
    Toggles the state of \a filter within filters() for widget().
    This function will ensure that one of \l Translations
    and \l Rotations is always enabled.

    \sa filters(), setFilters(), filtersChanged()
*/
void QMouse3DEventProvider::toggleFilter
    (QMouse3DEventProvider::Filter filter)
{
    Q_D(QMouse3DEventProvider);
    QMouse3DEventProvider::Filters newFilters = d->filters ^ filter;
    if ((newFilters & (QMouse3DEventProvider::Translations |
                       QMouse3DEventProvider::Rotations)) == 0) {
        // Cannot turn off both Translations and Rotations, so turn
        // on the other one that wasn't toggled.
        if (filter == QMouse3DEventProvider::Translations)
            newFilters |= QMouse3DEventProvider::Rotations;
        else if (filter == QMouse3DEventProvider::Rotations)
            newFilters |= QMouse3DEventProvider::Translations;
    }
    setFilters(newFilters);
}

/*!
    Returns the filters that can be modified by special keys on the
    mouse device.  The default is AllFilters.

    If a key-based filter is disabled, then only the application
    can modify the filter.  This can be useful when the application
    only wants the translation or rotation components from the
    3D mouse and does not want the user to alter this restriction
    using the special keys.

    Special keys will still be delivered to widget() as a QKeyEvent,
    even if the corresponding filter bit is disabled.

    \sa setKeyFilters(), filters()
*/
QMouse3DEventProvider::Filters QMouse3DEventProvider::keyFilters() const
{
    Q_D(const QMouse3DEventProvider);
    return d->keyFilters;
}

/*!
    Sets the \a filters that can be modified by special keys on
    the mouse device.

    \sa keyFilters()
*/
void QMouse3DEventProvider::setKeyFilters
    (QMouse3DEventProvider::Filters filters)
{
    Q_D(QMouse3DEventProvider);
    d->keyFilters = filters;
}

/*!
    Returns the sensitivity of the 3D mouse to wrist movements.
    Larger values have a large effect on the reported events,
    making the mouse more sensitive to small wrist movements.
    Smaller values make the mouse less sensitive to small wrist
    movements, which can help with the accuracy of fine-detail work.

    The return value ranges between 1/64 and 64, with the default
    value of 1 indicating the normal state where events are reported
    without any adjustment.  A value of 4 indicates that the
    mouse is 4 times more sensitive than normal, and a value of 1/4
    (0.25) indicates that the mouse is 4 times less sensitive
    than normal.

    \sa setSensitivity(), sensitivityChanged()
*/
qreal QMouse3DEventProvider::sensitivity() const
{
    Q_D(const QMouse3DEventProvider);
    return d->sensitivity;
}

/*!
    Sets the sensitivity of the 3D mouse to wrist movements
    to \a value, which is clamped to the range 1/64 to 64.
    The default value of 1 indicates the normal state where events
    are reported without any adjustment.

    The \a value is multiplied by incoming axis values to
    generate the components of the QMouse3DEvent.  A \a value of 4
    indicates that the mouse is four times more sensitive than
    normal, and a \a value of 1/4 (0.25) indicates that the mouse
    is four times less sensitive than normal.

    \sa sensitivity(), sensitivityChanged()
*/
void QMouse3DEventProvider::setSensitivity(qreal value)
{
    Q_D(QMouse3DEventProvider);

    // Clamp the value to the range 1/64 to 64.
    value = qMin(qMax(value, qreal(1.0f / 64.0f)), qreal(64.0f));
    if (d->sensitivity != value) {
        d->sensitivity = value;
        d->devices->updateSensitivity(this, d->sensitivity);
        emit sensitivityChanged();
    }
}

/*!
    \fn void QMouse3DEventProvider::availableChanged()

    Signal that is emitted when the set of available 3D mouse
    devices changes.  This can be used by an application to detect
    when a 3D mouse device is plugged in or unplugged.

    If there are multiple 3D mice attached to the machine,
    availableChanged() will be emitted whenever any one of them
    is plugged in or unplugged.  Use deviceNames() to fetch the
    names of all devices that are attached to the machine at present.

    \sa isAvailable(), deviceNames()
*/

/*!
    \fn void QMouse3DEventProvider::filtersChanged()

    This signal is emitted when filters() changes.

    \sa filters(), setFilters(), toggleFilter()
*/

/*!
    \fn void QMouse3DEventProvider::sensitivityChanged()

    This signal is emitted when sensitivity() changes.

    \sa sensitivity(), setSensitivity()
*/

QT_END_NAMESPACE
