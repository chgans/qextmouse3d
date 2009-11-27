/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
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

#include "qglsection.h"

#include <QtGui/qvector3d.h>

/*!
    \class QGLSection
    \brief The QGLSection class clusters like geometry in a QGLDisplayList.
    \since 4.6
    \ingroup qt3d
    \ingroup qt3d::geometry

    QGLSection instances partition a QGLDisplayList into related sections.
    All the vertices in a QGLSection are treated with the same
    \l{QGL::Smoothing}{smoothing}.

    Each QGLSection references a contiguous range of vertices in a
    QGLDisplayList - the start and count of this range are available via
    the start() and count() methods.

    A QGLDisplayList instance has the \l{QGLDisplayList::newSection()}{newSection()}
    function which creates a new QGLSection to reference its data.  Use this
    to construct new QGLSection instances, or alternatively construct
    a new QGLSection() and pass a non-null QGLDisplayList pointer.

    The draw() method draws just the vertices in the range of the section.
    Call the QGLDisplayList::draw() convenience function to draw all the
    QGLSection objects that make up the list.

    All faces in a QGLSection are triangles, although if drawn with the
    addQuad method, the two adjoining triangles making up the quad are made
    without a hard edge between them.  The triangle data can be accessed
    by indexing the QGLDisplayList via the start() and count() methods of
    the QGLSection class:
    \code
    const QVector<QVector3D> &v = displayList->vertexArray();
    qreal area = 0.0;
    int start = section->start();
    int end = start + section->count();
    for (int i = start; i < end; i += 3)
        area += calcTriangleArea(v[i], v[i+1], v[i+2]);
    \endcode
*/

/*!
    \fn QGLSection::QGLSection(QGLDisplayList *d,  QGL::Smoothing s)
    Construct a new QGLSection on the display list \a d, and with smoothing \a s.
    By default the smoothing is QGL::Smooth.

    The \a d must be non-null, and in debug mode, unless QT_NO_DEBUG is
    defined, this function will assert if \a d is null.

    The following lines of code have identical effect:
    \code
    QGLSection *s = myDisplayList->newSection(QGL::Faceted);
    QGLSection *s2 = new QGLSection(myDisplayList, QGL::Faceted);
    \endcode

    The start of the section is initialized to be the first unassigned
    index of the display list, in other words the current size of the
    list.
*/

/*!
    Destroy this QGLSection, recovering any resources.
*/
QGLSection::~QGLSection()
{
}

/*!
    \fn QGL::Smoothing QGLSection::smoothing() const
    Returns the smoothing mode for this section.

    \sa QGL::Smoothing
*/

/*!
   \fn int QGLSection::start() const

   Returns the current start index for this section, as an index into
   the associated QGLDisplayList.

   \sa setStart(), displayList()
*/

/*!
    \fn void QGLSection::setStart(int s)

    Sets the current start index for this section, to \a s, an index into
    the associated QGLDisplayList

    \sa start(), displayList()
*/

/*!
   \fn int QGLSection::count() const

   Returns the current index count for this section, as a count of indexed
   vertices in the associated QGLDisplayList.

   While this section is not finalized this value may increase as new
   geometry is added to it.

   \sa displayList()
*/

/*!
    \fn QGLDisplayList *QGLSection::displayList() const

    Returns the display list associated with this section.
*/

/*!
    \fn void QGLSection::draw(QGLPainter *painter) const
    Draws this section on the \a painter.
*/

/*!
    \fn QString QGLSection::name() const

    Returns the QString name assigned to this section, or a null QString
    if no name has been assigned.  The name may be used to locate a
    section by name, or to capture model domain information.  It is also
    printed out by the QDebug stream operator.

    \sa setName()
*/

/*!
    \fn void QGLSection::setName(const QString &name)

    Sets the QString \a name to be assigned to this section.

    \sa name()
*/

#ifndef QT_NO_DEBUG_STREAM
/*!
    Output a string representation of the QGLSection to a debug stream.
    \relates QGLSection
*/
QDebug operator<<(QDebug dbg, const QGLSection &section)
{
    dbg.space()
            << "QGLSection("
            << ", start:" << section.start()
            << ", count:" << section.count()
            << ", smoothing mode:" << (section.smoothing() == QGL::Smooth ?
                                       "QGL::Smooth" : "QGL::Faceted");
    const QGLDisplayList * const dl = section.displayList();
    QGLDisplayList::VectorArray verts = dl->vertexArray();
    QGLDisplayList::IndexArray indices = dl->indexArray();
    QGLDisplayList::ColorArray colors = dl->colorArray();
    QGLDisplayList::VectorArray normals = dl->normalArray();
    QGLDisplayList::TexCoordArray texCoords = dl->texCoordArray();
    for (int i = 0; i < section.count(); ++i)
    {
        int ix = i + section.start();
        GLuint ptr = indices.at(ix);
        const QVector3D &v = verts.at(ptr);
        dbg.nospace() << "   " << ptr << " -> " << v;
        if (dl->hasColors())
            dbg << "color:" << colors.at(ptr);
        if (dl->hasNormals())
            dbg << "normal:" << normals.at(ptr);
        if (dl->hasTexCoords())
            dbg << "texCoord:" << texCoords.at(ptr);
        dbg.space() << "\n";
    }
    dbg << ")\n";
    return dbg.space();
}
#endif
