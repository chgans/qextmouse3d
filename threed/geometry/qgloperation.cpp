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

#include "qgloperation.h"

/*!
    \class QGLOperation
    \brief The QGLOperation class provides an OO interface to QGLDisplayList
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    Use QGLOperation if an object based API to QGLDisplayList is
    preferred, to have stack based objects provide automatic begin/end
    semantics, or to use stream operators for some data.

    The QGLDisplayList class provides an API which is similar to OpenGL
    in that all functionality is accessed via function calls.  Where in
    OpenGL the state is held in an implicit context, the QGLDisplayList
    instance is an object on which all the function calls are made.

    QGLOperation simply provides some syntactic niceness and convenience
    so that the function calls

    \code
    // QGLDisplayList native API example
    displayList->begin(QGL::Quad);
    displayList->addVertex(QVector3D(0, 0, 0));
    displayList->addVertex(QVector3D(1, 0, 0));
    displayList->addVertex(QVector3D(1, 1, 0));
    displayList->addVertex(QVector3D(0, 1, 0));
    displayList->end();
    \endcode

    compared with:

    \code
    // QGLOperation version
    {
        QGLOperation op(displayList, QGL::Quad);
        op << QVector3D(0, 0, 0);
        op << QVector3D(1, 0, 0);
        op << QVector3D(1, 1, 0);
        op << QVector3D(0, 1, 0);
    }
    \endcode

    In the QGLOperation version, a new scope is started with a brace,
    and the \c{op} is created on the stack in that scope.  At the closing
    brace, the \c{op} variable goes out of scope and its destructor calls
    displayList->end() automatically.

    Here stream operators make the code easier to read.  Stream operators
    are provided for a few of the most common calls:

    \table
        \header
            \o Datatype streamed to QGLOperation \c{op << d}
            \o Corresponding function called
        \row
            \o \c{const QVector3D &d}
            \o \c{op.addVertex(d)}
        \row
            \o \c{const QVector2D &d}
            \o \c{op.addTexCoord(d, QGL::TextureCoord0)}
        \row
            \o \c{const QColor4b &d}
            \o \c{op.addColor(d)}
        \row
            \o \c{const QDataArray<QVector3D> &d}
            \o \c{op.addVertexArray(d)}
        \row
            \o \c{const QDataArray<QVector2D> &d}
            \o \c{op.addTexCoordArray(d, QGL::TextureCoord0)}
        \row
            \o \c{const QDataArray<QColor4b> &d}
            \o \c{op.addColorArray(d)}
        \row
            \o \c{const QGL::OperationFlags &d}
            \o \c{op.setFlags(d | op.flags())}
    \endtable

    Care should be taken moving from QGLDisplayList to QGLOperation:
    \code
    // current QGLDisplayList code
    displayList->addVertex(QVector2D(1, 0)); // calls implicit QVector3D constructor

    // WRONG
    QGLOperation op(displayList, QGL::Quad);
    op << QVector2D(1, 0);  // interpreted as a texture coordinate
    \endcode

*/

/*!
    \fn QGLOperation(QGLDisplayList *displayList, QGL::Operation operation)
    Construct a new QGLOperation that will call functions on \a displayList
    to perform \a operation.
*/

/*!
    \fn QGLOperation(QGLDisplayList *displayList, QGL::Operation operation)
    Construct a new QGLOperation that will call functions on \a displayList
    to perform \a operation.
*/

/*!
    \fn ~QGLOperation()
    Destroy this QGLOperation, recovering any resources.  This function also
    takes care of calling QGLDisplayList::end() by calling the end() function.
*/

/*!
    \fn void setFlags(QGL::OperationFlags flags)
    Sets the \a flags for this operation by calling
    QGLDisplayList::setFlags()
*/

/*!
    \fn QGL::OperationFlags flags() const
    Returns the currently set flags by calling QGLDisplayList::flags()
*/

/*!
    \fn void addVertex(const QVector3D &vertex)
    Adds a \a vertex to this operation by calling
    QGLDisplayList::addVertex()
*/

/*!
    \fn void addNormal(const QVector3D &normal)
    Adds a \a normal to this operation by calling
    QGLDisplayList::addNormal()
*/

/*!
    \fn void addColor(const QColor4b &color)
    Adds a \a color to this operation by calling
    QGLDisplayList::addColor()
*/

/*!
    \fn void addTexCoord(const QVector2D &texCoord, QGL::VertexAttribute attribute)
    Adds a texture coordinate \a texCoord \a attribute to this operation by calling
    QGLDisplayList::addTexCoord(texCoord, attribute)
*/

/*!
    \fn void addAttribute(const QVector3D &value, QGL::VertexAttribute attribute)
    Adds a custom attribute \a value \a attribute to this operation by calling
    QGLDisplayList::addAttribute(value, attribute)
*/

/*!
    \fn void addVertexArray(const QDataArray<QVector3D> &vertexArray)
    Adds \a vertexArray to this operation by calling
    QGLDisplayList::addVertexArray(vertexArray)
*/

/*!
    \fn void addNormalArray(const QDataArray<QVector3D> &normalArray)
    Adds \a normalArray to this operation by calling
    QGLDisplayList::addNormalArray(normalArray)
*/

/*!
    \fn void addColorArray(const QDataArray<QColor4b> &colorArray)
    Adds \a colorArray to this operation by calling
    QGLDisplayList::addColorArray(colorArray)
*/

/*!
    \fn void addTexCoordArray(const QDataArray<QVector2D> &texCoordArray, QGL::VertexAttribute attribute)
    Adds \a texCoordArray at \a attribute to this operation by calling
    QGLDisplayList::addTexCoordArray(texCoordArray, attribute)
*/

/*!
    \fn void addAttributeArray(const QDataArray<QVector3D> &attributeArray, QGL::VertexAttribute attribute)
    Adds a \a attributeArray at \a attribute to this operation by calling
    QGLDisplayList::addAttributeArray(attributeArray, attribute)
*/

/*!
    \fn void end()
    Completes this operation by calling QGLDisplayList::end().  This function is
    called automatically by ~QGLOperation.
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QDataArray<QVector3D> &ary)
    \relates QGLOperation
    Calls QGLOperation::addVertexArray() on \a op with argument \a ary.
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QDataArray<QVector2D> &ary)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QDataArray<QColor4b> &ary)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QDataArrayRef<QVector3D> &ary)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QDataArrayRef<QVector2D> &ary)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QDataArrayRef<QColor4b> &ary)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QVector3D &vertex)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QVector2D &texCoord)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op, const QColor4b &color)
    \relates QGLOperation
*/

/*!
    \fn QGLOperation &operator<<(QGLOperation &op,
                                const QGL::OperationFlag &flag)
    \relates QGLOperation
*/
