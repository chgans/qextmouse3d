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

#include "qml3dview.h"
#include "item3d.h"
#include "viewport.h"
#include <QtCore/qtimer.h>
#include <QtDeclarative/qmlcomponent.h>
#include <QtDeclarative/qmlcontext.h>

QT_BEGIN_NAMESPACE

/*!
    \class Qml3dView
    \brief The Qml3dView class is based on the QMLView class, and is intended to allow for the
    display of 3d imagery specified in QML/3d.  The Qml3dView takes the place of its parent class
    and contains provisions for display and management of \l Item3d objects, as well as \l Effect
    objects and related classes.
    \since 4.6.?
    \ingroup qt3d
    \ingroup qt3d::qml3d

    \section1 Defining an Qml3dView

    In standard QML the QMLView provides a means of displaying QML items and their supporting
    effects and animations.  For the purposes of 3d display of QML defined items, a more specific
    class is needed which caters to the needs of a Qt3d environment.

    Using the Qml3dView is extremely simple. Consider the code below:

    \code
    int main(int argc, char *argv[])
    {
        QApplication app(argc, argv);

        QString source = argv[index];

        QUrl url(source);
        QFileInfo fi(source);
        if (fi.exists())
            url = QUrl::fromLocalFile(fi.absoluteFilePath());

        Qml3dView view;
        view.setUrl(url);
        view.show();

        return app.exec();
    }
    \endcode

    This is a relatively simple example which shows how easy it can be to use the Qml3dView class.

    The user need only define a source URL, then call the setURL() and the show() functions.

    Internally the class sets the necessary parameters and functions for OpenGL activities, as
    well as user interaction through picking and mouse actions.  Initialisation of scene elements
    and other 3d components is also carried out within the Qml3dView.

    /sa QmlView
*/

/*!
    Construction of the QMl3dView class and assignment of the object parent as \a parent
*/
Qml3dView::Qml3dView(QWidget *parent)
    : QGLView(parent)
{
    m_viewport = 0;
    pickId = 0;
    initGLCalled = false;
    setOption(QGLView::ObjectPicking, true);
}

/*!
    Destruction of the Qml3dView class.
*/
Qml3dView::~Qml3dView()
{
}

/*!
    This function initiates an openGL redraw.
*/
void Qml3dView::updateGL()
{
    QGLView::updateGL();
}

/*!
    Initialise the openGL environment/viewport with \a painter.
*/
void Qml3dView::initializeGL(QGLPainter *painter)
{
    if (m_viewport)
        m_viewport->initialize(this, painter);
    initGLCalled = true;
}

/*!
    Perform early painting/drawing in openGL using \a painter.  This includes such activies as
    clearing, and drawing the backdrop for the viewport.
*/
void Qml3dView::earlyPaintGL(QGLPainter *painter)
{
    if (m_viewport)
        m_viewport->earlyDraw(painter);
    else
        QGLView::earlyPaintGL(painter);
}

/*!
    Repaint the contents of the viewport using the \a painter.
*/
void Qml3dView::paintGL(QGLPainter *painter)
{
    if (m_viewport)
        m_viewport->draw(painter);
}

/*!
    Sets the url which contains the QML source for this environment to \a url.
*/
void Qml3dView::setUrl(const QUrl& url)
{
    // Load the .qml file into a sub-context.
    component = new QmlComponent(&engine, url, this);

    if (!component->isLoading()) {
        loaded();
    } else {
        connect(component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(loaded()));
    }
}

/*!
    Once loaded the parameters and items within the viewport need to be initialised.  This
    function checks for any warnings or errors in loading, and sets up the viewport and its
    encapsulated objects.

    This function will only be called once for a change of status, then it will be disconnected
    from its initiating signal.
*/
void Qml3dView::loaded()
{
    disconnect(component, SIGNAL(statusChanged(QmlComponent::Status)), this, SLOT(loaded()));

    if (component->isError()) {
        QList<QmlError> errors = component->errors();
        foreach (const QmlError &error, errors) {
            qWarning() << error;
        }
    }

    QmlContext *ctx = new QmlContext(&engine);
    QObject *mainObject = component->create(ctx);

    Viewport *viewport = qobject_cast<Viewport *>(mainObject);
    Item3d *item = qobject_cast<Item3d *>(mainObject);

    if (viewport) {
        int width = viewport->width();
        if (width > 0)
            setMinimumWidth(width);
        int height = viewport->height();
        if (height > 0)
            setMinimumHeight(height);
        setOption(QGLView::ObjectPicking, viewport->picking());
        setOption(QGLView::ShowPicking, viewport->showPicking());
        setOption(QGLView::CameraNavigation, viewport->navigation());
        setViewport(viewport);

        QGLPainter painter(this);
        viewport->initialize(this, &painter);

        QTimer::singleShot(0, this, SLOT(updateGL()));
    } else if (item) {
        qWarning() << "qml3d: Item3d object without an enclosing Scene";
    } else if (qobject_cast<QmlGraphicsItem *>(mainObject)) {
        qWarning() << "qml3d: Ordinary QmlGraphicsItem node found; may be "
                      "missing '-graphicssystem OpenGL'";
    } else {
        qWarning() << "qml3d: No Scene or Item3d node found";
    }

    QGLView::initializeGL();
}

/*!
    \fn Viewport *Qml3dView::viewport() const;

    Returns the viewport associated with this view.
*/

/*!
    \fn void Qml3dView::setViewport(Viewport *value);

    Sets the viewport associated with this view to \a value.
*/

/*!
  \fn int Qml3dView::nextPickId();

  Increments the pick ID number and returns this new value.
*/


QT_END_NAMESPACE
