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

#ifndef QSGSTEREOCONTEXT_P_H
#define QSGSTEREOCONTEXT_P_H

#include <QtDeclarative/qsgcontext.h>
#include "qt3dquickglobal.h"
#include "qglnamespace.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSGStereoContextPrivate;

class Q_QT3D_QUICK_EXPORT QSGStereoContext : public QSGContext
{
    Q_OBJECT
    // These properties exist to allow QML plugins that don't link
    // against the Qt3DQuick library to determine the stereo settings.
    Q_PROPERTY(bool isStereo READ isStereo)
    Q_PROPERTY(int eye READ eyeInt)
public:
    explicit QSGStereoContext(const QString &key, QObject *parent = 0);
    ~QSGStereoContext();

    bool isStereo() const { return true; }
    QGL::Eye eye() const;

    Renderer *createRenderer();
    void renderNextFrame();

protected:
    void setEye(QGL::Eye eye);

private:
    int eyeInt() const { return int(eye()); }

    QScopedPointer<QSGStereoContextPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QSGStereoContext)
    Q_DISABLE_COPY(QSGStereoContext)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
