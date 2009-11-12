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

#include "qglabstractscene.h"
#include "qglsceneformatplugin.h"
#include <QtCore/qfile.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/private/qfactoryloader_p.h>
#include <QtNetwork/qnetworkreply.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLAbstractScene
    \brief The QGLAbstractScene class represents a 3D scene consisting of zero or more QGLSceneObject instances.
    \ingroup qt3d
    \ingroup qt3d::scene

    Scenes are typically created by 3D modelling packages and then loaded
    into the application via a QGLSceneFormatPlugin.  The functions in this
    class provide access to major scene objects so that they can be
    applied or drawn whenever the application decides.

    Because of the high degree of variation between 3D modelling packages,
    QGLAbstractScene presents a very simple model of a 3D scene.  Subclasses
    implement the specific scene graph and object representations in a
    manner specific to the package's format.  Subclasses may also provide
    functionality to create new objects at runtime, but this is not required.

    The scene is defined to consist of a number of important objects
    of the following types:

    \list
    \o Camera objects define a viewing position in world-coordinates and a
       projection that maps 3D world co-ordinates to 2D screen co-ordinates.
       It is recommended that camera objects inherit from QGLCamera.
    \o Main objects designate the major elements of the scene besides
       cameras, lights, and effects.
    \o Light objects define positions and parameters for lighting the scene.
       It is recommended that light objects inherit from QGLLightParameters.
    \o Effect objects define materials, shaders, and textures for use in
       rendering the surface of objects.  Normally effects are activated
       automatically when main objects are drawn.  But effects can be used
       independently if the 3D format is acting as a library of effects.
    \o Mesh objects define geometry information independently of effects.
       Normally meshes are drawn automatically with an appropriate effect
       when main objects are drawn.  But meshes can be used independently
       if the 3D format is acting as a library of meshes.
    \o World objects define collections of objects such that the entire
       scene can be drawn just by drawing the World object.
    \endlist

    Typically, the full scene represented by the World object is not
    interesting to the application.  3D modelling packages regularly
    insert cameras, lights, effects, and other library objects that
    are useful to the modelling package, but not the application.
    The Main objects are usually the most interesting to applications.

    QGLAbstractScene makes it easy to access the major scene elements
    with object(), objects(), and defaultObject().

    There are many other kinds of objects in the scene that may not
    be accessible via QGLAbstractScene because they are not considered
    "important" enough.  For example, a file that contains the data
    for a skateboard object would contain many objects for the board,
    wheels, texturing effects, animations, and so on.  The application
    may only be interested in the skateboard as a whole, and not its
    sub-components.  The skateboard would be considered an important
    Main object in this case, which can be easily accessed and
    incorporated into the application's logic.

    Each Subclass needs to provide its own policy for deciding which
    objects are considered "important".

    \sa QGLSceneObject, QGLSceneFormatPlugin
*/

/*!
    Constructs a 3D scene and attaches it to \a parent.
*/
QGLAbstractScene::QGLAbstractScene(QObject *parent)
    : QObject(parent)
{
    d_ptr = 0;  // Reserved for future use.
}

/*!
    Destroys this 3D scene.
*/
QGLAbstractScene::~QGLAbstractScene()
{
}

/*!
    \fn QList<QGLSceneObject *> QGLAbstractScene::objects(QGLSceneObject::Type type) const

    Returns a list of all objects in the scene that have the specified \a type
    and which are considered important.

    Important objects will typically be the world object, cameras, lights,
    and other top-level objects.  Sub-meshes and effects are normally
    not considered important unless the scene is acting as a library of
    meshes and effects.

    \sa objectNames(), object(), defaultObject()
*/

/*!
    Returns a list of the names of all objects in the scene that have the
    specified \a type, which are considered important, and which have
    non-empty names associated with them.

    The default implementation calls objects() and then compiles a list
    of all non-empty object names.

    \sa objects()
*/
QStringList QGLAbstractScene::objectNames(QGLSceneObject::Type type) const
{
    QList<QGLSceneObject *> objs = objects(type);
    QStringList names;
    foreach (QGLSceneObject *object, objs) {
        if (object) {
            QString name = object->objectName();
            if (!name.isEmpty())
                names += name;
        }
    }
    return names;
}

