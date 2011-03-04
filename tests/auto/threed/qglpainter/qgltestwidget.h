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

#ifndef QGLTESTWIDGET_H
#define QGLTESTWIDGET_H

#include <QtOpenGL/qgl.h>

class QEventLoop;

class QGLTestWidget : public QGLWidget
{
    Q_OBJECT
public:
    QGLTestWidget(QWidget *parent = 0);
    ~QGLTestWidget();

    bool runTest(QObject *target, const char *method, int timeout = 5000);

    QImage actualOutput() const { return capture; }
    QImage simulatedOutput() const { return simulated; }

protected:
    void paintGL();

private:
    QEventLoop *eventLoop;
    bool paintDone;
    QImage capture;
    QImage simulated;
    QObject *target;
    const char *method;
};

#endif
