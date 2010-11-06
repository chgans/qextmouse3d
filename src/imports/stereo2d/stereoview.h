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

#ifndef STEREOVIEW_H
#define STEREOVIEW_H

#include <QtDeclarative/qdeclarativeitem.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class StereoViewEffect;

class StereoView : public QDeclarativeItem
{
    Q_OBJECT
    Q_ENUMS(Layout)
    Q_PROPERTY(Layout layout READ layout WRITE setLayout NOTIFY layoutChanged)
    Q_PROPERTY(QGraphicsObject *viewport READ viewport NOTIFY viewportChanged)
    Q_PROPERTY(int eye READ eye WRITE setEye SCRIPTABLE false)
    Q_PROPERTY(QDeclarativeListProperty<QObject> stereoViewData READ stereoViewData DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "stereoViewData")
public:
    StereoView(QDeclarativeItem *parent = 0);
    ~StereoView();

    enum Layout
    {
        Default,
        RedCyan,
        LeftRight,
        RightLeft,
        TopBottom,
        BottomTop,
        Disabled
    };

    enum
    {
        NoEye,
        LeftEye,
        RightEye
    };

    StereoView::Layout layout() const { return m_layout; }
    void setLayout(StereoView::Layout layout);

    // May be different in the future: when layout() is Default,
    // the actualLayout() will indicate the real hardware layout.
    StereoView::Layout actualLayout() const { return m_layout; }

    QGraphicsObject *viewport() const { return m_viewport; }

    int eye() const { return m_eye; }
    void setEye(int eye) { m_eye = eye; }

    QDeclarativeListProperty<QObject> stereoViewData();

    static StereoView *findView(QDeclarativeItem *item);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

Q_SIGNALS:
    void layoutChanged();
    void viewportChanged();

private Q_SLOTS:
    void switchToOpenGL();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    Layout m_layout;
    int m_eye;
    QDeclarativeItem *m_viewport;
    StereoViewEffect *m_effect;

    void updateViewportSize();
};

QML_DECLARE_TYPE(StereoView)

QT_END_NAMESPACE

QT_END_HEADER

#endif
