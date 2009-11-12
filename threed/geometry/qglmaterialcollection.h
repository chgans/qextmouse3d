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

#ifndef QGLMATERIALCOLLECTION_H
#define QGLMATERIALCOLLECTION_H

#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

#include "qt3dglobal.h"
#include "qglmaterialparameters.h"
#include "qgltexture2d.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3d)

class QGLMaterialCollectionPrivate;
class QGLTexture2D;

class Q_QT3D_EXPORT QGLMaterialCollection : public QObject
{
    Q_OBJECT
public:
    QGLMaterialCollection(QObject *parent = 0);
    virtual ~QGLMaterialCollection();
    inline QGLMaterialParameters *materialByIndex(int) const;
    inline int materialIndexByName(const QString &) const;
    inline QString materialName(int) const;
    inline void markMaterialAsUsed(int);
    void removeUnusedMaterials();
    inline int addMaterial(QGLMaterialParameters *);
    inline void removeMaterial(QGLMaterialParameters *);
    inline void removeMaterial(int);
    inline bool isEmpty() const;
    inline int size() const;
    inline void setTexture(int, QGLTexture2D *);
    inline QGLTexture2D *texture(int);

private slots:
    void materialDeleted();

private:
    QList<QGLMaterialParameters *> mMaterials;
    QList<QGLTexture2D *> mTextures;
    QStringList mMaterialNames;
    QList<bool> mMaterialsUsed;
};

inline QGLMaterialParameters *QGLMaterialCollection::materialByIndex(int ix) const
{
    return mMaterials.at(ix);
}

inline int QGLMaterialCollection::materialIndexByName(const QString &name) const
{
    if (name.isEmpty() || mMaterials.count() == 0)
        return -1;
    return mMaterialNames.indexOf(name);
}

inline QString QGLMaterialCollection::materialName(int m) const
{
    if (m >= 0 && m < mMaterialsUsed.count())
        return mMaterialNames[m];
    return QString();
}

inline void QGLMaterialCollection::markMaterialAsUsed(int m)
{
    if (m >= 0 && m < mMaterialsUsed.count())
        mMaterialsUsed[m] = true;
}

inline int QGLMaterialCollection::addMaterial(QGLMaterialParameters *mat)
{
    int index = mMaterials.count();
    mMaterials.append(mat);
    mMaterialNames.append(mat->objectName());
    mMaterialsUsed.append(true);
    mTextures.append(0);
    connect(mat, SIGNAL(destroyed()), this, SLOT(materialDeleted()));
    return index;
}

inline void QGLMaterialCollection::removeMaterial(QGLMaterialParameters *material)
{
    int index = mMaterials.indexOf(material);
    if (index == -1)
        return;
    removeMaterial(index);
}

inline void QGLMaterialCollection::removeMaterial(int index)
{
    mMaterialNames[index] = "";    // no tr
    mMaterialsUsed[index] = false;
    mMaterials[index] = 0;
    mTextures[index] = 0;
}

inline bool QGLMaterialCollection::isEmpty() const
{
    return mMaterials.isEmpty();
}

inline int QGLMaterialCollection::size() const
{
    return mMaterials.size();
}

inline void QGLMaterialCollection::setTexture(int index, QGLTexture2D *texture)
{
    mTextures[index] = texture;
}

inline QGLTexture2D *QGLMaterialCollection::texture(int index)
{
    if (index == -1)
        return 0;
    return mTextures.at(index);
}

QT_END_NAMESPACE

QT_END_HEADER

#endif // QGLMATERIALCOLLECTION_H