/*!
    Returns the scene object that has the specified \a type and \a name;
    or null if the object was not found.

    The default implementation searches objects() for an object that
    matches \a name.

    \sa objects()
*/
QGLSceneObject *QGLAbstractScene::object
        (QGLSceneObject::Type type, const QString& name) const
{
    if (name.isEmpty())
        return 0;
    QList<QGLSceneObject *> objs = objects(type);
    foreach (QGLSceneObject *object, objs) {
        if (object && object->objectName() == name)
            return object;
    }
    return 0;
}

/*!
    Returns the default object with the specified \a type.  This is typically
    used to find objects like the world and camera objects that are usually
    unique within the scene.  Returns null if no default object for \a type
    was found.

    The default implementation returns the first element of objects(),
    or null if objects() returns an empty list.

    \sa objects()
*/
QGLSceneObject *QGLAbstractScene::defaultObject(QGLSceneObject::Type type)
{
    QList<QGLSceneObject *> objs = objects(type);
    if (objs.size() >= 1)
        return objs[0];
    else
        return 0;
}

#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader,
    (QGLSceneFormatFactoryInterface_iid, QLatin1String("/sceneformats")))
#endif

/*!
    Loads a scene from \a device in the specified \a format using
    the registered scene format plugins.  If \a format is an empty
    string, then the format will be autodetected from the filename
    extension of \a device.  The \a url specifies the location of
    the data in \a device so that relative resources can be located.

    Returns the scene object, or null if the scene could not be loaded
    or the \a format was not supported by any of the plugins.

    \sa QGLSceneFormatPlugin
*/
QGLAbstractScene *QGLAbstractScene::loadScene
    (QIODevice *device, const QUrl& url, const QString& format)
{
#if !defined (QT_NO_LIBRARY) && !defined(QT_NO_SETTINGS)
    if (!device)
        return 0;

    QFactoryLoader *l = loader();
    QStringList keys = l->keys();

    // If the format is not specified, then use the filename/url extension.
    QString fmt = format;
    if (fmt.isEmpty()) {
        QFile *file = qobject_cast<QFile *>(device);
        QString name;
        if (file) {
            name = file->fileName();
        } else {
            QNetworkReply *reply = qobject_cast<QNetworkReply *>(device);
            if (reply)
                name = reply->url().path();
        }
        int dot = name.lastIndexOf(QLatin1Char('.'));
        QString suffix = name.mid(dot+1).toLower();
        int index = keys.indexOf(suffix);
        if (index >= 0)
            fmt = suffix;
    }

    // Find the plugin that handles the format and ask it to create a handler.
    if (QGLSceneFormatFactoryInterface *factory
            = qobject_cast<QGLSceneFormatFactoryInterface*>
                (l->instance(fmt))) {
        QGLSceneFormatHandler *handler = factory->create(device, url, fmt);
        if (handler) {
            QGLAbstractScene *scene = handler->read();
            delete handler;
            return scene;
        }
    }

    // If we get here, then the format is not supported by any of the plugins.
    return 0;
#else // QT_NO_LIBRARY || QT_NO_SETTINGS
    Q_UNUSED(device);
    Q_UNUSED(url);
    Q_UNUSED(format);
    return 0;
#endif // QT_NO_LIBRARY || QT_NO_SETTINGS
}

/*!
    Loads a scene from \a fileName in the specified \a format using
    the registered scene format plugins.  If \a format is an empty
    string, then the format will be autodetected from the extension
    of \a fileName.

    Returns the scene object, or null if the scene could not be loaded
    or the \a format was not supported by any of the plugins.

    \sa QGLSceneFormatPlugin
*/
QGLAbstractScene *QGLAbstractScene::loadScene
    (const QString& fileName, const QString& format)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return 0;
    QFileInfo fi(fileName);
    QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
    return loadScene(&file, url, format);
}

QT_END_NAMESPACE
