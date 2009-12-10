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

#include "qgeometrydata_p.h"

#include <QtCore/qdebug.h>

QGeometryData::QGeometryData(QLogicalVertex::Types type)
    : m_normals(0)
    , m_texCoords(0)
    , m_colors(0)
    , m_dataTypes(type)
{
}

QGeometryData::~QGeometryData()
{
    delete m_normals;
    delete m_texCoords;
    delete m_colors;
}

void QGeometryData::reserve(int capacity)
{
    m_vertices.reserve(capacity);
    m_indices.reserve(capacity * 3);
    if (m_normals)
        m_normals->reserve(capacity);
    if (m_texCoords)
        m_texCoords->reserve(capacity);
    if (m_colors)
        m_colors->reserve(capacity);
}

void QGeometryData::squeeze()
{
    m_vertices.squeeze();
    m_indices.squeeze();
    if (m_normals)
        m_normals->squeeze();
    if (m_texCoords)
        m_texCoords->squeeze();
    if (m_colors)
        m_colors->squeeze();
}


/*!
    \internal
    Use QGLVertexArray as an intermediary between display list and painter.
*/
QGLVertexArray QGeometryData::toVertexArray() const
{
    QGLVertexArray array;
    array.addField(QGL::Position, 3);
    if (hasType(QLogicalVertex::Normal))
        array.addField(QGL::Normal, 3);
    if (hasType(QLogicalVertex::Texture))
        array.addField(QGL::TextureCoord0, 2);
    if (hasType(QLogicalVertex::Color))
        array.addField(QGL::Color, 1);
    for (int i = 0; i < m_vertices.count(); ++i)
    {
        array.append(m_vertices.at(i));
        if (hasType(QLogicalVertex::Normal))
            array.append(m_normals->at(i));
        if (hasType(QLogicalVertex::Texture))
            array.append(m_texCoords->at(i));
        if (hasType(QLogicalVertex::Color))
            array.append(m_colors->at(i));
    }
    return array;
}

/*!
    \internal
    Normalize all the m_normals.
*/
void QGeometryData::normalizeNormals()
{
    if (hasType(QLogicalVertex::Normal))
    {
        for (int i = 0; i < m_normals->count(); ++i)
            (*m_normals)[i].normalize();
    }
}
