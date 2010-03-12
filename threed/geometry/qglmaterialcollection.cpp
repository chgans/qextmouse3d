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

#include "qglmaterialcollection.h"

#include <QtCore/qset.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLMaterialCollection
    \brief The QGLMaterialCollection class manages groups of materials.
    \since 4.7
    \ingroup qt3d
    \ingroup qt3d::enablers

    Managing more complex 3d graphics with several materials is easier when the
    materials can be referred to as a collection.  This is the role of the
    QGLMaterialCollection class.

    Plug-ins implementing 3D formats may make the materials defined in
    the format available to the application via a QGLMaterialCollection.

    The collection is also optimised for the case where many small objects
    must refer to materials - such as faces in a mesh, or particles.  In
    this case the materials can be specified as a short data type using an
    offset into the collection, rather than the material name.

    The access functions for fetching a QGLMaterialsParameter object by
    the index (the short data type) are inlined and execute in constant
    time.

    When building up a collection, meshes that refer to the various materials
    can check off which ones are used by calling markMaterialAsUsed(), and then
    remove spurious unused materials by calling removeUnusedMaterials().  This
    technique is suitable for models loaded from a model file where a large
    number of materials may be specified but only a few of those materials
    are used by the particular mesh selected from the scene.

    In the case where a given material is mapped 1-to-1 with a given texture
    then the corresponding textures may be stored in the collection also.

    To make a material available from a collection, call addMaterial().  To
    retrieve a material from the collection call removeMaterial().

    To make a texture map to a given material call setTexture(int, QGLTexture2D*)
    and to retrieve that texture for the material use texture(int).

    The collection takes ownership of the QGLMaterial and QGLTexture2D
    objects passed to it by the addMaterial() and setTexture() functions.  These
    objects will be destroyed when the collection is destroyed.
*/

/*!
    Construct a new empty QGLMaterialCollection object.  The \a parent
    is set as the parent of this object.
*/
QGLMaterialCollection::QGLMaterialCollection(QObject *parent)
    : QObject(parent)
{
}

/*!
    Destroy this QGLMaterialCollection object recovering any resources.
    All QGLMaterial and QGLTexture2D objects referenced by the
    collection will be destroyed.  For this reason do not add a
    QGLMaterial or QGLTexture2D object to more than one
    QGLMaterialCollection.
*/
QGLMaterialCollection::~QGLMaterialCollection()
{
    qDeleteAll(QSet<QGLMaterial*>::fromList(mMaterials));
    qDeleteAll(QSet<QGLTexture2D*>::fromList(mTextures));
}

/*!
    \fn QGLMaterial *QGLMaterialCollection::materialByIndex(int index) const
    Returns a pointer to the QGLMaterial object corresponding
    to the \a index given.

    This method executes in constant time, and is intended for fast lookup.
    Use this method during rendering and animation to obtain a material
    to paint with.

    Here's an example of searching for a material with given ambientColor
    \c{color} in a QGLMaterialCollection \c{materials}:

    \code
    int colorIndex = 0;
    for ( ; colorIndex < materials->size(); ++colorIndex)
        if (materialByIndex(colorIndex) &&
                materialByIndex(colorIndex)->ambientColor() == color)
            break;
    if (colorIndex != materials->size())
        myObject->setMaterial(colorIndex);
    \endcode

    If a material has been removed, then the pointer returned will be null,
    so check the return result before using it.
*/

/*!
    \fn int QGLMaterialCollection::materialIndexByName(const QString &name) const
    Returns an index to a QGLMaterial object corresponding to
    the material \a name given.

    This method scans the list of material names, and thus has O(n) complexity.
    Use this method during model loading to set material indexes in a mesh.
*/

/*!
    \fn QString QGLMaterialCollection::materialName(int m) const
    Returns the name of material with index \a m as a QString.  If no such
    index exists (including if \a m is -1) then a null QString is returned.
*/

/*!
    \fn void QGLMaterialCollection::markMaterialAsUsed(int index)
    Flags the material corresponding to the \a index as used.  Some model files
    may contain a range of materials, applying to various objects in the scene.

    When a particular object is loaded from the file, many of those materials may
    not be used in that object.  This wastes space, and makes processing materials
    by name difficult, with many spurious materials being stored.

    Materials flagged as used will not be removed by removeUnusedMaterials(); and
    their names will be returned by the materialNames() method.

    Use this method during model loading or construction to mark off
    materials that have been used.

    \sa removeUnusedMaterials()
*/

/*!
    Removes and deletes materials which have not been marked as used.
    Once this method has been called, the materialNames() method can be called
    with a guarantee that all material names listed are utilized in the model.

    \sa markMaterialAsUsed()
*/
void QGLMaterialCollection::removeUnusedMaterials()
{
    int index = 0;
    QList<bool>::const_iterator it(mMaterialsUsed.begin());
    for ( ; it != mMaterialsUsed.end(); ++it, ++index)
    {
        if (!(*it))
        {
            delete mMaterials[index];
            removeMaterial(index);
        }
    }
}

/*!
    \fn int QGLMaterialCollection::addMaterial(QGLMaterial *material)
    Adds the \a material to this collection and returns its new index.  The
    collection takes ownership of the material and will delete it when the
    collection is destroyed.

    \sa removeMaterial()
*/

/*!
    \fn void QGLMaterialCollection::removeMaterial(QGLMaterial *material)
    Removes the \a material from this collection.  This method simply
    call removeMaterial(int) with the index of the material.

    \sa addMaterial()
*/

/*!
    \fn void QGLMaterialCollection::removeMaterial(int index)
    Removes the material corresponding to \a index from this collection.
    Since the collection is designed for fast lookup by index, the
    internal record is not removed but the pointer stored is set to NULL.
    The material is flagged as not used, and its name is set to empty.
    The collection disowns the \a index and the caller must ensure
    it is deleted.
*/

/*!
    \fn bool QGLMaterialCollection::isEmpty() const
    Returns true if the collection is empty, false otherwise.  If the collection is
    empty then its size is 0.

    \sa size()
*/

/*!
    \fn int QGLMaterialCollection::size() const
    Returns the size of this collection which is the number of
    (possibly NULL) QGLMaterialParamters pointers it contains.

    \sa isEmpty()
*/

/*!
    \internal
    Responds to the deleted() signal by calling removeMaterial on the
    material about to be deleted;
*/
void QGLMaterialCollection::materialDeleted()
{
    QGLMaterial *mat = qobject_cast<QGLMaterial *>(sender());
    removeMaterial(mat);
}

/*!
    \fn void QGLMaterialCollection::setTexture(int index, QGLTexture2D *texture)
    Sets the QGLTexture2D for the given \a index to be \a texture.  This
    collection takes ownership of \a texture and will delete it on destruction.
*/

/*!
    \fn QGLTexture2D *QGLMaterialCollection::texture(int index)
    Returns the QGLTexture2D pointer for the given \a index, or NULL if
    no such pointer has been set.
*/
