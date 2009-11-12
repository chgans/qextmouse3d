/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SPHERE_H
#define SPHERE_H

#include "item3d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class SpherePrivate;

class Sphere : public Item3d
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(qreal size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(qreal faces READ faces WRITE setFaces NOTIFY facesChanged)
public:
    Sphere(QObject *parent = 0);
    ~Sphere();

    enum Type
    {
        Ico,
        Cube,
        UV
    };

    Type type() const;
    void setType(Type value);

    qreal size() const;
    void setSize(qreal value);

    qreal faces() const;
    void setFaces(qreal value);

protected:
    void drawItem(QGLPainter *painter);

Q_SIGNALS:
    void typeChanged();
    void sizeChanged();
    void facesChanged();

private:
    SpherePrivate *d;
};

QML_DECLARE_TYPE(Sphere)

QT_END_NAMESPACE

QT_END_HEADER

#endif
