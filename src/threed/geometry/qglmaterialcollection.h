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

#ifndef QGLMATERIALCOLLECTION_H
#define QGLMATERIALCOLLECTION_H

#include <QtCore/qobject.h>

#include "qt3dglobal.h"
#include "qglmaterial.h"
#include "qgltexture2d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLMaterialCollectionPrivate;

class Q_QT3D_EXPORT QGLMaterialCollection : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QGLMaterialCollection)
    Q_DISABLE_COPY(QGLMaterialCollection)
public:
    QGLMaterialCollection(QObject *parent = 0);
    virtual ~QGLMaterialCollection();

    QGLMaterial *material(int index) const;
    QGLMaterial *material(const QString &name) const;

    bool contains(QGLMaterial *material) const;
    bool contains(const QString &name) const;

    int indexOf(QGLMaterial *material) const;
    int indexOf(const QString &name) const;

    QString materialName(int index) const;

    bool isMaterialUsed(int index) const;
    void markMaterialAsUsed(int index);
    void removeUnusedMaterials();

    int addMaterial(QGLMaterial *material);
    void removeMaterial(QGLMaterial *material);
    QGLMaterial *removeMaterial(int index);

    bool isEmpty() const;
    int size() const;

private Q_SLOTS:
    void materialDeleted();

private:
    QScopedPointer<QGLMaterialCollectionPrivate> d_ptr;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLMATERIALCOLLECTION_H
